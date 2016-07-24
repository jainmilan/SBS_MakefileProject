#include "ampl/ampl.h"
#include <math.h>
#include <string>
#include "ControlBox.h"

using namespace SimpleBuildingSimulator;

ControlBox::ControlBox() {
}

ControlBox::~ControlBox() {
}

std::string IntToString ( int number )
{
  std::ostringstream oss;

  // Works just like cout
  oss<< number;

  // Return the underlying string
  return oss.str();
}

std::string DoubleToString ( double number )
{
  std::ostringstream oss;

  // Works just like cout
  oss<< number;

  // Return the underlying string
  return oss.str();
}

double gen_random(double lower_limit, double upper_limit, int dec = 0) {
	/* Random Error */
	double ll = lower_limit * (double) (10 ^ dec);
	double ul = upper_limit * (double) (10 ^ dec);

	double rand_int = (double) (rand() % (int) (ul - ll + 1.0) + ll) / (double) (10 ^ dec);
	return rand_int;
}

MAT_FLOAT ControlBox::GetSAVMatrix(MAT_FLOAT SAV_Zones, int num_rooms, int total_rooms) {
	MAT_FLOAT SAV_Zones_Rep = SAV_Zones.replicate(1, num_rooms);

	Eigen::VectorXf SAV_Matrix_Diagonal(
			Eigen::Map<Eigen::VectorXf>(SAV_Zones_Rep.data(),
					SAV_Zones_Rep.cols() * SAV_Zones_Rep.rows()));

	MAT_FLOAT SAVMatrix = MAT_FLOAT::Zero(total_rooms, total_rooms);
	SAVMatrix = SAV_Matrix_Diagonal.asDiagonal();

	return SAVMatrix;
}

struct ControlVariables ControlBox::DefaultControl(const int& total_rooms, const PARAMS& ParamsIn) {
	struct ControlVariables cv;

	cv.SAT_Value = 30;

	cv.r = 0.8;

	cv.SAT = MAT_FLOAT::Ones(1, total_rooms) * cv.SAT_Value;

	cv.SAV_Zones = MAT_FLOAT::Ones(ParamsIn.CommonBuilding.num_zones_, 1) * 0.05f;

	cv.SAV_Matrix = GetSAVMatrix(cv.SAV_Zones, ParamsIn.CommonBuilding.num_rooms_, total_rooms);

	cv.SPOT_CurrentState = MAT_FLOAT::Ones(1, total_rooms);

	return cv;
}

struct ControlVariables ControlBox::ReactiveControl(const int& total_rooms, MAT_FLOAT TR1, MAT_FLOAT O,
		int k, MAT_FLOAT SPOT_PreviousState, const PARAMS& ParamsIn) {

	struct ControlVariables cv;

	cv.SAT_Value = 30;

	cv.SAT = MAT_FLOAT::Ones(1, total_rooms) * cv.SAT_Value;

	cv.SAV_Zones = MAT_FLOAT::Ones(ParamsIn.CommonBuilding.num_zones_, 1) * 0.05f;

	cv.SAV_Matrix = GetSAVMatrix(cv.SAV_Zones, ParamsIn.CommonBuilding.num_rooms_, total_rooms);

	cv.SPOT_CurrentState = SPOT_PreviousState;

	std::cout << cv.SPOT_CurrentState(0, 0) << " " << O.col(0).value() << " " << TR1.col(0).value() << std::endl;

	if (k == 0) {
		return cv;
	}

	for (size_t i = 0, nCols = cv.SPOT_CurrentState.cols(); i < nCols; i++) {
		// If room is occupied
		if (O.col(i).value() == 1) {
			if ((TR1.col(i).value() > 23)
					&& (SPOT_PreviousState.col(i).value() == 1)) {
				cv.SPOT_CurrentState(0, i) = 0;
			} else if ((TR1.col(i).value() < 21)
					&& (SPOT_PreviousState.col(i).value() == 0)) {
				cv.SPOT_CurrentState(0, i) = 1;
			}
		}
		// If room is unoccupied
		else if (O.col(i).value() == 0) {
			if ((TR1.col(i).value() > 28)
					&& (SPOT_PreviousState.col(i).value() == 1)) {
				cv.SPOT_CurrentState(0, i) = 0;
			} else if ((TR1.col(i).value() < 18)
					&& (SPOT_PreviousState.col(i).value() == 0)) {
				cv.SPOT_CurrentState(0, i) = 1;
			}
		}
	}

	return cv;
}

