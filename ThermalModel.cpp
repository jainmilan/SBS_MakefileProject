/*
 * ThermalModel.cpp
 *
 *  Created on: Jul 14, 2016
 *      Author: m26jain
 */

#include "time.h"
#include <algorithm>

#include "ControlBox.h"
#include "ThermalModel.h"
#include "IntroduceError.h"

using namespace SimpleBuildingSimulator;

MAT_FLOAT ModelRachel::Create_CoWI_CRT_Matrix(const int& time_step, const int& total_rooms, const PARAMS& ParamsIn) {
	float CoWI_CRT = 1 - ((ParamsIn.CommonRoom.alpha_o * time_step) / ParamsIn.CommonRoom.C);

	MAT_FLOAT CoWI_CRT_Matrix = MAT_FLOAT::Identity(total_rooms, total_rooms) * CoWI_CRT;

	return CoWI_CRT_Matrix;
}

MAT_FLOAT ModelRachel::Create_CoWI_OAT_Matrix(const int& time_step, const int& total_rooms, const PARAMS& ParamsIn) {
	float CoWI_OAT = (ParamsIn.CommonRoom.alpha_o * time_step) / ParamsIn.CommonRoom.C;

	MAT_FLOAT CoWI_OAT_Matrix = MAT_FLOAT::Identity(total_rooms, total_rooms) * CoWI_OAT;

	return CoWI_OAT_Matrix;
}

MAT_FLOAT ModelRachel::Create_CoHI_CRT_Matrix(const int& time_step, const int& total_rooms, const PARAMS& ParamsIn) {
	float CoHI_CRT = (-1 * time_step * ParamsIn.CommonAir.density
			* ParamsIn.CommonAir.specific_heat) / (ParamsIn.CommonRoom.C);

	MAT_FLOAT CoHI_CRT_Matrix = MAT_FLOAT::Identity(total_rooms, total_rooms) * CoHI_CRT;

	return CoHI_CRT_Matrix;
}

MAT_FLOAT ModelRachel::Create_CoHI_SAT_Matrix(const int& time_step, const int& total_rooms, const PARAMS& ParamsIn) {
	float CoHI_SAT = (time_step * ParamsIn.CommonAir.density * ParamsIn.CommonAir.specific_heat)
			/ (ParamsIn.CommonRoom.C);

	MAT_FLOAT CoHI_SAT_Matrix = MAT_FLOAT::Identity(total_rooms, total_rooms) * CoHI_SAT;

	return CoHI_SAT_Matrix;
}

MAT_FLOAT ModelRachel::Create_CoEI_OLEL_Matrix(const int& time_step, const int& total_rooms, const PARAMS& ParamsIn) {
		float CoEI_OLEL = (time_step * ParamsIn.CommonRoom.Q_l) / (ParamsIn.CommonRoom.C);

	MAT_FLOAT CoEI_OLEL_Matrix = MAT_FLOAT::Identity(total_rooms, total_rooms) * CoEI_OLEL;

	return CoEI_OLEL_Matrix;
}

MAT_FLOAT ModelRachel::Create_CoOI_OHL_Matrix(const int& time_step, const int& total_rooms, const PARAMS& ParamsIn) {
	float CoOI_OHL = (time_step * ParamsIn.CommonRoom.Q_h) / (ParamsIn.CommonRoom.C_);

	MAT_FLOAT CoOI_OHL_Matrix = MAT_FLOAT::Identity(total_rooms, total_rooms) * CoOI_OHL;

	return CoOI_OHL_Matrix;
}

MAT_FLOAT ModelRachel::Create_CoSI_SCS_Matrix(const int& time_step, const int& total_rooms, const PARAMS& ParamsIn) {
	float CoSI_SCS = (time_step * ParamsIn.CommonRoom.Q_s) / (ParamsIn.CommonRoom.C_);

	MAT_FLOAT CoSI_SCS_Matrix = MAT_FLOAT::Identity(total_rooms, total_rooms) * CoSI_SCS;

	return CoSI_SCS_Matrix;
}

