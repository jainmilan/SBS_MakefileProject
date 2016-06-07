/*
 * Occupancy.h
 *
 *  Created on: May 30, 2016
 *      Author: m26jain
 */

#ifndef OCCUPANCY_H_
#define OCCUPANCY_H_

#include"defs.h"
#include<Eigen/Dense>

namespace SimpleBuildingSimulator {
	class Occupants
	{
	public:
		Occupants();
		~Occupants();
		Eigen::MatrixXi GetOccupancyForecast(long int duration, int time_step, int num_zones, int num_rooms);
	private:

	};

}

#endif /* OCCUPANCY_H_ */
