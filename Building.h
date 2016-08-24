/*
 * Building.h
 *
 *  Created on: May 30, 2016
 *      Author: m26jain
 */

#ifndef BUILDING_H_
#define BUILDING_H_

#include "defs.h"
#include<string>

namespace SimpleBuildingSimulator
{
	class Building
	{
	public:
		int skip_lines;
		PARAMS ParamsIn;

		Building();
		~Building();

		// Simulate within Time Limits
		void Simulate(time_t &start_t, time_t &stop_t, const int& time_step_mpc, const int& time_step_spot, const int& control_type, const int& horizon);
	};
} // namespace SimpleBuildingSimulator

#endif /* BUILDING_H_ */