MAT_FLOAT ModelRachel::Create_CoRC_CiRT_Matrix(const int& time_step, const int& total_rooms, const PARAMS& ParamsIn) {
	float CoRC_CiRT = 1 - ((time_step * ParamsIn.CommonRoom.alpha_r) / (ParamsIn.CommonRoom.C_));

	MAT_FLOAT CoRC_CiRT_Matrix = MAT_FLOAT::Identity(total_rooms, total_rooms) * CoRC_CiRT;

	return CoRC_CiRT_Matrix;
}

MAT_FLOAT ModelRachel::Create_CoRC_CiR1T_Matrix(const int& time_step, const int& total_rooms, const PARAMS& ParamsIn) {
	float CoRC_CiR1T = (time_step * ParamsIn.CommonRoom.alpha_r)
			/ (ParamsIn.CommonRoom.C - ParamsIn.CommonRoom.C_);

	MAT_FLOAT CoRC_CiR1T_Matrix = MAT_FLOAT::Identity(total_rooms, total_rooms) * CoRC_CiR1T;

	return CoRC_CiR1T_Matrix;
}

float ModelRachel::GetMixedAirTemperature(MAT_FLOAT TR2, MAT_FLOAT T_ext, float r) {
	float MixedAirTemperature = 0.0f;

	float TR2Mean = TR2.mean();
	float TextMean = T_ext.mean();

	//std::cout << "TR2Mean: " << TR2Mean << "\nTextMean: " << TextMean << "\n";
	MixedAirTemperature = (r * TR2Mean) + ((1 - r) * TextMean);

	return MixedAirTemperature;
}

float ModelRachel::GetAHUPower(float MixedAirTemperature, MAT_FLOAT SPOT_CurrentState,
		float SAT_Value, MAT_FLOAT SAV_Matrix, const PARAMS& ParamsIn) {
	float AHUPower = 0.0f;

	// Properties of Air
	float density = ParamsIn.CommonAir.density;				// Density of Air(kg / m3)
	float specific_heat = ParamsIn.CommonAir.specific_heat;// Specific heat of Air(kJ / kg.K)
	float Q_s = ParamsIn.CommonRoom.Q_s;				       // Heat Load of SPOT Unit(kW)

	float HeatingEfficiency = ParamsIn.CommonAHU.HeatingEfficiency;
	float CoolingEfficiency = ParamsIn.CommonAHU.CoolingEfficiency;

	float SupplyAirTemperature = SAT_Value;
	float T_c = MixedAirTemperature;

	float CoefficientHeatingPower = (density * specific_heat)
			/ HeatingEfficiency;
	float CoefficientCoolingPower = (density * specific_heat)
			/ CoolingEfficiency;
	float CoefficientFanPower = ParamsIn.CommonRoom.fan_coef;          // kW.s.s / Kg.Kg

	float HeatingPower = SAV_Matrix.sum()
			* (CoefficientHeatingPower * (SupplyAirTemperature - T_c));
	float CoolingPower = SAV_Matrix.sum()
			* (CoefficientCoolingPower * (MixedAirTemperature - T_c));
	float FanPower = CoefficientFanPower * SAV_Matrix.sum() * SAV_Matrix.sum();
	float SPOTPower = Q_s * SPOT_CurrentState.sum();

	//std::cout << "SAV_Zones: " << SAV_Zones << "\nCoefficientHeatingPower: " << CoefficientHeatingPower << "\n";
	//std::cout << "SupplyAirTemperature: " << SupplyAirTemperature << "\nT_c: " << T_c << "\nMixedAirTemperature: " << MixedAirTemperature << "\n";
	//std::cout << "Heating Power: " << HeatingPower << "\nCooling Power: " << CoolingPower << "\nFanPower: " << FanPower << "\nSPOT Power: " << SPOTPower << "\n";
	AHUPower = HeatingPower + CoolingPower + FanPower + SPOTPower;

	return AHUPower;
}

