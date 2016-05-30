/*
 * ControlBox.cpp
 *
 *  Created on: May 30, 2016
 *      Author: m26jain
 */

#include "ControlBox.h"

using namespace SimpleBuildingSimulator;

ControlBox::ControlBox()
{
}

ControlBox::~ControlBox()
{
}

Eigen::MatrixXf ControlBox::GetSAVMatrix(Eigen::MatrixXf SAV_Zones, int num_rooms, int total_rooms) {
	Eigen::MatrixXf SAV_Zones_Rep = SAV_Zones.replicate(1, num_rooms);

	Eigen::VectorXf SAV_Matrix_Diagonal(Eigen::Map<Eigen::VectorXf>(SAV_Zones_Rep.data(), SAV_Zones_Rep.cols() * SAV_Zones_Rep.rows()));

	Eigen::MatrixXf SAVMatrix = Eigen::MatrixXf::Zero(total_rooms, total_rooms);
	SAVMatrix = SAV_Matrix_Diagonal.asDiagonal();

	return SAVMatrix;
}


struct ControlVariables ControlBox::DefaultControl(uint8 num_zones, uint8 num_rooms) {
	int total_rooms = num_zones * num_rooms;

	struct ControlVariables cv;

	cv.SAT_Value = 30;

	cv.SAT = Eigen::MatrixXf::Ones(1, total_rooms) * cv.SAT_Value;

	cv.SAV_Zones = Eigen::MatrixXf::Ones(num_zones, 1) * 0.05f;

	cv.SAV_Matrix = GetSAVMatrix(cv.SAV_Zones, num_rooms, total_rooms);

	cv.SPOT_CurrentState = Eigen::MatrixXi::Ones(1, total_rooms);

	return cv;
}

struct ControlVariables ControlBox::ReactiveControl(uint8 num_zones, uint8 num_rooms, Eigen::MatrixXf TR1,
	Eigen::MatrixXf O, int k, Eigen::MatrixXi SPOT_PreviousState) {

	int total_rooms = num_zones * num_rooms;

	struct ControlVariables cv;

	cv.SAT_Value = 30;

	cv.SAT = Eigen::MatrixXf::Ones(1, total_rooms) * cv.SAT_Value;

	cv.SAV_Zones = Eigen::MatrixXf::Ones(num_zones, 1) * 0.05f;

	cv.SAV_Matrix = GetSAVMatrix(cv.SAV_Zones, num_rooms, total_rooms);

	cv.SPOT_CurrentState = SPOT_PreviousState;

	std::cout << cv.SPOT_CurrentState(0, 0) << " " << O.col(0).value() << " " << TR1.col(0).value() << std::endl;

	if (k == 0) {
		return cv;
	}

	for (size_t i = 0, nCols = cv.SPOT_CurrentState.cols(); i < nCols; i++) {
		// If room is occupied
		if (O.col(i).value() == 1) {
			if ((TR1.col(i).value() > 23) && (SPOT_PreviousState.col(i).value() == 1)) {
				cv.SPOT_CurrentState(0, i) = 0;
			}
			else if ((TR1.col(i).value() < 21) && (SPOT_PreviousState.col(i).value() == 0)) {
				cv.SPOT_CurrentState(0, i) = 1;
			}
		}
		// If room is unoccupied
		else if (O.col(i).value() == 0) {
			if ((TR1.col(i).value() > 28) && (SPOT_PreviousState.col(i).value() == 1)) {
				cv.SPOT_CurrentState(0, i) = 0;
			}
			else if ((TR1.col(i).value() < 18) && (SPOT_PreviousState.col(i).value() == 0)) {
				cv.SPOT_CurrentState(0, i) = 1;
			}
		}
	}

	return cv;
}


