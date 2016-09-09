/*
 * Building.cpp
 *
 *  Created on: May 30, 2016
 *      Author: m26jain
 */

#include "Weather.h"
#include "Building.h"
#include "Occupancy.h"
#include "ThermalModel.h"
//#include "WriteOutput.h"

#include<stdio.h>
#include<iostream>
#include <fstream>

using namespace SimpleBuildingSimulator;

/* Building(): Constructor that initializes all the
 * parameters with their default values when we create
 * a new object of the Building Class.
 */
Building::Building() {

	/* Thermal Capacities */
	ParamsIn.CommonRoom.C = 2000;					// Thermal Capacity of Room (kJ/K)
	ParamsIn.CommonRoom.C_ = 200;					// Thermal Capacity of SPOT Region (kJ/K)

	/* Heat Transfer Coefficients */
	ParamsIn.CommonRoom.alpha_o = 0.048f;			// Heat Transfer Coefficient for Outside (kJ/K.s)
	ParamsIn.CommonRoom.alpha_r = 0.1425f;			// Heat Transfer Coefficient for Regions (kJ/K.s)

	/* Heat Loads */
	ParamsIn.CommonRoom.Q_l = 0.1f;					// Heat Load Due to Lightening and Equipments (kW)
	ParamsIn.CommonRoom.Q_h = 0.1f;					// Heat Load Due to Presence of Occupants (kW)
	ParamsIn.CommonRoom.Q_s = 0.7f;					// Heat Load of SPOT Unit (kW)

	/* Impact of Fan */
	ParamsIn.CommonRoom.fan_coef = 0.094f;			// Coefficient of Fan Power Consumption (-)

	/* Parameters for Air */
	ParamsIn.CommonAir.density = 1.225f;			// Density of Air (kg/m3)
	ParamsIn.CommonAir.specific_heat = 1.003f;		// Specific Heat Capacity of Air (J/Kg-K)

	/* PMV Parameters */
	ParamsIn.PMV_Params.P1 = 0.2466f;				// 1st Coefficient of Learned PMV Equation (-)
	ParamsIn.PMV_Params.P2 = 1.4075f;				// 2nd Coefficient of Learned PMV Equation (-)
	ParamsIn.PMV_Params.P3 = 0.581f;				// 3rd Coefficient of Learned PMV Equation (-)
	ParamsIn.PMV_Params.P4 = 5.4668f;				// 4th Coefficient of Learned PMV Equation (-)

	/* AHU Efficiencies */
	ParamsIn.CommonAHU.HeatingEfficiency = 0.9f;	// Heating Efficiency of AHU (-)
	ParamsIn.CommonAHU.CoolingEfficiency = 0.9f;	// Cooling Efficiency of AHU (-)

	/* Error Values */
	ParamsIn.CommonErrors.err_bparams = 0.02;		// Error in Building Parameters (-)
	ParamsIn.CommonErrors.err_text = 0.02;			// Error in Weather Conditions (-)

	/* Files */
	ParamsIn.Files.weather_file = "";				// Input File to Read Weather Data (-)
	ParamsIn.Files.occupancy_file = "";				// Input File to Read Occupancy Data (-)
	ParamsIn.Files.merged_data_file = "";				// Input File to Read Occupancy Data (-)
	ParamsIn.Files.output_file = "";				// Output File Created by the Program (-)

	/* Other Parameters */
	ParamsIn.CommonBuilding.num_zones_ = 1;			// Number of VAV Zones in the Building (-)
	ParamsIn.CommonBuilding.num_rooms_ = 1;			// Number of Rooms in each VAV Zone (-)
	skip_lines = 1;									// Initial number of lines to skip in data files (-)
}

/* Destructor for the Building Class */
Building::~Building() {
}

/* Simulate() takes following input from the user:
 * 1. Start: Time to initiate simulation (By Default: Start of Weather/Occupancy Data)
 * 2. Stop: Time to stop the simulation (By Default: End of Weather/Occupancy Data)
 * 3. Time Step: Sampling rate for simulation
 * 4. Control Type: Three control algorithms implemented in the Simulator
 * 		1 - Default (Majorly to monitor correct working of the simulator)
 * 		2 - Reactive Control
 * 		3 - MPC Based Control
 * 5. Horizon - Predictions will be done only for Horizon Interval.
 * */
