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
		Weather();			// Constructor
		~Weather();			// Destructor
		/* Test Function */
		Eigen::MatrixXf GetWeatherForecast(long int duration, int time_step, int num_zones, int num_rooms, float err);
		/* Parse Weather Data File */
		void ParseWeatherData(DF_FLOAT& theData, const std::string& filename, time_t &start_t, time_t &end_t,
				const int& time_step, const int& skip_lines);
		/* Convert Dataframe to Matrix for Computation */
		MAT_FLOAT GetWeatherMatrix(DF_OUTPUT df[], const long int& n, const int& total_rooms);
	};
}

#endif /* WEATHER_H_ */
