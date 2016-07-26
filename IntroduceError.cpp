/*
 * IntroduceError.cpp
 *
 *  Created on: Jul 26, 2016
 *      Author: m26jain
 */

#include <cmath>
#include <stdlib.h>

#include "IntroduceError.h"

void ErrorInParams(PARAMS& ParamsIn, float error) {
	/* Thermal Capacities */
	ParamsIn.CommonRoom.C = ParamsIn.CommonRoom.C * (1 + error);					// Thermal Capacity of Room (kJ/K)
	ParamsIn.CommonRoom.C_ = ParamsIn.CommonRoom.C_ * (1 + error);					// Thermal Capacity of SPOT Region (kJ/K)

	/* Heat Transfer Coefficients */
	ParamsIn.CommonRoom.alpha_o = ParamsIn.CommonRoom.alpha_o * (1 + error);		// Heat Transfer Coefficient for Outside (kJ/K.s)
	ParamsIn.CommonRoom.alpha_o = ParamsIn.CommonRoom.alpha_o * (1 + error);		// Heat Transfer Coefficient for Regions (kJ/K.s)
}

MAT_FLOAT ErrorInWeather(MAT_FLOAT T_ext, float error) {
	if (error == 0) {
		return T_ext;
	}

	size_t nRows = ext_temperature.rows();
	size_t nCols = ext_temperature.cols();

	// Matrix of size (n x total_rooms) to assign external temperature equal to 5
	MAT_FLOAT ext_temperature = MAT_FLOAT::Ones(nRows, nCols);

	float max_err = error * 100.0;				// Max Error Possible
	float min_err = error * 100.0 * (-1.0);		// Minimum Error Possible

	// Assign error (randomaly distributed) to each element
	for (size_t i = 0; i < nRows; ++i) {
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
