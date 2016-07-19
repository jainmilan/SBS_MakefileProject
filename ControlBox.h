/*
 * ControlBox.h
 *
 *  Created on: May 30, 2016
 *      Author: m26jain
 */

#ifndef CONTROLBOX_H_
#define CONTROLBOX_H_

#include "defs.h"
#include "Building.h"

#include<iostream>
#include<Eigen/Dense>

namespace SimpleBuildingSimulator {

struct ControlVariables {
	float r;
	float SAT_Value;
	MAT_FLOAT SAT;
	MAT_FLOAT SAV_Zones;
	MAT_FLOAT SAV_Matrix;
	MAT_FLOAT SPOT_CurrentState;
};
class ControlBox {
public:
	ControlBox();
	~ControlBox();
	struct ControlVariables DefaultControl(const int& total_rooms, const PARAMS& ParamsIn);
	struct ControlVariables ReactiveControl(const int& total_rooms, MAT_FLOAT TR1, MAT_FLOAT O,
			int k, MAT_FLOAT SPOT_PreviousState, const PARAMS& ParamsIn);
	float MPCControl(DF_OUTPUT df[], const long int& tinstances, const int& time_step, MAT_FLOAT T_Outside,
			MAT_FLOAT Occupancy, MAT_FLOAT TNoSPOTInit,	MAT_FLOAT DeltaTSPOTInit, const PARAMS& ParamsIn, const int& horizon,
			const int& Time_IH,	ControlVariables& CV);
private:
	MAT_FLOAT GetSAVMatrix(MAT_FLOAT SAV_Zones, int num_rooms, int total_rooms);
};

}

#endif /* CONTROLBOX_H_ */