void ModelRachel::ComputeCoefficients(const int& time_step, const int& total_rooms, const PARAMS& ParamsIn) {
	/* Building Parameters For HVAC Impact*/

	// Impact of Weather
	CoWI_CRT_Matrix = Create_CoWI_CRT_Matrix(time_step, total_rooms, ParamsIn);
	WI_CRT = MAT_FLOAT(1, total_rooms);

	CoWI_OAT_Matrix = Create_CoWI_OAT_Matrix(time_step, total_rooms, ParamsIn);
	WI_OAT = MAT_FLOAT(1, total_rooms);

	// Impact of HVAC
	CoHI_CRT_Matrix = Create_CoHI_CRT_Matrix(time_step, total_rooms, ParamsIn);
	HI_CRT = MAT_FLOAT(1, total_rooms);

	CoHI_SAT_Matrix = Create_CoHI_SAT_Matrix(time_step, total_rooms, ParamsIn);
	HI_SAT = MAT_FLOAT(1, total_rooms);

	// Impact of Equipments
	CoEI_OLEL_Matrix = Create_CoEI_OLEL_Matrix(time_step, total_rooms, ParamsIn);
	EI_OLEL = MAT_FLOAT(1, total_rooms);

	/* Building Parameters For Temperature in SPOT Region */

	// Impact of Room Coupling
	CoRC_CiRT_Matrix = Create_CoRC_CiRT_Matrix(time_step, total_rooms, ParamsIn);
	RC_CiRT = MAT_FLOAT(1, total_rooms);

	// Impact of SPOT
	CoSI_SCS_Matrix = Create_CoSI_SCS_Matrix(time_step, total_rooms, ParamsIn);
	SI_SCS = MAT_FLOAT(1, total_rooms);

	// Impact of Occupants
	CoOI_OHL_Matrix = Create_CoOI_OHL_Matrix(time_step, total_rooms, ParamsIn);
	OI_OHL = MAT_FLOAT(1, total_rooms);

	/* Building Parameters For Temperature in Non-SPOT Region */

	// Impact of Room Coupling
	CoRC_CiR1T_Matrix = Create_CoRC_CiR1T_Matrix(time_step, total_rooms, ParamsIn);
	RC_CiR1T = MAT_FLOAT(1, total_rooms);
}

