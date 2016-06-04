/*
 * ControlBox.h
 *
 *  Created on: May 30, 2016
 *      Author: m26jain
 */

#ifndef CONTROLBOX_H_
#define CONTROLBOX_H_

#include<iostream>

#include "defs.h"
#include "building.h"

#include<Eigen/Dense>

namespace SimpleBuildingSimulator {
	struct ControlVariables
	{
		float SAT_Value;
		Eigen::MatrixXf SAT;
		Eigen::MatrixXf SAV_Zones;
		Eigen::MatrixXf SAV_Matrix;
		Eigen::MatrixXi SPOT_CurrentState;
	};
	class ControlBox
	{
	public:
		ControlBox();
		~ControlBox();
		struct ControlVariables DefaultControl(uint8 num_zones, uint8 num_rooms);
		struct ControlVariables ReactiveControl(uint8 num_zones, uint8 num_rooms, Eigen::MatrixXf TR1,
			Eigen::MatrixXf O, int k, Eigen::MatrixXi SPOT_PreviousState);
		struct ControlVariables MPCControl(int num_zones, int num_rooms, int duration, int time_step,
					Building::Air air_params, Building::Room room_params, Building::AHU ahu_params, Building::PMV_Model pmv_params);
	private:
		Eigen::MatrixXf GetSAVMatrix(Eigen::MatrixXf SAV_Zones, int num_rooms, int total_rooms);
	};

}

#endif /* CONTROLBOX_H_ */
