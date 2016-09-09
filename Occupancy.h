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
		Occupants();		// Constructor
		~Occupants();		// Destructor
		/* Test Function */
		MAT_INT GetOccupancyForecast(long int duration, int time_step, int num_zones, int num_rooms);
		/* Parse Occupancy Data File */
		void ParseOccupancyData(DF_INT2& theData, const int& total_rooms, const std::string& filename, time_t &start_t, time_t &end_t,
						const int& time_step, const int& skip_lines);
		/* Convert Dataframe to Matrix for Computation */
		MAT_FLOAT GetOccupancyMatrix(DF_INT2& df, time_t &start_t, const long int& n, const int& time_step, const int& total_rooms);
	private:

	};

}

#endif /* OCCUPANCY_H_ */
