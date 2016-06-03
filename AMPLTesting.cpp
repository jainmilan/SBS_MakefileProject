//============================================================================
// Name        : AMPLTesting.cpp
// Author      : Milan Jain
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
//============================================================================

#include "ampl/ampl.h"

#include <iostream>
#include <Eigen/Dense>

int main(int argc, char **argv) {
	ampl::AMPL ampl;

	double num_zones = 1;
	double num_rooms = 1;

	// Building Parameters
	double C = 2000;
	double C_= 200;

	double alpha_o = 0.048;
	double alpha_r = 0.1425;

	double Q_l = 0.1;
	double Q_h = 0.1;
	double Q_s = 0.7;

	double density = 1.225;
	double specific_heat = 1.003;

	int tau = 600;
	int duration = 14400;

	int tinstances = duration/tau;
	int total_rooms = num_zones * num_rooms;

	// Read the model and data files.
	std::string modelDirectory = "remodel";
	ampl.read(modelDirectory + "/sbs.mod");

	// Initialize Parameters - Basic Parameters
	ampl::Parameter pDuration = ampl.getParameter("duration");
	double pDurationA[] = { tinstances };
	pDuration.setValues(pDurationA, 1);

	ampl::Parameter pTotalRooms = ampl.getParameter("total_rooms");
	double pTotalRoomsv = total_rooms;
	double pTotalRoomsA[] = { pTotalRoomsv };
	pTotalRooms.setValues(pTotalRoomsA, 1);

	// Initialize Parameters -Parameters for Objective Function
	ampl::Parameter pCoHP = ampl.getParameter("Coefficient_Heating_Power");
	double pCoHPv = (density * specific_heat) / 0.9;
	double pCoHPA[] = { pCoHPv };
	pCoHP.setValues(pCoHPA, 1);

	ampl::Parameter pCoCP = ampl.getParameter("Coefficient_Cooling_Power");
	double pCoCPv = (density * specific_heat) / 0.9;
	double pCoCPA[] = { pCoCPv };
	pCoCP.setValues(pCoCPA, 1);

	ampl::Parameter pCoFP = ampl.getParameter("Coefficient_Fan_Power");
	double pCoFPA[] = { 0.094 };
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
	double pCoHICRTv = (-1 * tau * density * specific_heat)/(num_rooms * C);
	double pCoHICRTA[] = { pCoHICRTv };
	pCoHICRT.setValues(pCoHICRTA, 1);

	ampl::Parameter pCoHISAT = ampl.getParameter("CoHI_SAT");
	double pCoHISATv = (tau * density * specific_heat)/(num_rooms * C);
	double pCoHISATA[] = {pCoHISATv};
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

	// Initialize Parameters - Limits on Variables
	ampl::Parameter pSATll = ampl.getParameter("SAT_ll");
	double pSATllA[] = { -20 };
	pSATll.setValues(pSATllA, 1);

	ampl::Parameter pSATul = ampl.getParameter("SAT_ul");
	double pSATulA[] = { 20 };
	pSATul.setValues(pSATulA, 1);

	ampl::Parameter pSAVll = ampl.getParameter("SAV_ll");
	double pSAVllA[] = { 0 };
	pSAVll.setValues(pSAVllA, 1);

	ampl::Parameter pSAVul = ampl.getParameter("SAV_ul");
	double pSAVulA[] = { 1 };
	pSAVul.setValues(pSAVulA, 1);

	ampl::Parameter pPMVll = ampl.getParameter("PMV_ll");
	double pPMVllA[] = { -5 };
	pPMVll.setValues(pPMVllA, 1);

	ampl::Parameter pPMVul = ampl.getParameter("PMV_ul");
	double pPMVulA[] = { 5 };
	pPMVul.setValues(pPMVulA, 1);

	ampl::Parameter pTll = ampl.getParameter("T_ll");
	double pTllA[] = { -20 };
	pTll.setValues(pTllA, 1);

	ampl::Parameter pTul = ampl.getParameter("T_ul");
	double pTulA[] = { 20 };
	pTul.setValues(pTulA, 1);

	ampl::Parameter pTSPOTll = ampl.getParameter("T_SPOT_ll");
	double pTSPOTllA[] = { -20 };
	pTSPOTll.setValues(pTSPOTllA, 1);

	ampl::Parameter pTSPOTul = ampl.getParameter("T_SPOT_ul");
	double pTSPOTulA[] = { 20 };
	pTSPOTul.setValues(pTSPOTulA, 1);

	ampl::Parameter pTNoSPOTll = ampl.getParameter("T_NoSPOT_ll");
	double pTNoSPOTllA[] = { -20 };
	pTNoSPOTll.setValues(pTNoSPOTllA, 1);

	ampl::Parameter pTNoSPOTul = ampl.getParameter("T_NoSPOT_ul");
	double pTNoSPOTulA[] = { 20 };
	pTNoSPOTul.setValues(pTNoSPOTulA, 1);

	// Initialize Parameters - Input Data
	Eigen::MatrixXf T_Outside = Eigen::MatrixXf::Ones(total_rooms, tinstances) * 5;
	double pTOutsideA[tinstances];
	for(int i=0; i<tinstances; i++){
		pTOutsideA[i] = T_Outside.row(0)(i);
	}
	ampl::Parameter pTOutside = ampl.getParameter("T_Outside");
	pTOutside.setValues(pTOutsideA, tinstances);

	Eigen::MatrixXf Occupancy = Eigen::MatrixXf::Ones(total_rooms, tinstances);
	double pOccupancyA[total_rooms * tinstances];
	double pOccupancyRows[total_rooms];
	double pOccupancyCols[tinstances];

	for(int i=0; i < total_rooms; i++){
		pOccupancyRows[i] = i + 1;
		for (int j=0; j < tinstances; j++){
			pOccupancyA[i * tinstances + j] = Occupancy(i, j);
		}
	}
	for(int i=0; i < tinstances; i++){
		pOccupancyCols[i] = i + 1;
	}
	ampl::Parameter pOccupancy = ampl.getParameter("Occupancy");
	pOccupancy.setValues(total_rooms, pOccupancyRows, tinstances, pOccupancyCols,
			pOccupancyA, false);

	Eigen::MatrixXf TNoSPOTInit = Eigen::MatrixXf::Ones(total_rooms, tinstances) * 21;
	double pTNoSPOTInitA[(int)total_rooms];
	for(int i=0; i<total_rooms; i++){
		pTNoSPOTInitA[i] = TNoSPOTInit(i, 0);
	}
	ampl::Parameter pTNoSPOTInit = ampl.getParameter("T_NoSPOT_Init");
	pTNoSPOTInit.setValues(pTNoSPOTInitA, total_rooms);

	Eigen::MatrixXf DeltaTSPOTInit = Eigen::MatrixXf::Zero(total_rooms, tinstances);
	double pDeltaTSPOTInitA[total_rooms];
	for(int i=0; i<total_rooms; i++){
		pDeltaTSPOTInitA[i] = DeltaTSPOTInit(i, 0);
	}
	ampl::Parameter pDeltaTSPOTInit = ampl.getParameter("Delta_T_SPOT_Init");
	pDeltaTSPOTInit.setValues(pDeltaTSPOTInitA, total_rooms);

	// Resolve and display objective
	ampl.solve();
	ampl::Objective totalcost = ampl.getObjective("total_cost");

	std::cout << "Objective Is: " << totalcost.value() << std::endl;
}
