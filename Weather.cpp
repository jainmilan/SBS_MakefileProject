/*
 * Weather.cpp
 *
 *  Created on: May 30, 2016
 *      Author: m26jain
 */

#include "Weather.h"
#include <stdlib.h>
#include<iostream>
using namespace SimpleBuildingSimulator;

Weather::Weather()
{
}

Weather::~Weather()
{
}

Eigen::MatrixXf Weather::GetWeatherForecast(long int duration, int time_step, int num_zones, int num_rooms, float err) {
	long int n = (duration / time_step) + 1;
	int total_rooms = num_zones * num_rooms;

	Eigen::MatrixXf ext_temperature = Eigen::MatrixXf::Ones(n, total_rooms) * 5;

	std::cout << err;
	for (size_t i = 0, nRows = ext_temperature.rows(), nCols = ext_temperature.cols(); i < nRows; ++i) {
		for (size_t j = 0; j < nCols; ++j) {
			float max_err = err * 100.0;
			float min_err = err * 100.0 * (-1.0);
			float rand_err = (float) (rand() % (int)(max_err - min_err + 1.0f) + min_err) / 100.0;
			ext_temperature(i, j) =	ext_temperature(i, j) + (ext_temperature(i, j) * rand_err);
		}
	}
	return ext_temperature;
}



