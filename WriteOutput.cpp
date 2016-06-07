/*
 * WriteOutput.cpp
 *
 *  Created on: Jun 7, 2016
 *      Author: m26jain
 */

#include <iostream>
#include <fstream>

#include "WriteOutput.h"

using namespace SimpleBuildingSimulator;

WriteOutput::WriteOutput() {
}

WriteOutput::~WriteOutput() {
}

void WriteOutput::WriteOutputCSV(long int duration, int time_step,
		int num_zones, int num_rooms, Eigen::MatrixXf T, Eigen::MatrixXf TR1,
		Eigen::MatrixXf TR2, Eigen::MatrixXf DeltaTR1, Eigen::MatrixXf DeltaTR2,
		Eigen::MatrixXf PPV, Eigen::MatrixXf MixedAirTemperature,
		Eigen::MatrixXf PowerAHU, Eigen::MatrixXf Occupancy,
		Eigen::MatrixXf T_ext, Eigen::MatrixXi SPOT_State,
		Building::Room CommonRoom, Building::AHU CommonAHU,
		Building::Air CommonAir, Building::PMV_Model PMV_Params) {

	long int tinstances = duration / time_step;
	int total_rooms = num_zones * num_rooms;

	std::ofstream myfile;
	myfile.open("output/simulation.csv");

	/* Input Parameters of the Simulation */
	myfile << "# Number of Zones: " << num_zones << "\n";
	myfile << "# Number of Rooms: " << num_rooms << "\n";

	myfile << "# Simulation Duration: " << duration << " seconds\n";
	myfile << "# Time Step: " << time_step << " seconds \n";
	myfile << "# Number of Time Instances: " << tinstances << "\n";

	myfile << "# Thermal Capacity of Room (C): " << CommonRoom.C << " kJ/K \n";
	myfile << "# Thermal Capacity of SPOT Region (C_): " << CommonRoom.C_
			<< " kJ/K \n";

	myfile << "# Heat Transfer Coefficient for Outside (alpha_o): "
			<< CommonRoom.alpha_o << " kJ/K.s \n";
	myfile << "# Heat Transfer Coefficient for Regions (alpha_r): "
			<< CommonRoom.alpha_r << " kJ/K.s \n";

	myfile << "# Heat Load Due to Lightening and Equipments (Q_l): "
			<< CommonRoom.Q_l << " kW \n";
	myfile << "# Heat Load Due to Presence of Occupants (Q_h): "
			<< CommonRoom.Q_h << " kW \n";
	myfile << "# Heat Load of SPOT Unit (Q_s): " << CommonRoom.Q_s << " kW \n";

	myfile << "# Coefficient of Fan Power Consumption (fan_coef): "
			<< CommonRoom.fan_coef << "\n";

	myfile << "# Density of Air (density): " << CommonAir.density
			<< " kg/m3 \n";
	myfile << "# Specific Heat Capacity of Air (specific_heat): "
			<< CommonAir.specific_heat << " kJ/kg.K \n";

	myfile << "# Parameter-1 of Learned PMV Model: " << PMV_Params.P1 << "\n";
	myfile << "# Parameter-2 of Learned PMV Model: " << PMV_Params.P2 << "\n";
	myfile << "# Parameter-3 of Learned PMV Model: " << PMV_Params.P3 << "\n";
	myfile << "# Parameter-4 of Learned PMV Model: " << PMV_Params.P4 << "\n";

	myfile << "# Heating Efficiency of AHU: " << CommonAHU.HeatingEfficiency
			<< "\n";
	myfile << "# Cooling Efficiency of AHU: " << CommonAHU.CoolingEfficiency
			<< "\n\n";

	for (int j = 0; j < total_rooms; j++) {
		myfile << "S_{SPOT}^" << j + 1 << ", ";
	}
	for (int j = 0; j < total_rooms; j++) {
		myfile << "Occupancy^" << j + 1 << ", ";
	}
	for (int j = 0; j < total_rooms; j++) {
		myfile << "T_{HVAC}^" << j + 1 << ", ";
	}
	for (int j = 0; j < total_rooms; j++) {
		myfile << "T_{SPOT}^" << j + 1 << ", ";
	}
	for (int j = 0; j < total_rooms; j++) {
		myfile << "T_{NoSPOT}^" << j + 1 << ", ";
	}
	for (int j = 0; j < total_rooms; j++) {
		myfile << "DeltaT_{SPOT}^" << j + 1 << ", ";
	}
	for (int j = 0; j < total_rooms; j++) {
		myfile << "DeltaT_{NoSPOT}^" << j + 1 << ", ";
	}
	for (int j = 0; j < total_rooms; j++) {
		myfile << "PPV^" << j + 1 << ", ";
	}
	myfile << "T_{MixedAir}" << ", ";
	myfile << "T_{ext}" << ", ";
	myfile << "P_{AHU}" << "\n";

	for (long int i = 0; i < tinstances; i++) {
		for (int j = 0; j < total_rooms; j++) {
			myfile << SPOT_State(i, j) << ", ";
		}
		for (int j = 0; j < total_rooms; j++) {
			myfile << Occupancy(i, j) << ", ";
		}
		for (int j = 0; j < total_rooms; j++) {
			myfile << T(i, j) << ", ";
		}
		for (int j = 0; j < total_rooms; j++) {
			myfile << TR1(i, j) << ", ";
		}
		for (int j = 0; j < total_rooms; j++) {
			myfile << TR2(i, j) << ", ";
		}
		for (int j = 0; j < total_rooms; j++) {
			myfile << DeltaTR1(i, j) << ", ";
		}
		for (int j = 0; j < total_rooms; j++) {
			myfile << DeltaTR2(i, j) << ", ";
		}
		for (int j = 0; j < total_rooms; j++) {
			myfile << PPV(i, j) << ", ";
		}
		myfile << MixedAirTemperature(i, 0) << ", ";
		myfile << T_ext(i, 0) << ", ";
		myfile << PowerAHU(i, 0) << "\n";
	}
	myfile.close();
}