float ControlBox::MPCControl(DF_OUTPUT df[], const long int& tinstances, const int& time_step, MAT_FLOAT T_Outside,
		MAT_FLOAT Occupancy, MAT_FLOAT TNoSPOTInit,	MAT_FLOAT DeltaTSPOTInit, const PARAMS& ParamsIn, const int& horizon,
		const int& Time_IH,	ControlVariables& CV, int current_index) {

	// Initialize AMPL and Control Variables
	ampl::AMPL ampl;

	// Generic Parameters
	int tau = time_step;
	float res_val = 0;
	int total_rooms = ParamsIn.CommonBuilding.num_zones_ * ParamsIn.CommonBuilding.num_rooms_;

	// Building Parameters
	float C = ParamsIn.CommonRoom.C;
	float C_ = ParamsIn.CommonRoom.C_;

	float alpha_o = ParamsIn.CommonRoom.alpha_o;
	float alpha_r = ParamsIn.CommonRoom.alpha_r;

	float Q_l = ParamsIn.CommonRoom.Q_l;
	float Q_h = ParamsIn.CommonRoom.Q_h;
	float Q_s = ParamsIn.CommonRoom.Q_s;

	float density = ParamsIn.CommonAir.density;
	float specific_heat = ParamsIn.CommonAir.specific_heat;

	int buffer1 = 0;
	int buffer2 = 0;

	// Limits on Variables
	float SAT_ll = 12;
	float SAT_ul = 35;

	float SAV_ll = 0.236;
	float SAV_ul = 4;

	float PMV_ll = -0.29;
	float PMV_ul = 0.23;

	float T_ll = 18;
	float T_ul = 28;

	float T_SPOT_ll = 18;
	float T_SPOT_ul = 28;

	float T_NoSPOT_ll = 18;
	float T_NoSPOT_ul = 28;

	// Read the model and data files.
	std::string modelDirectory = "models";
	try {
		ampl.read(modelDirectory + "/mpcmodel.mod");

		// Initialize Parameters - Basic Parameters
		// std::cout << "Duration: " << tinstances << "\n";
		ampl::Parameter pDuration = ampl.getParameter("duration");
		double pDurationA[] = { (double) tinstances };
		pDuration.setValues(pDurationA, 1);

		// std::cout << "Total Rooms: " << total_rooms << "\n";
		ampl::Parameter pTotalRooms = ampl.getParameter("total_rooms");
		double pTotalRoomsv = total_rooms;
		double pTotalRoomsA[] = { pTotalRoomsv };
		pTotalRooms.setValues(pTotalRoomsA, 1);

		// std::cout << "Horizon: " << horizon << "\n";
		ampl::Parameter pHorizon = ampl.getParameter("Nhorizon");
		double pHorizonA[] = { (double) horizon };
		pHorizon.setValues(pHorizonA, 1);

		// std::cout << "Time_IH: " << Time_IH << "\n";
		ampl::Parameter pTimeIH = ampl.getParameter("Time_IH");
		double pTimeIHA[] = { (double) Time_IH };
		pTimeIH.setValues(pTimeIHA, 1);

		if (Time_IH > 0) {
			buffer1 = 1;
			buffer2 = Time_IH;
		}

		// std::cout << "Buffer1: " << buffer1 << "\n";
		ampl::Parameter pBuffer1 = ampl.getParameter("buffer1");
		double pBuffer1A[] = { (double) buffer1 };
		pBuffer1.setValues(pBuffer1A, 1);

		// std::cout << "Buffer2: " << buffer2 << "\n";
		ampl::Parameter pBuffer2 = ampl.getParameter("buffer2");
		double pBuffer2A[] = { (double) buffer2 };
		pBuffer2.setValues(pBuffer2A, 1);

		ampl::Parameter pSATPrev = ampl.getParameter("SAT_Prev");
		double pSATPrevv = (double) round(CV.SAT_Value * 100) / 100;
		double pSATPrevA[] = { pSATPrevv };
		pSATPrev.setValues(pSATPrevA, 1);
		// std::cout << "SAT_Prev: " << pSATPrevv << "\n";

		// Initialize Parameters -Parameters for Objective Function
		ampl::Parameter pCoHP = ampl.getParameter("Coefficient_Heating_Power");
		double pCoHPv = (density * specific_heat)
				/ ParamsIn.CommonAHU.HeatingEfficiency;
		double pCoHPA[] = { pCoHPv };
		pCoHP.setValues(pCoHPA, 1);
		// std::cout << "Coefficient_Heating_Power: " << pCoHPv << "\n";

		ampl::Parameter pCoCP = ampl.getParameter("Coefficient_Cooling_Power");
		double pCoCPv = (density * specific_heat)
				/ ParamsIn.CommonAHU.CoolingEfficiency;
		double pCoCPA[] = { pCoCPv };
		pCoCP.setValues(pCoCPA, 1);
		// std::cout << "Coefficient_Cooling_Power: " << pCoCPv << "\n";

		ampl::Parameter pCoFP = ampl.getParameter("Coefficient_Fan_Power");
		double pCoFPv = ParamsIn.CommonRoom.fan_coef;
		double pCoFPA[] = { pCoFPv };
		pCoFP.setValues(pCoFPA, 1);
		// std::cout << "Coefficient_Fan_Power: " << pCoFPv << "\n";

		ampl::Parameter pCoSP = ampl.getParameter("Coefficient_SPOT_Power");
		double pCoSPv = Q_s;
		double pCoSPA[] = { pCoSPv };
		pCoSP.setValues(pCoSPA, 1);
		// std::cout << "Coefficient_SPOT_Power: " << pCoSPv << "\n";

		// Initialize Parameters - Coefficient Values
		ampl::Parameter pCoWICRT = ampl.getParameter("CoWI_CRT");
		double pCoWICRTv = 1 - ((alpha_o * tau) / C);
		double pCoWICRTA[] = { pCoWICRTv };
		pCoWICRT.setValues(pCoWICRTA, 1);
		// std::cout << "CoWI_CRT: " << pCoWICRTv << "\n";

		ampl::Parameter pCoWIOAT = ampl.getParameter("CoWI_OAT");
		double pCoWIOATv = (alpha_o * tau) / C;
		double pCoWIOATA[] = { pCoWIOATv };
		pCoWIOAT.setValues(pCoWIOATA, 1);
		// std::cout << "CoWI_OAT: " << pCoWIOATv << "\n";

		ampl::Parameter pCoHICRT = ampl.getParameter("CoHI_CRT");
		double pCoHICRTv = (-1 * tau * density * specific_heat)
				/ (ParamsIn.CommonBuilding.num_rooms_ * C);
		double pCoHICRTA[] = { pCoHICRTv };
		pCoHICRT.setValues(pCoHICRTA, 1);
		// std::cout << "CoHI_CRT: " << pCoHICRTv << "\n";

		ampl::Parameter pCoHISAT = ampl.getParameter("CoHI_SAT");
		double pCoHISATv = (tau * density * specific_heat) / (ParamsIn.CommonBuilding.num_rooms_ * C);
		double pCoHISATA[] = { pCoHISATv };
		pCoHISAT.setValues(pCoHISATA, 1);
		// std::cout << "CoHI_SAT: " << pCoHISATv << "\n";

		ampl::Parameter pCoEIOLEL = ampl.getParameter("CoEI_OLEL");
		double pCoEIOLELv = (tau * Q_l) / C;
		double pCoEIOLELA[] = { pCoEIOLELv };
		pCoEIOLEL.setValues(pCoEIOLELA, 1);
		// std::cout << "CoEI_OLEL: " << pCoEIOLELv << "\n";

		ampl::Parameter pCoOIOHL = ampl.getParameter("CoOI_OHL");
		double pCoOIOHLv = (tau * Q_h) / C_;
		double pCoOIOHLA[] = { pCoOIOHLv };
		pCoOIOHL.setValues(pCoOIOHLA, 1);
		// std::cout << "CoOI_OHL: " << pCoOIOHLv << "\n";

		ampl::Parameter pCoSISCS = ampl.getParameter("CoSI_SCS");
		double pCoSISCSv = (tau * Q_s) / C_;
		double pCoSISCSA[] = { pCoSISCSv };
		pCoSISCS.setValues(pCoSISCSA, 1);
		// std::cout << "CoSI_SCS: " << pCoSISCSv << "\n";

		ampl::Parameter pCoRCCiRT = ampl.getParameter("CoRC_CiRT");
		double pCoRCCiRTv = 1 - ((tau * alpha_r) / C_);
		double pCoRCCiRTA[] = { pCoRCCiRTv };
		pCoRCCiRT.setValues(pCoRCCiRTA, 1);
		// std::cout << "CoRC_CiRT: " << pCoRCCiRTv << "\n";

		ampl::Parameter pCoRCCiR1T = ampl.getParameter("CoRC_CiR1T");
		double pCoRCCiR1Tv = (tau * alpha_r) / (C - C_);
		double pCoRCCiR1TA[] = { pCoRCCiR1Tv };
		pCoRCCiR1T.setValues(pCoRCCiR1TA, 1);
		// std::cout << "CoRC_CiR1T: " << pCoRCCiR1Tv << "\n";

		// Initialize Parameters - PMV Model
		ampl::Parameter ppmvp1 = ampl.getParameter("pmv_p1");
		double ppmvp1v = ParamsIn.PMV_Params.P1;
		double ppmvp1A[] = { ppmvp1v };
		ppmvp1.setValues(ppmvp1A, 1);
		// std::cout << "PMV P1: " << ppmvp1v << "\n";

		ampl::Parameter ppmvp2 = ampl.getParameter("pmv_p2");
		double ppmvp2v = ParamsIn.PMV_Params.P2;
		double ppmvp2A[] = { ppmvp2v };
		ppmvp2.setValues(ppmvp2A, 1);
		// std::cout << "PMV P2: " << ppmvp2v << "\n";

		ampl::Parameter ppmvp3 = ampl.getParameter("pmv_p3");
		double ppmvp3v = ParamsIn.PMV_Params.P3;
		double ppmvp3A[] = { ppmvp3v };
		ppmvp3.setValues(ppmvp3A, 1);
		// std::cout << "PMV P3: " << ppmvp3v << "\n";

		ampl::Parameter ppmvp4 = ampl.getParameter("pmv_p4");
		double ppmvp4v = ParamsIn.PMV_Params.P4;
		double ppmvp4A[] = { ppmvp4v };
		ppmvp4.setValues(ppmvp4A, 1);
		// std::cout << "PMV P4: " << ppmvp4v << "\n";

		// Initialize Parameters - Limits on Variables
		ampl::Parameter pSATll = ampl.getParameter("SAT_ll");
		double pSATllv = SAT_ll;
		double pSATllA[] = { pSATllv };
		pSATll.setValues(pSATllA, 1);
		// std::cout << "SAT_ll: " << pSATllv << "\n";

		ampl::Parameter pSATul = ampl.getParameter("SAT_ul");
		double pSATulv = SAT_ul;
		double pSATulA[] = { pSATulv };
		pSATul.setValues(pSATulA, 1);
		// std::cout << "SAT_ul: " << pSATulv << "\n";

		ampl::Parameter pSAVll = ampl.getParameter("SAV_ll");
		double pSAVllv = SAV_ll;
		double pSAVllA[] = { pSAVllv };
		pSAVll.setValues(pSAVllA, 1);
		// std::cout << "SAV_ll: " << pSAVllv << "\n";

		ampl::Parameter pSAVul = ampl.getParameter("SAV_ul");
		double pSAVulv = SAV_ul;
		double pSAVulA[] = { pSAVulv };
		pSAVul.setValues(pSAVulA, 1);
		// std::cout << "SAV_ul: " << pSAVulv << "\n";

		ampl::Parameter pPMVll = ampl.getParameter("PMV_ll");
		double pPMVllv = PMV_ll;
		double pPMVllA[] = { pPMVllv };
		pPMVll.setValues(pPMVllA, 1);
		// std::cout << "PMV_ll: " << pPMVllv << "\n";

		ampl::Parameter pPMVul = ampl.getParameter("PMV_ul");
		double pPMVulv = PMV_ul;
		double pPMVulA[] = { pPMVulv };
		pPMVul.setValues(pPMVulA, 1);
		// std::cout << "PMV_ul: " << pPMVulv << "\n";

		ampl::Parameter pTll = ampl.getParameter("T_ll");
		double pTllv = T_ll;
		double pTllA[] = { pTllv };
		pTll.setValues(pTllA, 1);
		// std::cout << "T_ll: " << pTllv << "\n";

		ampl::Parameter pTul = ampl.getParameter("T_ul");
		double pTulv = T_ul;
		double pTulA[] = { pTulv };
		pTul.setValues(pTulA, 1);
		// std::cout << "T_ul: " << pTulv << "\n";

		ampl::Parameter pTSPOTll = ampl.getParameter("T_SPOT_ll");
		double pTSPOTllv = T_SPOT_ll;
		double pTSPOTllA[] = { pTSPOTllv };
		pTSPOTll.setValues(pTSPOTllA, 1);
		// std::cout << "T_SPOT_ll: " << pTSPOTllv << "\n";

		ampl::Parameter pTSPOTul = ampl.getParameter("T_SPOT_ul");
		double pTSPOTulv = T_SPOT_ul;
		double pTSPOTulA[] = { pTSPOTulv };
		pTSPOTul.setValues(pTSPOTulA, 1);
		// std::cout << "T_SPOT_ul: " << pTSPOTulv << "\n";

		ampl::Parameter pTNoSPOTll = ampl.getParameter("T_NoSPOT_ll");
		double pTNoSPOTllv = T_NoSPOT_ll;
		double pTNoSPOTllA[] = { pTNoSPOTllv };
		pTNoSPOTll.setValues(pTNoSPOTllA, 1);
		// std::cout << "T_NoSPOT_ll: " << pTNoSPOTllv << "\n";

		ampl::Parameter pTNoSPOTul = ampl.getParameter("T_NoSPOT_ul");
		double pTNoSPOTulv = T_NoSPOT_ul;
		double pTNoSPOTulA[] = { pTNoSPOTulv };
		pTNoSPOTul.setValues(pTNoSPOTulA, 1);
		// std::cout << "T_NoSPOT_ul: " << pTNoSPOTulv << "\n";

		// Initialize Parameters - Input Data
		double *pTOutsideA = new double[tinstances];
		for (int i = 0; i < tinstances; i++) {
			pTOutsideA[i] = T_Outside.row(i).mean();
		}
		ampl::Parameter pTOutside = ampl.getParameter("T_Outside");
		pTOutside.setValues(pTOutsideA, tinstances);
		// std::cout << "T_Outside: " << T_Outside << "\n";

		double *pOccupancyA = new double[tinstances * total_rooms];
		double *pOccupancyRows = new double[tinstances];
		double *pOccupancyCols = new double[total_rooms];

		for (int i = 0; i < tinstances; i++) {
			pOccupancyRows[i] = i + 1;
			for (int j = 0; j < total_rooms; j++) {
				pOccupancyA[i * total_rooms + j] = Occupancy(i, j);
			}
		}
		for (int i = 0; i < total_rooms; i++) {
			pOccupancyCols[i] = i + 1;
		}

		ampl::Parameter pOccupancy = ampl.getParameter("Occupancy");
		pOccupancy.setValues(tinstances, pOccupancyRows, total_rooms,
				pOccupancyCols, pOccupancyA, false);
		// std::cout << "Occupancy: " << Occupancy << "\n";

		double *pTNoSPOTInitA = new double[total_rooms];
		for (int i = 0; i < total_rooms; i++) {
			pTNoSPOTInitA[i] = TNoSPOTInit(0, i);
		}
		ampl::Parameter pTNoSPOTInit = ampl.getParameter("T_NoSPOT_Init");
		pTNoSPOTInit.setValues(pTNoSPOTInitA, total_rooms);
		// std::cout << "T_NoSPOT_Init: " << TNoSPOTInit << "\n";

		double *pDeltaTSPOTInitA = new double[total_rooms];
		for (int i = 0; i < total_rooms; i++) {
			pDeltaTSPOTInitA[i] = DeltaTSPOTInit(0, i);
		}
		ampl::Parameter pDeltaTSPOTInit = ampl.getParameter(
				"Delta_T_SPOT_Init");
		pDeltaTSPOTInit.setValues(pDeltaTSPOTInitA, total_rooms);
		// std::cout << "Delta_T_SPOT_Init: " << DeltaTSPOTInit << "\n";

		int current_seed, i, j = 0;

		/* Initial Guess */
		for(i = 1; i < 17; i++) {
			current_seed = current_seed + i;

			for(j = 0; j < tinstances; ++j) {
				current_seed = current_seed + 1;
				// srand(current_seed);
				double vTMixingUnitInit = gen_random(2, 25);
				ampl.eval("let T_Mixing_Unit[" + IntToString(j + 1) + "] := " + DoubleToString(vTMixingUnitInit) + ";");

				current_seed = current_seed + 1;
				// srand(current_seed);
				double vTCoolingUnitInit = gen_random(2, 25);
				ampl.eval("let T_Cooling_Unit[" + IntToString(j + 1) + "] := " + DoubleToString(vTCoolingUnitInit) + ";");

				current_seed = current_seed + 1;
				// srand(current_seed);
				double vSATInit = gen_random(12, 35);
				ampl.eval("let SAT[" + IntToString(j + 1) + "] := " + DoubleToString(vSATInit) + ";");

				current_seed = current_seed + 1;
				// srand(current_seed);
				double vSAVInit = gen_random(0.2, 4, 1);
				ampl.eval("let SAV[" + IntToString(j + 1) + "] := " + DoubleToString(vSAVInit) + ";");

				current_seed = current_seed + 1;
				// srand(current_seed);
				double vRatioInit = gen_random(0, 0.8, 1);
				ampl.eval("let Ratio[" + IntToString(j + 1) + "] := " + DoubleToString(vRatioInit) + ";");

				for(int k = 0; k < total_rooms; k++) {
					current_seed = current_seed + 1;
					// srand(current_seed);
					double vDeltaTSPOTInit = gen_random(0, 4);
					ampl.eval("let Delta_T_SPOT[" + IntToString(j + 1) + ", " + IntToString(k + 1) + "] := " + DoubleToString(vDeltaTSPOTInit) + ";");

					current_seed = current_seed + 1;
					// srand(current_seed);
					double vTNoSPOTInit = gen_random(18, 28);
					ampl.eval("let T_NoSPOT[" + IntToString(j + 1) + ", " + IntToString(k + 1) + "] := " + DoubleToString(vTNoSPOTInit) + ";");

					current_seed = current_seed + 1;
					// srand(current_seed);
					double vSPOTStatusInit = gen_random(0, 1, 1);
					ampl.eval("let SPOT_Status[" + IntToString(j + 1) + ", " + IntToString(k + 1) + "] := " + DoubleToString(vSPOTStatusInit) + ";");

					current_seed = current_seed + 1;
					// srand(current_seed);
					double vPMVInit = gen_random(-0.29, 0.23, 2);
					ampl.eval("let PMV[" + IntToString(j + 2) + ", " + IntToString(k + 1) + "] := " + DoubleToString(vPMVInit) + ";");

					current_seed = current_seed + 1;
					// srand(current_seed);
					double vFanSpeedInit = 0.0;
					ampl.eval("let Fan_Speed[" + IntToString(j + 1) + ", " + IntToString(k + 1) + "] := " + DoubleToString(vFanSpeedInit) + ";");
				}

			}

			for(int k = 0; k < total_rooms; k++) {
				current_seed = current_seed + 1;
				// srand(current_seed);
				double vDeltaTSPOTInit = gen_random(0, 4);
				ampl.eval("let Delta_T_SPOT[" + IntToString(j + 1) + ", " + IntToString(k + 1) + "] := " + DoubleToString(vDeltaTSPOTInit) + ";");

				current_seed = current_seed + 1;
				// srand(current_seed);
				double vTNoSPOTInit = gen_random(18, 28);
				ampl.eval("let T_NoSPOT[" + IntToString(j + 1) + ", " + IntToString(k + 1) + "] := " + DoubleToString(vTNoSPOTInit) + ";");
			}

			// Resolve and display objective
			ampl.solve();
			ampl::Objective totalcost = ampl.getObjective("total_power");

			ampl.eval("let response := solve_result_num;");

			// Get final value of response
			ampl::Parameter response = ampl.getParameter("response");
			res_val = response.get().dbl();
			std::cout << res_val << "\n";

			if (res_val >= 199) {
				continue;
			}

/*			std::cout << "vDeltaTSPOTInit: " << vDeltaTSPOTInit << "\n";
			std::cout << "vTNoSPOTInit: " << vTNoSPOTInit << "\n";
			std::cout << "vTMixingUnitInit: " << vTMixingUnitInit << "\n";
			std::cout << "vTCoolingUnitInit: " << vTCoolingUnitInit << "\n";
			std::cout << "vSATInit: " << vSATInit << "\n";
			std::cout << "vSAVInit: " << vSAVInit << "\n";
			std::cout << "vSPOTStatusInit: " << vSPOTStatusInit << "\n";
			std::cout << "vRatioInit: " << vRatioInit << "\n";
			std::cout << "vPMVInit: " << vPMVInit << "\n";
			std::cout << "vFanSpeedInit: " << vFanSpeedInit << "\n"; */
			break;
		}

		// std::cout << "Selected Seed: " << current_seed << " at: " << i << "\n";

		// Get final value of Supply Air Temperature (SAT)
		ampl::Variable vSAT = ampl.getVariable("SAT");
		ampl::DataFrame dfSAT = vSAT.getValues();

		CV.SAT_Value = dfSAT.getRowByIndex(0)[1].dbl();
		CV.SAT = Eigen::MatrixXf::Ones(1, total_rooms) * CV.SAT_Value;

		// Get final value of Supply Air Volume (SAV)
		ampl::Variable vR = ampl.getVariable("Ratio");
		ampl::DataFrame dfR = vR.getValues();

		// std::cout << dfR.toString() << std::endl;
		CV.r = dfR.getRowByIndex(0)[1].dbl();
		// std::cout << "SAT Values Are: " << cv.SAT << std::endl;

		// BUG: Model returns single value of SAV, however it should be for each zone
		// Get final value of Supply Air Volume (SAV)
		ampl::Variable vSAV = ampl.getVariable("SAV");
		ampl::DataFrame dfSAV = vSAV.getValues();

		CV.SAV_Zones = Eigen::MatrixXf::Ones(ParamsIn.CommonBuilding.num_zones_, 1)
				* (dfSAV.getRowByIndex(0)[1].dbl() / total_rooms);
		CV.SAV_Matrix = GetSAVMatrix(CV.SAV_Zones, ParamsIn.CommonBuilding.num_rooms_, total_rooms);
		// std::cout << "SAV Values Are: " << cv.SAV_Matrix << std::endl;

		// Get final value of SPOT Status
		// std::string command = "{j in 1.." + IntToString(total_rooms) + "} SPOT_Status[1,j]";
		// ampl::DataFrame dfSPOTStatusNew = ampl.getData(command.c_str());

		ampl::Variable vSPOTStatus = ampl.getVariable("SPOT_Status");
		ampl::DataFrame dfSPOTStatus = vSPOTStatus.getValues();
		// std::cout << dfSPOTStatusNew.toString() << "\n";

		size_t nRows = dfSPOTStatus.getNumRows();
		CV.SPOT_CurrentState = Eigen::MatrixXf::Ones(1, total_rooms);
		for (size_t i = 0, nCols = CV.SPOT_CurrentState.cols(); i < nCols;
				i++) {
			CV.SPOT_CurrentState(0, i) = dfSPOTStatus.getRowByIndex(i)[2].dbl(); // 2 is the column of returned dataframe
		}
		// std::cout << "SPOT Status: " << CV.SPOT_CurrentState << std::endl;
		// std::cout << "Objective Is: " << totalcost.value() << std::endl;
	} catch (const std::exception &exc) {
		std::cerr << "Solver Not Working!!";
		std::cerr << exc.what();
	}

	return res_val;
}