void Building::Simulate(time_t &start_t, time_t &stop_t, const int& time_step_mpc, const int& time_step_spot,
		const int& control_type, const int& horizon) {

	// Total number of rooms to simulate
	int total_rooms = ParamsIn.CommonBuilding.num_zones_ * ParamsIn.CommonBuilding.num_rooms_;

	// Create Objects for Weather and Occupancy Class
	Weather weather;
	Occupants occupancy;

	// Parse Weather and Occupancy Data from Input Files
	DF_FLOAT df_weather_mpc;
	weather.ParseWeatherData(df_weather_mpc, ParamsIn.Files.weather_file, start_t, stop_t, time_step_mpc, skip_lines);

	DF_FLOAT df_weather_spot;
	weather.ParseWeatherData(df_weather_spot, ParamsIn.Files.weather_file, start_t, stop_t, time_step_spot, skip_lines);

	DF_INT2 df_occupancy_mpc;
	occupancy.ParseOccupancyData(df_occupancy_mpc, total_rooms, ParamsIn.Files.occupancy_file, start_t, stop_t, time_step_mpc, skip_lines);

	DF_INT2 df_occupancy_spot;
	occupancy.ParseOccupancyData(df_occupancy_spot, total_rooms, ParamsIn.Files.occupancy_file, start_t, stop_t, time_step_spot, skip_lines);

	std::cout << "Start: " << start_t << ", Stop: " << stop_t << "\n";

	// Use updated start and stop time to compute duration and number of time instances (n)
	long int duration = stop_t - start_t;
	int time_step_ratio = time_step_mpc / time_step_spot;

	long int n_mpc = (duration / time_step_mpc) + 1;
	long int n_spot = (duration / time_step_spot) + 1;

	std::cout << "Duration: " << duration << ", #MPC: " << n_mpc << ", #SPOT: " << n_spot << "\n";

	// Output of the Simulation
	DF_OUTPUT df[n_spot];

	size_t j = 0;		// Index for each time stamp
	for (time_t i = start_t; i <= stop_t; i = i + time_step_spot) {
		df[j].t = i;						// Epoch Time
		df[j].weather = df_weather_spot[i];		// External Temperature
		df[j].weather_err = df_weather_spot[i];		// External Temperature
		df[j].power = 0.0f;
		df[j].r = 0.0f;
		df[j].tmix = 0.0f;
		df[j].response = 0.0f;

		df[j].occ = new int[total_rooms];
		df[j].ppv = new float[total_rooms];
		df[j].tspot = new float[total_rooms];
		df[j].tnospot = new float[total_rooms];
		df[j].spot_status = new int[total_rooms];

		for (size_t room = 0; room < (size_t) total_rooms; room++) {
			df[j].occ[room] = df_occupancy_spot[i][room];		// Occupancy in the Room
			df[j].ppv[room] = 0.0f;
			df[j].tspot[room] = 0.0f;
			df[j].tnospot[room] = 0.0f;
			df[j].spot_status[room] = 0;
		}
		j = j + 1;
	}

	std::cout << "Value of j: " << j << "\n";

	if( remove( ParamsIn.Files.merged_data_file.c_str() ) != 0 )
		perror( "Error deleting file" );
	else
		puts( "File successfully deleted" );

	// Write to Intermediate File
	std::fstream mf;
	mf.open(ParamsIn.Files.merged_data_file.c_str(), std::fstream::in | std::fstream::out | std::fstream::app);
	for (size_t j = 0; j < (size_t) n_spot; j++) {
		mf << df[j].t << "," << df[j].weather;
		for (size_t room = 0; room < (size_t) total_rooms; room++) {
				mf << "," << df[j].occ[room];
		}
		mf << "\n";
	}
	mf.close();

	// Convert DataFrame format to Matrix format for computations
	MAT_FLOAT T_ext_mpc = weather.GetWeatherMatrix(df_weather_mpc, n_mpc, total_rooms);
	MAT_FLOAT T_ext_spot = weather.GetWeatherMatrix(df_weather_spot, n_spot, total_rooms);

	MAT_FLOAT O_mpc = occupancy.GetOccupancyMatrix(df_occupancy_mpc, n_mpc, total_rooms).cast<float>();
	MAT_FLOAT O_spot = occupancy.GetOccupancyMatrix(df_occupancy_spot, n_spot, total_rooms).cast<float>();

	// Simulate as per Thermal Model
	ModelRachel MPCModel;

	MPCModel.SimulateModel(df, T_ext_mpc, T_ext_spot, O_mpc, O_spot, ParamsIn, time_step_mpc, time_step_spot,
			total_rooms, n_mpc, n_spot, control_type, horizon);

	// Write to Output File
	if( remove( ParamsIn.Files.output_file.c_str() ) != 0 )
		perror( "Error deleting file" );
	else
		  puts( "File successfully deleted" );

	std::fstream ff;
	ff.open(ParamsIn.Files.output_file.c_str(), std::fstream::in | std::fstream::out | std::fstream::app);

	// Titles for file
	ff << "Response,Timestamp,T_ext,T_ext_err,Power,Ratio,T_mix";
	for (size_t room = 0; room < (size_t) total_rooms; room++) {
		ff << ",Occ_" << room + 1;
		ff << ",PPV_" << room + 1;
		ff << ",T_SPOT_" << room + 1;
		ff << ",T_NoSPOT_" << room + 1;
		ff << ",SPOT_" << room + 1;
	}
	ff << "\n";
	for (size_t j = 0; j < (size_t) n_spot; j++) {
		ff << df[j].response << "," << df[j].t << "," << df[j].weather << "," << df[j].weather_err << "," <<
				df[j].power << "," << df[j].r << "," << df[j].tmix;
		for (size_t room = 0; room < (size_t) total_rooms; room++) {
			ff << "," << df[j].occ[room];
			ff << "," << df[j].ppv[room];
			ff << "," << df[j].tspot[room];
			ff << "," << df[j].tnospot[room];
			ff << "," << df[j].spot_status[room];
		}
		ff << "\n";
	}
	ff.close();

	//WriteOutput writer;
	//writer.WriteOutputCSV(duration, time_step, num_zones_, num_rooms_, T, TR1,
	//		TR2, DeltaTR1, DeltaTR2, PPV, MixedAirTemperature, PowerAHU, O,
	//		T_ext, SPOT_State, CommonRoom, CommonAHU, CommonAir, PMV_Params, Files.output_file);
}
