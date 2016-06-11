#include "ampl/ampl.h"
#include "ControlBox.h"

using namespace SimpleBuildingSimulator;

ControlBox::ControlBox() {
}

ControlBox::~ControlBox() {
}

Eigen::MatrixXf ControlBox::GetSAVMatrix(Eigen::MatrixXf SAV_Zones,
		int num_rooms, int total_rooms) {
	Eigen::MatrixXf SAV_Zones_Rep = SAV_Zones.replicate(1, num_rooms);

	Eigen::VectorXf SAV_Matrix_Diagonal(
			Eigen::Map<Eigen::VectorXf>(SAV_Zones_Rep.data(),
					SAV_Zones_Rep.cols() * SAV_Zones_Rep.rows()));

	Eigen::MatrixXf SAVMatrix = Eigen::MatrixXf::Zero(total_rooms, total_rooms);
	SAVMatrix = SAV_Matrix_Diagonal.asDiagonal();

	return SAVMatrix;
}

struct ControlVariables ControlBox::DefaultControl(int num_zones,
		int num_rooms) {
	int total_rooms = num_zones * num_rooms;

	struct ControlVariables cv;

	cv.SAT_Value = 30;

	cv.SAT = Eigen::MatrixXf::Ones(1, total_rooms) * cv.SAT_Value;

	cv.SAV_Zones = Eigen::MatrixXf::Ones(num_zones, 1) * 0.05f;

	cv.SAV_Matrix = GetSAVMatrix(cv.SAV_Zones, num_rooms, total_rooms);

	cv.SPOT_CurrentState = Eigen::MatrixXi::Ones(1, total_rooms);

	return cv;
}

