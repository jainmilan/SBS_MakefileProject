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
		void ParseOccupancyData(DF_INT& theData, std::string filename, time_t &start_t, time_t &end_t,
						int time_step, int skip_lines);
		Eigen::MatrixXf GetOccupancyMatrix(DF_OUTPUT df[], long int n, int total_rooms);
	private:

	};

}

#endif /* OCCUPANCY_H_ */
