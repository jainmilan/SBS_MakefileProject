/*
 * IntroduceError.cpp
 *
 *  Created on: Jul 26, 2016
 *      Author: m26jain
 */

#include <cmath>
#include <stdlib.h>

#include "IntroduceError.h"

PARAMS ErrorInParams(PARAMS& ParamsIn, float error) {
	PARAMS ParamsErr;

	/* Thermal Capacities */
	ParamsErr.CommonRoom.C = ParamsIn.CommonRoom.C * (1 + error);					// Thermal Capacity of Room (kJ/K)
	ParamsErr.CommonRoom.C_ = ParamsIn.CommonRoom.C_ * (1 + error);					// Thermal Capacity of SPOT Region (kJ/K)

	/* Heat Transfer Coefficients */
	ParamsErr.CommonRoom.alpha_o = ParamsIn.CommonRoom.alpha_o * (1 + error);		// Heat Transfer Coefficient for Outside (kJ/K.s)
	ParamsErr.CommonRoom.alpha_r = ParamsIn.CommonRoom.alpha_r * (1 + error);		// Heat Transfer Coefficient for Regions (kJ/K.s)

	/* Heat Loads */
	ParamsErr.CommonRoom.Q_l = ParamsIn.CommonRoom.Q_l;								// Heat Load Due to Lightening and Equipments (kW)
	ParamsErr.CommonRoom.Q_h = ParamsIn.CommonRoom.Q_h;								// Heat Load Due to Presence of Occupants (kW)
	ParamsErr.CommonRoom.Q_s = ParamsIn.CommonRoom.Q_s;								// Heat Load of SPOT Unit (kW)

	/* Impact of Fan */
	ParamsErr.CommonRoom.fan_coef = ParamsIn.CommonRoom.fan_coef;					// Coefficient of Fan Power Consumption (-)

	/* Parameters for Air */
	ParamsErr.CommonAir.density = ParamsIn.CommonAir.density;						// Density of Air (kg/m3)
	ParamsErr.CommonAir.specific_heat = ParamsIn.CommonAir.specific_heat;			// Specific Heat Capacity of Air (J/Kg-K)

	/* PMV Parameters */
	ParamsErr.PMV_Params.P1 = ParamsIn.PMV_Params.P1;								// 1st Coefficient of Learned PMV Equation (-)
	ParamsErr.PMV_Params.P2 = ParamsIn.PMV_Params.P2;								// 2nd Coefficient of Learned PMV Equation (-)
	ParamsErr.PMV_Params.P3 = ParamsIn.PMV_Params.P3;								// 3rd Coefficient of Learned PMV Equation (-)
	ParamsErr.PMV_Params.P4 = ParamsIn.PMV_Params.P4;								// 4th Coefficient of Learned PMV Equation (-)

	/* AHU Efficiencies */
	ParamsErr.CommonAHU.HeatingEfficiency = ParamsIn.CommonAHU.HeatingEfficiency;	// Heating Efficiency of AHU (-)
	ParamsErr.CommonAHU.CoolingEfficiency = ParamsIn.CommonAHU.CoolingEfficiency;	// Cooling Efficiency of AHU (-)

	/* Error Values */
	ParamsErr.CommonErrors.err_bparams = ParamsIn.CommonErrors.err_bparams;			// Error in Building Parameters (-)
	ParamsErr.CommonErrors.err_text = ParamsIn.CommonErrors.err_text;				// Error in Weather Conditions (-)

	/* Files */
	ParamsErr.Files.weather_file = ParamsIn.Files.weather_file;						// Input File to Read Weather Data (-)
	ParamsErr.Files.occupancy_file = ParamsIn.Files.occupancy_file;					// Input File to Read Occupancy Data (-)
	ParamsErr.Files.merged_data_file = ParamsIn.Files.merged_data_file;				// Input File to Read Occupancy Data (-)
	ParamsErr.Files.output_file = ParamsIn.Files.output_file;						// Output File Created by the Program (-)

	/* Other Parameters */
	ParamsErr.CommonBuilding.num_zones_ = ParamsIn.CommonBuilding.num_zones_;		// Number of VAV Zones in the Building (-)
	ParamsErr.CommonBuilding.num_rooms_ = ParamsIn.CommonBuilding.num_rooms_;		// Number of Rooms in each VAV Zone (-)

	return ParamsErr;
}

MAT_FLOAT ErrorInWeather(MAT_FLOAT T_ext, float error) {
	if (error == 0) {
		return T_ext;
	}

	size_t nRows = T_ext.rows();
	size_t nCols = T_ext.cols();

	// Matrix of size (n x total_rooms) to assign external temperature equal to 5
	MAT_FLOAT ext_temperature = MAT_FLOAT::Ones(nRows, nCols);

	float max_err = error * 100.0;				// Max Error Possible
	float min_err = error * 100.0 * (-1.0);		// Minimum Error Possible

	ext_temperature.row(0) = T_ext.row(0);

	// Assign error (randomaly distributed) to each element
	for (size_t i = 1; i < nRows; ++i) {
		for (size_t j = 0; j < nCols; ++j) {
			// Random Error
			float rand_err = (float) (rand() % (int) (max_err - min_err + 1.0f)
					+ min_err) / 100.0;
			// Final Value of External Temperature
			ext_temperature(i, j) = T_ext(i, j) * (1 + rand_err);
		}
	}
	return ext_temperature;		// Return Matrix*/
}
