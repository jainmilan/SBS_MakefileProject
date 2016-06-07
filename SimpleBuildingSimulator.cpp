/*
 * SimpleBuildingSimulator.cpp
 *
 *  Created on: May 30, 2016
 *      Author: m26jain
 */

#include<string>
#include<stdio.h>
#include<iostream>

#include "defs.h"
#include "Building.h"

using namespace SimpleBuildingSimulator;
using namespace Eigen;

int main()
{
	/* Initialize Building */
	Building dc;

	int num_zones = dc.get_num_zones();
	int num_rooms = dc.get_num_rooms();

	int update = 0;

	printf("DEFAULT: \n Number of Zones = %d \n Number of Rooms = %d \n", num_zones, num_rooms);

	printf("Do you want to change? (1-Yes, 0-No): ");
	scanf("%d", &update);

	if (update == 1) {
		printf("WARNING: Number should lie between 0-255. Otherwise, Unexpected Results\n");

		printf("Enter Number of Zones : ");
		scanf("%d", &num_zones);
		dc.set_num_zones(num_zones);

		printf("Enter Number of Rooms : ");
		scanf("%d", &num_rooms);
		dc.set_num_rooms(num_rooms);
	}

	num_zones = dc.get_num_zones();
	num_rooms = dc.get_num_rooms();

	printf("UPDATED: \n Number of Zones = %d \n Number of Rooms = %d \n", num_zones, num_rooms);

	/* Initialize Simulation Details */
	int MIN2SEC = 60;
	long int duration = 120000;
	int time_step = 600;

	printf("DEFAULT: \n Duration = %ld \n Time Step = %d \n", duration, time_step);

	printf("Do you want to change? (1-Yes, 0-No): ");
	scanf("%d", &update);

	if (update == 1) {
		printf("Enter Simulation Duration (in seconds) : ");
		scanf("%ld", &duration);

		printf("Enter Time Step (in seconds) : ");
		scanf("%d", &time_step);
	}

	if ((time_step/MIN2SEC)/duration > 1) {
		printf("ERROR: Time step can't be greater than total duration of simulation");
		return 0;
	}

	printf("UPDATED: \n Duration = %ld \n Time Step = %d \n", duration, time_step);

	int control_type = 1;

	printf("DEFAULT: SPOT Always On \n");

	printf("Do you want to change? (1-Yes, 0-No): ");
	scanf("%d", &update);

	if (update == 1) {
		printf("Enter Control Type (1 - AlwaysOn, 2 - Reactive, 3 - MPC) : ");
		scanf("%d", &control_type);
	}

	if (control_type == 1) {
		printf("Updated: SPOT Always On \n");
	}
	else if (control_type == 1) {
		printf("Updated: Reactive Control \n");
	}
	else {
		printf("Updated: MPC Control \n");
	}

	/*
	Eigen::MatrixXf m1 = dc.Create_CoWI_CRT_Matrix(time_step);
	std::cout << m1 << std::endl;
	printf("\n");

	Eigen::MatrixXf m2 = dc.Create_CoWI_OAT_Matrix(time_step);
	std::cout << m2 << std::endl;
	printf("\n");

	Eigen::MatrixXf m3 = dc.Create_CoHI_CRT_Matrix(time_step);
	std::cout << m3 << std::endl;
	printf("\n");

	Eigen::MatrixXf m4 = dc.Create_CoHI_SAT_Matrix(time_step);
	std::cout << m4 << std::endl;
	printf("\n");

	Eigen::MatrixXf m5 = dc.Create_CoEI_OLEL_Matrix(time_step);
	std::cout << m5 << std::endl;
	printf("\n");

	Eigen::MatrixXf m6 = dc.Create_CoOI_OHL_Matrix(time_step);
	std::cout << m6 << std::endl;
	printf("\n");

	Eigen::MatrixXf m7 = dc.Create_CoSI_SCS_Matrix(time_step);
	std::cout << m7 << std::endl;
	printf("\n");

	Eigen::MatrixXf m8 = dc.Create_CoRC_CiRT_Matrix(time_step);
	std::cout << m8 << std::endl;
	printf("\n");

	Eigen::MatrixXf m9 = dc.Create_CoRC_CiR1T_Matrix(time_step);
	std::cout << m9 << std::endl;
	printf("\n");

	Weather forecast;
	Eigen::MatrixXf m10 = forecast.GetWeatherForecast(duration, time_step, num_zones, num_rooms);
	std::cout << m10 << std::endl;
	printf("\n");

	Occupants occupancy;
	Eigen::MatrixXi m11 = occupancy.GetOccupancyForecast(duration, time_step, num_zones, num_rooms);
	std::cout << m11 << std::endl;
	printf("\n");

	SPOT spot;
	Eigen::MatrixXi m12 = spot.GetSPOTForecast(duration, time_step, num_zones, num_rooms);
	std::cout << m12 << std::endl;
	printf("\n");
	*/

	dc.Simulate(duration, time_step, control_type);


	system("pause");
	return 0;
}
