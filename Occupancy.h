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
		Eigen::MatrixXi GetOccupancyForecast(uint32 duration, uint16 time_step, uint8 num_zones, uint8 num_rooms);
	private:

	};

}

#endif /* OCCUPANCY_H_ */
