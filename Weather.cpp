/*
 * Weather.cpp
 *
 *  Created on: May 30, 2016
 *      Author: m26jain
 */

#include "Weather.h"

using namespace SimpleBuildingSimulator;

Weather::Weather()
{
}

Weather::~Weather()
{
}

Eigen::MatrixXf Weather::GetWeatherForecast(long int duration, int time_step, int num_zones, int num_rooms) {
	long int n = (duration / time_step) + 1;
	int total_rooms = num_zones * num_rooms;

	Eigen::MatrixXf ext_temperature = Eigen::MatrixXf::Ones(n, total_rooms) * 5;

	return ext_temperature;
}