void ModelRachel::SimulateModel(DF_OUTPUT df[], MAT_FLOAT T_ext_mpc, MAT_FLOAT T_ext_spot, MAT_FLOAT O_mpc, MAT_FLOAT O_spot, PARAMS& ParamsIn,
		const int& time_step_mpc, const int& time_step_spot, const int& total_rooms, int time_instances_mpc, int time_instances_spot,
		const int& control_type, const int& horizon) {

	int k = 0;
	float response = 0;

	int n_mpc = time_instances_mpc;
	int n_spot = time_instances_spot;

	PARAMS ParamsErr;
	ParamsErr = ErrorInParams(ParamsIn, ParamsIn.CommonErrors.err_bparams);

	ComputeCoefficients(time_step_mpc, total_rooms, ParamsIn);

	/* Output of the Program */

	// Temperature Matrices
	Eigen::MatrixXf T = Eigen::MatrixXf::Zero(n_spot, total_rooms);
	Eigen::MatrixXf TR1 = Eigen::MatrixXf::Zero(n_spot, total_rooms);
	Eigen::MatrixXf TR2 = Eigen::MatrixXf::Zero(n_spot, total_rooms);

	// Delta Matrices
	Eigen::MatrixXf DeltaTR1 = Eigen::MatrixXf::Zero(n_spot, total_rooms);
	Eigen::MatrixXf DeltaTR2 = Eigen::MatrixXf::Zero(n_spot, total_rooms);

	// PPV
	Eigen::MatrixXf PPV = Eigen::MatrixXf::Zero(n_spot, total_rooms);

	// AHU Parameters
	Eigen::MatrixXf r = Eigen::MatrixXf::Zero(n_spot, 1);
	Eigen::MatrixXf MixedAirTemperature = Eigen::MatrixXf::Zero(n_spot, 1);
	Eigen::MatrixXf PowerAHU = Eigen::MatrixXf::Zero(n_spot, 1);

	// Initialization
	size_t step_size_mpc = (horizon * 60 * 60) / time_step_mpc;
	size_t step_size_spot = (horizon * 60 * 60) / time_step_spot;

	std::cout << "Step Size MPC: " << step_size_mpc << ", Step Size SPOT: " << step_size_spot << "\n";

	// std::cout << duration << "\n" << horizon << "\n" << n << "\n" << step_size << "\n";

	MAT_FLOAT T_ext_blk = MAT_FLOAT::Ones(step_size_mpc, 1);
	MAT_FLOAT T_ext_eblk = MAT_FLOAT::Ones(step_size_mpc, 1);

	MAT_FLOAT O_blk = MAT_FLOAT::Ones(step_size_mpc, 1);

	time_t start_time = df[k].t;
	struct tm *date = gmtime(&start_time);
	int Time_IH = (date->tm_min)/10;

	ControlBox cb;
	ControlVariables CV = cb.DefaultControl(total_rooms, ParamsIn);

	MAT_FLOAT SPOT_State = MAT_FLOAT::Zero(n_spot, total_rooms);
	SPOT_State.row(k) = CV.SPOT_CurrentState;

	T.row(k) = Eigen::VectorXf::Ones(total_rooms) * 21;
	TR1.row(k) = T.row(k) + DeltaTR1.row(k);
	TR2.row(k) = T.row(k) + DeltaTR2.row(k);

	PPV.row(k) = O_spot.row(k).array() * ((ParamsIn.PMV_Params.P1 * TR1.row(k))
					- (ParamsIn.PMV_Params.P2 * MAT_FLOAT::Zero(1, total_rooms))
					+ (ParamsIn.PMV_Params.P3 * MAT_FLOAT::Zero(1, total_rooms))
					- (ParamsIn.PMV_Params.P4 * MAT_FLOAT::Ones(1, total_rooms))).array();

	/* Update Output Frame */
	df[k].response = response;
	for (size_t room = 0; room < (size_t) total_rooms; room++) {
		df[k].ppv[room] = PPV(k, room);
		df[k].tspot[room] = TR1(k, room);
		df[k].tnospot[room] = TR2(k, room);
	}
	std::cout << "Room: \n";

	int time_step_ratio = time_step_mpc / time_step_spot;
	for(size_t k = 1; k <= (size_t) (n_mpc - step_size_mpc); k = k + 1) {
		int mpc_indx_prev = (k-1) * time_step_ratio;	// Converting MPC previous index to SPOT index
		int mpc_indx_curr = k * time_step_ratio;		// Converting MPC current index to SPOT index

		start_time = df[mpc_indx_prev].t;	// Previous MPC Index

		std::cout << "Start Time: " << df[mpc_indx_prev].t << "\n";	// Previous MPC Index
		struct tm *date = gmtime(&start_time);
		Time_IH = (date -> tm_min)/10;

		T_ext_blk = T_ext_mpc.block(k-1, 0, step_size_mpc, 1);		// Previous MPC Index
		O_blk = O_mpc.block(k-1, 0, step_size_mpc, total_rooms);	// Every Time Step of MPC

		switch (control_type) {
		case 1:
			CV = cb.DefaultControl(total_rooms, ParamsIn);
			break;
		case 2:
			CV = cb.ReactiveControl(total_rooms, TR1.row(k-1), O_mpc.row(k-1), k-1, SPOT_State.row(k-1), ParamsIn);
			break;
		case 3:
			T_ext_eblk = ErrorInWeather(T_ext_blk, ParamsIn.CommonErrors.err_text);
			std::cout << "C: " << ParamsIn.CommonRoom.C << " >> " << ParamsErr.CommonRoom.C << std::endl;
			std::cout << "C_: " << ParamsIn.CommonRoom.C_ << " >> " << ParamsErr.CommonRoom.C_ << std::endl;
			std::cout << "alpha_o: " << ParamsIn.CommonRoom.alpha_o << " >> " << ParamsErr.CommonRoom.alpha_o << std::endl;
			std::cout << "alpha_r: " << ParamsIn.CommonRoom.alpha_r << " >> " << ParamsErr.CommonRoom.alpha_r << std::endl;
			//std::cout << T_ext_blk << std::endl;
			//std::cout << T_ext_eblk << std::endl;

			response = cb.MPCControl(step_size_mpc, time_step_mpc, T_ext_eblk, O_blk, TR2.row(mpc_indx_curr - 1),
					DeltaTR1.row(mpc_indx_curr - 1), ParamsErr, horizon, Time_IH, CV, k-1); // Every time step of SPOT except last argument
			break;
		default:
			break;
		}

		/* Temperature Change in the Room Due to HVAC */
		if (response == 299) {
			SPOT_State.row(mpc_indx_curr) = SPOT_State.row(mpc_indx_curr - 1);	// Every time step of SPOT
			r.row(mpc_indx_curr) << r.row(mpc_indx_curr-1);						// Every time step of SPOT

			T.row(mpc_indx_curr) = T.row(mpc_indx_curr-1);						// Every time step of SPOT

			TR1.row(mpc_indx_curr) = TR1.row(mpc_indx_curr-1);					// Every time step of SPOT
			TR2.row(mpc_indx_curr) = TR2.row(mpc_indx_curr-1);					// Every time step of SPOT

			DeltaTR1.row(mpc_indx_curr) = DeltaTR1.row(mpc_indx_curr-1);		// Every time step of SPOT
			DeltaTR2.row(mpc_indx_curr) = DeltaTR2.row(mpc_indx_curr-1);		// Every time step of SPOT

			PPV.row(mpc_indx_curr) = PPV.row(mpc_indx_curr-1);					// Every time step of SPOT
			MixedAirTemperature.row(mpc_indx_curr) = MixedAirTemperature.row(mpc_indx_curr-1); // Every time step of SPOT
			PowerAHU.row(mpc_indx_curr) = PowerAHU.row(mpc_indx_curr-1);		// Every time step of SPOT
		}
		else {

			SPOT_State.row(mpc_indx_curr-1) = CV.SPOT_CurrentState;
			r.row(mpc_indx_curr-1) << CV.r;

			// Impact of Weather
			WI_CRT = TR2.row(mpc_indx_curr-1) * CoWI_CRT_Matrix;
			WI_OAT = T_ext_mpc.row(k-1) * CoWI_OAT_Matrix;

			// Impact of HVAC
			HI_CRT = TR2.row(mpc_indx_curr-1) * CV.SAV_Matrix * CoHI_CRT_Matrix;
			HI_SAT = CV.SAT * CV.SAV_Matrix * CoHI_SAT_Matrix;

			// Impact of Equipments
			EI_OLEL = O_mpc.row(k-1) * CoEI_OLEL_Matrix;

			T.row(mpc_indx_curr) = WI_CRT + WI_OAT + HI_CRT + HI_SAT + EI_OLEL;

			/* Temperature Change in SPOT Region*/

			// Impact of Region Coupling
			RC_CiRT = DeltaTR1.row(mpc_indx_curr-1) * CoRC_CiRT_Matrix;

			// Impact of SPOT
			SI_SCS = SPOT_State.row(mpc_indx_curr-1) * CoSI_SCS_Matrix; // CV.SPOT_CurrentState * CoSI_SCS_Matrix;

			// Impact of Occupants
			OI_OHL = O_mpc.row(k-1) * CoOI_OHL_Matrix;

			DeltaTR1.row(mpc_indx_curr) = RC_CiRT + SI_SCS + OI_OHL;
			TR1.row(mpc_indx_curr) = T.row(mpc_indx_curr) + DeltaTR1.row(mpc_indx_curr);

			/* Temperature Change in Non-SPOT Region*/

			// Impact of Region Coupling
			RC_CiR1T = DeltaTR1.row(mpc_indx_curr-1) * CoRC_CiR1T_Matrix;

			DeltaTR2.row(mpc_indx_curr) = RC_CiR1T;
			TR2.row(mpc_indx_curr) = T.row(mpc_indx_curr) + DeltaTR2.row(mpc_indx_curr);

			PPV.row(mpc_indx_curr) = O_mpc.row(k-1).array() * ((ParamsIn.PMV_Params.P1 * TR1.row(mpc_indx_curr))
				- (ParamsIn.PMV_Params.P2 * MAT_FLOAT::Zero(1, total_rooms))
				+ (ParamsIn.PMV_Params.P3 * MAT_FLOAT::Zero(1, total_rooms))
				- (ParamsIn.PMV_Params.P4 * MAT_FLOAT::Ones(1, total_rooms))).array();

			MixedAirTemperature.row(mpc_indx_prev) << GetMixedAirTemperature(TR2.row(mpc_indx_curr-1), T_ext_blk.row(0), r.row(mpc_indx_prev).value());
			PowerAHU.row(mpc_indx_prev) << GetAHUPower(MixedAirTemperature.row(mpc_indx_prev).value(),
						CV.SPOT_CurrentState, CV.SAT_Value, CV.SAV_Matrix, ParamsIn);
		}

		std::cout << T_ext_spot(mpc_indx_prev) << " => " << T_ext_blk(0) << std::endl;

		/* Update Output Frame */
		df[mpc_indx_curr-1].weather_err = T_ext_blk(0);		// External Temperature
		df[mpc_indx_curr-1].power = PowerAHU(mpc_indx_prev);
		df[mpc_indx_curr-1].r = r(mpc_indx_prev);
		df[mpc_indx_curr-1].tmix = MixedAirTemperature(mpc_indx_prev);
		df[mpc_indx_curr].response = response;

		for (size_t room = 0; room < (size_t) total_rooms; room++) {
			df[mpc_indx_curr].ppv[room] = PPV(mpc_indx_curr, room);
			df[mpc_indx_curr].tspot[room] = TR1(mpc_indx_curr, room);
			df[mpc_indx_curr].tnospot[room] = TR2(mpc_indx_curr, room);
			df[mpc_indx_curr-1].spot_status[room] = SPOT_State(mpc_indx_curr-1, room);
		}

		//		std::cout << "TR2: " << TR2.row(k) << "\n";
		//		std::cout << "Delta_TR1: " << DeltaTR1.row(k) << "\n";
		// std::cout << "SAT Prev: " << CV.SAT_Value << "\n";
		//		std::cout << "SPOT Current State: " << CV.SPOT_CurrentState << "\n";

		//		std::cout << "T Ext: " << T_ext_blk.row(1) << "\n";
		//		std::cout << "Occupancy: " << O.row(k) << "\n";

		//std::cout << "Mixed Air Temperature: " << MixedAirTemperature.row(k) << "\n";
		//std::cout << "R: " << r.row(k) << "\n";
		//std::cout << "TR2: " << TR2.row(k) << "\n";
		//std::cout << "T_ext: " << T_ext_blk.row(k) << "\n";

		std::cout << "Power AHU: " << PowerAHU.row(mpc_indx_curr-1) << "\n";
	}

}
