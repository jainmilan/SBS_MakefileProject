/*
 * Building.cpp
 *
 *  Created on: May 30, 2016
 *      Author: m26jain
 */

#include "Building.h"

using namespace SimpleBuildingSimulator;

Building::Room CommonRoom;
Building::Air CommonAir;

Building::Building()
{
}

Building::~Building()
{
}

Eigen::MatrixXf Building::Create_CoWI_CRT_Matrix(uint16 time_step) {
	int total_rooms = num_zones_ * num_rooms_;

	float CoWI_CRT = 1 - ((CommonRoom.alpha_o*time_step) / CommonRoom.C);

	Eigen::MatrixXf CoWI_CRT_Matrix = Eigen::MatrixXf::Identity(total_rooms, total_rooms) * CoWI_CRT;

	return CoWI_CRT_Matrix;
}

Eigen::MatrixXf Building::Create_CoWI_OAT_Matrix(uint16 time_step) {
	int total_rooms = num_zones_ * num_rooms_;

	float CoWI_OAT = (CommonRoom.alpha_o*time_step) / CommonRoom.C;

	Eigen::MatrixXf CoWI_OAT_Matrix = Eigen::MatrixXf::Identity(total_rooms, total_rooms) * CoWI_OAT;

	return CoWI_OAT_Matrix;
}

Eigen::MatrixXf Building::Create_CoHI_CRT_Matrix(uint16 time_step) {
	int total_rooms = num_zones_ * num_rooms_;

	float CoHI_CRT = (-1*time_step*CommonAir.density*CommonAir.specific_heat) / (num_rooms_*CommonRoom.C);

	Eigen::MatrixXf CoHI_CRT_Matrix = Eigen::MatrixXf::Identity(total_rooms, total_rooms) * CoHI_CRT;

	return CoHI_CRT_Matrix;
}

Eigen::MatrixXf Building::Create_CoHI_SAT_Matrix(uint16 time_step) {
	int total_rooms = num_zones_ * num_rooms_;

	float CoHI_SAT = (time_step*CommonAir.density*CommonAir.specific_heat) / (num_rooms_*CommonRoom.C);

	Eigen::MatrixXf CoHI_SAT_Matrix = Eigen::MatrixXf::Identity(total_rooms, total_rooms) * CoHI_SAT;

	return CoHI_SAT_Matrix;
}

Eigen::MatrixXf Building::Create_CoEI_OLEL_Matrix(uint16 time_step) {
	int total_rooms = num_zones_ * num_rooms_;

	float CoEI_OLEL = (time_step*CommonRoom.Q_l) / (CommonRoom.C);

	Eigen::MatrixXf CoEI_OLEL_Matrix = Eigen::MatrixXf::Identity(total_rooms, total_rooms) * CoEI_OLEL;

	return CoEI_OLEL_Matrix;
}

Eigen::MatrixXf Building::Create_CoOI_OHL_Matrix(uint16 time_step) {
	int total_rooms = num_zones_ * num_rooms_;

	float CoOI_OHL = (time_step*CommonRoom.Q_h) / (CommonRoom.C_);

	Eigen::MatrixXf CoOI_OHL_Matrix = Eigen::MatrixXf::Identity(total_rooms, total_rooms) * CoOI_OHL;

	return CoOI_OHL_Matrix;
}

Eigen::MatrixXf Building::Create_CoSI_SCS_Matrix(uint16 time_step) {
	int total_rooms = num_zones_ * num_rooms_;

	float CoSI_SCS = (time_step*CommonRoom.Q_s) / (CommonRoom.C_);

	Eigen::MatrixXf CoSI_SCS_Matrix = Eigen::MatrixXf::Identity(total_rooms, total_rooms) * CoSI_SCS;

	return CoSI_SCS_Matrix;
}

Eigen::MatrixXf Building::Create_CoRC_CiRT_Matrix(uint16 time_step) {
	int total_rooms = num_zones_ * num_rooms_;

	float CoRC_CiRT = 1 - ((time_step*CommonRoom.alpha_r) / (CommonRoom.C_));

	Eigen::MatrixXf CoRC_CiRT_Matrix = Eigen::MatrixXf::Identity(total_rooms, total_rooms) * CoRC_CiRT;

	return CoRC_CiRT_Matrix;
}

Eigen::MatrixXf Building::Create_CoRC_CiR1T_Matrix(uint16 time_step) {
	int total_rooms = num_zones_ * num_rooms_;

	float CoRC_CiR1T = (time_step*CommonRoom.alpha_r) / (CommonRoom.C - CommonRoom.C_);

	Eigen::MatrixXf CoRC_CiR1T_Matrix = Eigen::MatrixXf::Identity(total_rooms, total_rooms) * CoRC_CiR1T;

	return CoRC_CiR1T_Matrix;
}

float Building::GetMixedAirTemperature(Eigen::MatrixXf TR1, Eigen::MatrixXf T_ext) {
	float MixedAirTemperature = 0.0f;

	float TR1Mean = TR1.row(0).rowwise().mean().value();
	float TextMean = T_ext.row(0).rowwise().mean().value();
	float r = 0.8f;

	MixedAirTemperature = (r * TR1Mean) + ((1-r) * TextMean);

	return MixedAirTemperature;
}

