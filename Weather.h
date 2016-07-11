/*
 * Weather.h
 *
 *  Created on: May 30, 2016
 *      Author: m26jain
 */

#ifndef WEATHER_H_
#define WEATHER_H_

#include "defs.h"
#include<Eigen/Dense>

namespace SimpleBuildingSimulator {
	class Weather
	{
	public:
		Weather();
		~Weather();
		Eigen::MatrixXf GetWeatherForecast(long int duration, int time_step, int num_zones, int num_rooms, float err);
		void ParseWeatherData(DF_FLOAT& theData, std::string filename, time_t &start_t, time_t &end_t,
				int time_step, int skip_lines);
		Eigen::MatrixXf GetWeatherMatrix(DF_OUTPUT df[], long int n, int total_rooms);
	private:

	};
}

#endif /* WEATHER_H_ */
