/*
 * ThermalModel.h
 *
 *  Created on: Jul 14, 2016
 *      Author: m26jain
 */

#ifndef THERMALMODEL_H_
#define THERMALMODEL_H_

#include "defs.h"

namespace SimpleBuildingSimulator {
	class ModelRachel {
	public:
		void SimulateModel(DF_OUTPUT df[], MAT_FLOAT T_ext_blk, MAT_FLOAT O_blk, const PARAMS& ParamsIn,
				const int& time_step, const int& total_rooms, int start_idx, int step_size, const int& control_type);

	private:
		// Coefficient Matrices
		MAT_FLOAT CoWI_CRT_Matrix;
		MAT_FLOAT CoWI_OAT_Matrix;
		MAT_FLOAT CoHI_CRT_Matrix;
		MAT_FLOAT CoHI_SAT_Matrix;
		MAT_FLOAT CoEI_OLEL_Matrix;
		MAT_FLOAT CoRC_CiRT_Matrix;
		MAT_FLOAT CoSI_SCS_Matrix;
		MAT_FLOAT CoOI_OHL_Matrix;
		MAT_FLOAT CoRC_CiR1T_Matrix;

		MAT_FLOAT WI_CRT;
		MAT_FLOAT WI_OAT;
		MAT_FLOAT HI_CRT;
		MAT_FLOAT HI_SAT;
		MAT_FLOAT EI_OLEL;
		MAT_FLOAT RC_CiRT;
		MAT_FLOAT SI_SCS;
		MAT_FLOAT OI_OHL;
		MAT_FLOAT RC_CiR1T;

		// Functions to compute coefficient matrices
		MAT_FLOAT Create_CoWI_CRT_Matrix(const int& time_step, const int& total_rooms, const PARAMS& ParamsIn);
		MAT_FLOAT Create_CoWI_OAT_Matrix(const int& time_step, const int& total_rooms, const PARAMS& ParamsIn);

		MAT_FLOAT Create_CoHI_CRT_Matrix(const int& time_step, const int& total_rooms, const PARAMS& ParamsIn);
		MAT_FLOAT Create_CoHI_SAT_Matrix(const int& time_step, const int& total_rooms, const PARAMS& ParamsIn);

		MAT_FLOAT Create_CoEI_OLEL_Matrix(const int& time_step, const int& total_rooms, const PARAMS& ParamsIn);
		MAT_FLOAT Create_CoOI_OHL_Matrix(const int& time_step, const int& total_rooms, const PARAMS& ParamsIn);

		MAT_FLOAT Create_CoSI_SCS_Matrix(const int& time_step, const int& total_rooms, const PARAMS& ParamsIn);

		MAT_FLOAT Create_CoRC_CiRT_Matrix(const int& time_step, const int& total_rooms, const PARAMS& ParamsIn);
		MAT_FLOAT Create_CoRC_CiR1T_Matrix(const int& time_step, const int& total_rooms, const PARAMS& ParamsIn);

		float GetMixedAirTemperature(MAT_FLOAT TR1, MAT_FLOAT T_ext);
		float GetAHUPower(float MixedAirTemperature, MAT_FLOAT SPOT_CurrentState, float SAT_Value,
				MAT_FLOAT SAV_Zones, const PARAMS& ParamsIn);

		void ComputeCoefficients(const int& time_step, const int& total_rooms, const PARAMS& ParamsIn);

	};
}



#endif /* THERMALMODEL_H_ */