float Building::GetAHUPower(float MixedAirTemperature, Eigen::MatrixXf O, float SAT_Value, Eigen::MatrixXf SAV_Zones) {
	float AHUPower = 0.0f;

	// Properties of Air
	float density = CommonAir.density;					// Density of Air(kg / m3)
	float specific_heat = CommonAir.specific_heat;		// Specific heat of Air(kJ / kg.K)
	float Q_s = CommonRoom.Q_s;				            // Heat Load of SPOT Unit(kW)

	float HeatingEfficiency = 0.9f;
	float CoolingEfficiency = 0.9f;

	float SupplyAirTemperature = SAT_Value;
	float T_c = MixedAirTemperature;

	float CoefficientHeatingPower = (density * specific_heat) / HeatingEfficiency;
	float CoefficientCoolingPower = (density * specific_heat) / CoolingEfficiency;
	float CoefficientFanPower = 0.094f;          // kW.s.s / Kg.Kg

	float HeatingPower = SAV_Zones.sum() * (CoefficientHeatingPower*(SupplyAirTemperature - T_c));
	float CoolingPower = SAV_Zones.sum() * (CoefficientCoolingPower*(MixedAirTemperature - T_c));
	float FanPower = CoefficientFanPower * SAV_Zones.sum();
	float SPOTPower = Q_s * O.sum();

	AHUPower = HeatingPower + CoolingPower + FanPower + SPOTPower;

	return AHUPower;
}
void Building::Simulate(uint32 duration, uint16 time_step, int control_type) {
	int n = (duration / time_step) + 1;

	int total_rooms = num_zones_ * num_rooms_;

	Weather forecast;
	Eigen::MatrixXf T_ext = forecast.GetWeatherForecast(duration, time_step, num_zones_, num_rooms_);

	Occupants occupancy;
	Eigen::MatrixXf O = occupancy.GetOccupancyForecast(duration, time_step, num_zones_, num_rooms_).cast <float>();

	/* Building Parameters For HVAC Impact*/

	// Impact of Weather
	Eigen::MatrixXf CoWI_CRT_Matrix = Create_CoWI_CRT_Matrix(time_step);
	Eigen::MatrixXf WI_CRT(1, total_rooms);

	Eigen::MatrixXf CoWI_OAT_Matrix = Create_CoWI_OAT_Matrix(time_step);
	Eigen::MatrixXf WI_OAT(1, total_rooms);

	// Impact of HVAC
	Eigen::MatrixXf CoHI_CRT_Matrix = Create_CoHI_CRT_Matrix(time_step);
	Eigen::MatrixXf HI_CRT(1, total_rooms);

	Eigen::MatrixXf CoHI_SAT_Matrix = Create_CoHI_SAT_Matrix(time_step);
	Eigen::MatrixXf HI_SAT(1, total_rooms);

	// Impact of Equipments
	Eigen::MatrixXf CoEI_OLEL_Matrix = Create_CoEI_OLEL_Matrix(time_step);
	Eigen::MatrixXf EI_OLEL(1, total_rooms);

	/* Building Parameters For Temperature in SPOT Region */

	// Impact of Room Coupling
	Eigen::MatrixXf CoRC_CiRT_Matrix = Create_CoRC_CiRT_Matrix(time_step);
	Eigen::MatrixXf RC_CiRT(1, total_rooms);

	// Impact of SPOT
	Eigen::MatrixXf CoSI_SCS_Matrix = Create_CoSI_SCS_Matrix(time_step);
	Eigen::MatrixXf SI_SCS(1, total_rooms);

	// Impact of Occupants
	Eigen::MatrixXf CoOI_OHL_Matrix = Create_CoOI_OHL_Matrix(time_step);
	Eigen::MatrixXf OI_OHL(1, total_rooms);

	/* Building Parameters For Temperature in Non-SPOT Region */

	// Impact of Room Coupling
	Eigen::MatrixXf CoRC_CiR1T_Matrix = Create_CoRC_CiR1T_Matrix(time_step);
	Eigen::MatrixXf RC_CiR1T(1, total_rooms);

	/* Output of the Program */

	// Temperature Matrices
	Eigen::MatrixXf T = Eigen::MatrixXf::Zero(n, total_rooms);
	Eigen::MatrixXf TR1 = Eigen::MatrixXf::Zero(n, total_rooms);
	Eigen::MatrixXf TR2 = Eigen::MatrixXf::Zero(n, total_rooms);

	// Delta Matrices
	Eigen::MatrixXf DeltaTR1 = Eigen::MatrixXf::Zero(n, total_rooms);
	Eigen::MatrixXf DeltaTR2 = Eigen::MatrixXf::Zero(n, total_rooms);

	// PPV
	Eigen::MatrixXf PPV = Eigen::MatrixXf::Zero(n, total_rooms);

	// AHU Parameters
	Eigen::MatrixXf MixedAirTemperature = Eigen::MatrixXf::Zero(n, 1);
	Eigen::MatrixXf PowerAHU = Eigen::MatrixXf::Zero(n, 1);

	// Initialization
	int k = 0;
	ControlBox cb;
	ControlVariables CV = cb.DefaultControl(num_zones_, num_rooms_);

	Eigen::MatrixXi SPOT_State = Eigen::MatrixXi::Ones(n, total_rooms);
	SPOT_State.row(k) = CV.SPOT_CurrentState;

	T.row(k) = Eigen::VectorXf::Ones(total_rooms) * 21;
	TR1.row(k) = T.row(k) + DeltaTR1.row(k);
	TR2.row(k) = T.row(k) + DeltaTR2.row(k);

	PPV.row(k) = (0.2466f * TR1.row(k)) - (1.4075f * Eigen::MatrixXf::Zero(1, total_rooms)) +
		(0.581f *  Eigen::MatrixXf::Zero(1, total_rooms)) - (5.4668f *  Eigen::MatrixXf::Ones(1, total_rooms));

	MixedAirTemperature.row(k) << GetMixedAirTemperature(TR1.row(k), T_ext.row(k));
	PowerAHU.row(k) << GetAHUPower(MixedAirTemperature.row(k).value(), O.row(k), CV.SAT_Value, CV.SAV_Zones);

	// Print Initial Values
/*	std::cout << T << std::endl;
	std::cout << TR1 << std::endl;
	std::cout << TR2 << std::endl;

	std::cout << DeltaTR1 << std::endl;
	std::cout << DeltaTR2 << std::endl;

	std::cout << PPV << std::endl;
	std::cout << MixedAirTemperature << std::endl;
*/

	//std::cout << CV.SAV_Matrix << std::endl;
	//std::cout << SPOT_State << std::endl;

	for (k = 0; k < n-1; k++) {

		switch (control_type)
		{
		case 1:
			CV = cb.DefaultControl(num_zones_, num_rooms_);
			break;
		case 2:
			CV = cb.ReactiveControl(num_zones_, num_rooms_, TR1.row(k), O.row(k), k, SPOT_State.row(k));
			break;
		default:
			break;
		}

		SPOT_State.row(k + 1) = CV.SPOT_CurrentState;

		/* Temperature Change in the Room Due to HVAC */

		// Impact of Weather
		WI_CRT = T.row(k) * CoWI_CRT_Matrix;
		WI_OAT = T_ext.row(k) * CoWI_OAT_Matrix;

		// Impact of HVAC
		HI_CRT = T.row(k) * CV.SAV_Matrix * CoHI_CRT_Matrix;
		HI_SAT = CV.SAT * CV.SAV_Matrix * CoHI_SAT_Matrix;

		// Impact of Equipments
		EI_OLEL = O.row(k) * CoEI_OLEL_Matrix;

		T.row(k + 1) = WI_CRT + WI_OAT + HI_CRT + HI_SAT + EI_OLEL;

		/* Temperature Change in SPOT Region*/

		// Impact of Region Coupling
		RC_CiRT = DeltaTR1.row(k) * CoRC_CiRT_Matrix;

		// Impact of SPOT
		SI_SCS = CV.SPOT_CurrentState.cast<float>() * CoSI_SCS_Matrix;

		// Impact of Occupants
		OI_OHL = O.row(k) * CoOI_OHL_Matrix;

		DeltaTR1.row(k + 1) = RC_CiRT + SI_SCS + OI_OHL;
		TR1.row(k + 1) = T.row(k + 1) + DeltaTR1.row(k + 1);

		/* Temperature Change in Non-SPOT Region*/

		// Impact of Region Coupling
		RC_CiR1T = DeltaTR1.row(k) * CoRC_CiR1T_Matrix;

		DeltaTR2.row(k + 1) = RC_CiR1T;
		TR2.row(k + 1) = T.row(k + 1) + DeltaTR2.row(k + 1);

		PPV.row(k+1) = (0.2466f * TR1.row(k+1)) - (1.4075f * Eigen::MatrixXf::Zero(1, total_rooms)) +
			(0.581f *  Eigen::MatrixXf::Zero(1, total_rooms)) - (5.4668f *  Eigen::MatrixXf::Ones(1, total_rooms));

		MixedAirTemperature.row(k+1) << GetMixedAirTemperature(TR1.row(k+1), T_ext.row(k+1));
		PowerAHU.row(k+1) << GetAHUPower(MixedAirTemperature.row(k+1).value(), O.row(k), CV.SAT_Value, CV.SAV_Zones);
	}

	// Print Final Values

	//std::cout << SPOT_State << std::endl;
	//std::cout << TR1 << std::endl;
	//std::cout << TR2 << std::endl;

	//std::cout << DeltaTR1 << std::endl;
	//std::cout << DeltaTR2 << std::endl;

	//std::cout << PPV << std::endl;
	//std::cout << MixedAirTemperature << std::endl;
}


