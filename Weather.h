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
		Eigen::MatrixXf GetWeatherForecast(uint32 duration, uint16 time_step, uint8 num_zones, uint8 num_rooms);
	private:

	};
}

#endif /* WEATHER_H_ */