struct ControlVariables ControlBox::ReactiveControl(int num_zones,
		int num_rooms, Eigen::MatrixXf TR1, Eigen::MatrixXf O, int k,
		Eigen::MatrixXi SPOT_PreviousState) {

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

struct ControlVariables ControlBox::MPCControl(int num_zones, int num_rooms,
		long int duration, int time_step, Building::Air air_params,
		Building::Room room_params, Building::AHU ahu_params,
		Building::PMV_Model pmv_params, Eigen::MatrixXf T_Outside,
		Eigen::MatrixXf Occupancy, Eigen::MatrixXf TNoSPOTInit,
		Eigen::MatrixXf DeltaTSPOTInit) {

	// Initialize AMPL and Control Variables
	ampl::AMPL ampl;
	struct ControlVariables cv;

	// Generic Parameters
	int tau = time_step;
	long int tinstances = duration / tau;
	int total_rooms = num_zones * num_rooms;

	// Building Parameters
	float C = room_params.C;
	float C_ = room_params.C_;

	float alpha_o = room_params.alpha_o;
	float alpha_r = room_params.alpha_r;

	float Q_l = room_params.Q_l;
	float Q_h = room_params.Q_h;
	float Q_s = room_params.Q_s;

	float density = air_params.density;
	float specific_heat = air_params.specific_heat;

	// Limits on Variables
	float SAT_ll = 12;
	float SAT_ul = 35;

	float SAV_ll = 0.236;
	float SAV_ul = 4;

	float PMV_ll = -0.29;
	float PMV_ul = 0.23;

	float T_ll = -20;
	float T_ul = 20;

	float T_SPOT_ll = -20;
	float T_SPOT_ul = 20;

	float T_NoSPOT_ll = -20;
	float T_NoSPOT_ul = 20;

	// Read the model and data files.
	std::string modelDirectory = "./models";
	try {
		ampl.read(modelDirectory + "/mpcmodel.mod");

		// Initialize Parameters - Basic Parameters
		ampl::Parameter pDuration = ampl.getParameter("duration");
		double pDurationA[] = { (double) tinstances };
		pDuration.setValues(pDurationA, 1);

		ampl::Parameter pTotalRooms = ampl.getParameter("total_rooms");
		double pTotalRoomsv = total_rooms;
		double pTotalRoomsA[] = { pTotalRoomsv };
		pTotalRooms.setValues(pTotalRoomsA, 1);

		// Initialize Parameters -Parameters for Objective Function
		ampl::Parameter pCoHP = ampl.getParameter("Coefficient_Heating_Power");
		double pCoHPv = (density * specific_heat)
				/ ahu_params.HeatingEfficiency;
		double pCoHPA[] = { pCoHPv };
		pCoHP.setValues(pCoHPA, 1);

		ampl::Parameter pCoCP = ampl.getParameter("Coefficient_Cooling_Power");
		double pCoCPv = (density * specific_heat)
				/ ahu_params.CoolingEfficiency;
		double pCoCPA[] = { pCoCPv };
		pCoCP.setValues(pCoCPA, 1);

		ampl::Parameter pCoFP = ampl.getParameter("Coefficient_Fan_Power");
		double pCoFPA[] = { room_params.fan_coef };
		pCoFP.setValues(pCoFPA, 1);

		ampl::Parameter pCoSP = ampl.getParameter("Coefficient_SPOT_Power");
		double pCoSPA[] = { Q_s };
		pCoSP.setValues(pCoSPA, 1);

		// Initialize Parameters - Coefficient Values
		ampl::Parameter pCoWICRT = ampl.getParameter("CoWI_CRT");
		double pCoWICRTv = 1 - ((alpha_o * tau) / C);
		double pCoWICRTA[] = { pCoWICRTv };
		pCoWICRT.setValues(pCoWICRTA, 1);

		ampl::Parameter pCoWIOAT = ampl.getParameter("CoWI_OAT");
		double pCoWIOATv = (alpha_o * tau) / C;
		double pCoWIOATA[] = { pCoWIOATv };
		pCoWIOAT.setValues(pCoWIOATA, 1);

		ampl::Parameter pCoHICRT = ampl.getParameter("CoHI_CRT");
		double pCoHICRTv = (-1 * tau * density * specific_heat)
				/ (num_rooms * C);
		double pCoHICRTA[] = { pCoHICRTv };
		pCoHICRT.setValues(pCoHICRTA, 1);

		ampl::Parameter pCoHISAT = ampl.getParameter("CoHI_SAT");
		double pCoHISATv = (tau * density * specific_heat) / (num_rooms * C);
		double pCoHISATA[] = { pCoHISATv };
		pCoHISAT.setValues(pCoHISATA, 1);

		ampl::Parameter pCoEIOLEL = ampl.getParameter("CoEI_OLEL");
		double pCoEIOLELv = (tau * Q_l) / C;
		double pCoEIOLELA[] = { pCoEIOLELv };
		pCoEIOLEL.setValues(pCoEIOLELA, 1);

		ampl::Parameter pCoOIOHL = ampl.getParameter("CoOI_OHL");
		double pCoOIOHLv = (tau * Q_h) / C_;
		double pCoOIOHLA[] = { pCoOIOHLv };
		pCoOIOHL.setValues(pCoOIOHLA, 1);

		ampl::Parameter pCoSISCS = ampl.getParameter("CoSI_SCS");
		double pCoSISCSv = (tau * Q_s) / C_;
		double pCoSISCSA[] = { pCoSISCSv };
		pCoSISCS.setValues(pCoSISCSA, 1);

		ampl::Parameter pCoRCCiRT = ampl.getParameter("CoRC_CiRT");
		double pCoRCCiRTv = 1 - ((tau * alpha_r) / C_);
		double pCoRCCiRTA[] = { pCoRCCiRTv };
		pCoRCCiRT.setValues(pCoRCCiRTA, 1);

		ampl::Parameter pCoRCCiR1T = ampl.getParameter("CoRC_CiR1T");
		double pCoRCCiR1Tv = (tau * alpha_r) / (C - C_);
		double pCoRCCiR1TA[] = { pCoRCCiR1Tv };
		pCoRCCiR1T.setValues(pCoRCCiR1TA, 1);

		// Initialize Parameters - PMV Model
		ampl::Parameter ppmvp1 = ampl.getParameter("pmv_p1");
		double ppmvp1A[] = { pmv_params.P1 };
		ppmvp1.setValues(ppmvp1A, 1);

		ampl::Parameter ppmvp2 = ampl.getParameter("pmv_p2");
		double ppmvp2A[] = { pmv_params.P2 };
		ppmvp2.setValues(ppmvp2A, 1);

		ampl::Parameter ppmvp3 = ampl.getParameter("pmv_p3");
		double ppmvp3A[] = { pmv_params.P3 };
		ppmvp3.setValues(ppmvp3A, 1);

		ampl::Parameter ppmvp4 = ampl.getParameter("pmv_p4");
		double ppmvp4A[] = { pmv_params.P4 };
		ppmvp4.setValues(ppmvp4A, 1);

		// Initialize Parameters - Limits on Variables
		ampl::Parameter pSATll = ampl.getParameter("SAT_ll");
		double pSATllA[] = { SAT_ll };
		pSATll.setValues(pSATllA, 1);

		ampl::Parameter pSATul = ampl.getParameter("SAT_ul");
		double pSATulA[] = { SAT_ul };
		pSATul.setValues(pSATulA, 1);

		ampl::Parameter pSAVll = ampl.getParameter("SAV_ll");
		double pSAVllA[] = { SAV_ll };
		pSAVll.setValues(pSAVllA, 1);

		ampl::Parameter pSAVul = ampl.getParameter("SAV_ul");
		double pSAVulA[] = { SAV_ul };
		pSAVul.setValues(pSAVulA, 1);

		ampl::Parameter pPMVll = ampl.getParameter("PMV_ll");
		double pPMVllA[] = { PMV_ll };
		pPMVll.setValues(pPMVllA, 1);

		ampl::Parameter pPMVul = ampl.getParameter("PMV_ul");
		double pPMVulA[] = { PMV_ul };
		pPMVul.setValues(pPMVulA, 1);

		ampl::Parameter pTll = ampl.getParameter("T_ll");
		double pTllA[] = { T_ll };
		pTll.setValues(pTllA, 1);

		ampl::Parameter pTul = ampl.getParameter("T_ul");
		double pTulA[] = { T_ul };
		pTul.setValues(pTulA, 1);

		ampl::Parameter pTSPOTll = ampl.getParameter("T_SPOT_ll");
		double pTSPOTllA[] = { T_SPOT_ll };
		pTSPOTll.setValues(pTSPOTllA, 1);

		ampl::Parameter pTSPOTul = ampl.getParameter("T_SPOT_ul");
		double pTSPOTulA[] = { T_SPOT_ul };
		pTSPOTul.setValues(pTSPOTulA, 1);

		ampl::Parameter pTNoSPOTll = ampl.getParameter("T_NoSPOT_ll");
		double pTNoSPOTllA[] = { T_NoSPOT_ll };
		pTNoSPOTll.setValues(pTNoSPOTllA, 1);

		ampl::Parameter pTNoSPOTul = ampl.getParameter("T_NoSPOT_ul");
		double pTNoSPOTulA[] = { T_NoSPOT_ul };
		pTNoSPOTul.setValues(pTNoSPOTulA, 1);

		// Initialize Parameters - Input Data
		double *pTOutsideA = new double[tinstances];
		for (int i = 0; i < tinstances; i++) {
			pTOutsideA[i] = T_Outside(i, 0);
		}
		ampl::Parameter pTOutside = ampl.getParameter("T_Outside");
		pTOutside.setValues(pTOutsideA, tinstances);

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

		double *pTNoSPOTInitA = new double[total_rooms];
		for (int i = 0; i < total_rooms; i++) {
			pTNoSPOTInitA[i] = TNoSPOTInit(0, i);
		}
		ampl::Parameter pTNoSPOTInit = ampl.getParameter("T_NoSPOT_Init");
		pTNoSPOTInit.setValues(pTNoSPOTInitA, total_rooms);

		double *pDeltaTSPOTInitA = new double[total_rooms];
		for (int i = 0; i < total_rooms; i++) {
			pDeltaTSPOTInitA[i] = DeltaTSPOTInit(0, i);
		}
		ampl::Parameter pDeltaTSPOTInit = ampl.getParameter(
				"Delta_T_SPOT_Init");
		pDeltaTSPOTInit.setValues(pDeltaTSPOTInitA, total_rooms);

		// Resolve and display objective
		ampl.solve();
		ampl::Objective totalcost = ampl.getObjective("total_power");

		// Get final value of Supply Air Temperature (SAT)
		ampl::Variable vSAT = ampl.getVariable("SAT");
		ampl::DataFrame dfSAT = vSAT.getValues();

		cv.SAT_Value = dfSAT.getRowByIndex(0)[1].dbl();
		cv.SAT = Eigen::MatrixXf::Ones(1, total_rooms) * cv.SAT_Value;

		std::cout << "SAT Values Are: " << cv.SAT << std::endl;

		// BUG: Model returns single value of SAV, however it should be for each zone
		// Get final value of Supply Air Volume (SAV)
		ampl::Variable vSAV = ampl.getVariable("SAV");
		ampl::DataFrame dfSAV = vSAV.getValues();

		cv.SAV_Zones = Eigen::MatrixXf::Ones(num_zones, 1)
				* dfSAV.getRowByIndex(0)[1].dbl();
		cv.SAV_Matrix = GetSAVMatrix(cv.SAV_Zones, num_rooms, total_rooms);

		std::cout << "SAV Values Are: " << cv.SAV_Matrix << std::endl;

		// Get final value of SPOT Status
		ampl::Variable vSPOTStatus = ampl.getVariable("SPOT_Status");
		ampl::DataFrame dfSPOTStatus = vSPOTStatus.getValues();

		size_t nRows = dfSPOTStatus.getNumRows();
		cv.SPOT_CurrentState = Eigen::MatrixXi::Ones(1, total_rooms);
		for (size_t i = 0, nCols = cv.SPOT_CurrentState.cols(); i < nCols;
				i++) {
			cv.SPOT_CurrentState(0, i) = dfSPOTStatus.getRowByIndex(
					i * nRows + 0)[2].dbl(); // 2 is the column of returned dataframe
		}

		std::cout << "SPOT Status: " << cv.SPOT_CurrentState << std::endl;
		std::cout << "Objective Is: " << totalcost.value() << std::endl;
	} catch (const std::exception &exc) {
		std::cerr << "Solver Not Working!!";

		cv.SAT_Value = 30;
		cv.SAT = Eigen::MatrixXf::Ones(1, total_rooms) * cv.SAT_Value;

		cv.SAV_Zones = Eigen::MatrixXf::Ones(num_zones, 1) * 0.05f;
		cv.SAV_Matrix = GetSAVMatrix(cv.SAV_Zones, num_rooms, total_rooms);

		cv.SPOT_CurrentState = Eigen::MatrixXi::Ones(1, total_rooms);

		std::cerr << exc.what();
	}

	return cv;
}
