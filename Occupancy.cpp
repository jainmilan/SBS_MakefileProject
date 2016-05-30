/*
 * Occupancy.cpp
 *
 *  Created on: May 30, 2016
 *      Author: m26jain
 */

#include "Occupancy.h"

using namespace SimpleBuildingSimulator;

Occupants::Occupants()
{
}

Occupants::~Occupants()
{
}

Eigen::MatrixXi Occupants::GetOccupancyForecast(uint32 duration, uint16 time_step, uint8 num_zones, uint8 num_rooms) {
	int n = (duration / time_step) + 1;
	int total_rooms = num_zones * num_rooms;

	Eigen::MatrixXi occupancy = Eigen::MatrixXi::Ones(n, total_rooms);

	return occupancy;
}


