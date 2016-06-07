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
		Eigen::MatrixXf GetWeatherForecast(long int duration, int time_step, int num_zones, int num_rooms);
	private:

	};
}

#endif /* WEATHER_H_ */
