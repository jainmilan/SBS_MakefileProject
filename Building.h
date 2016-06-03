/*
 * Building.h
 *
 *  Created on: May 30, 2016
 *      Author: m26jain
 */

#ifndef BUILDING_H_
#define BUILDING_H_

#include<stdio.h>
#include<iostream>

#include "defs.h"
#include "Weather.h"
#include "ControlBox.h"
#include "Occupancy.h"

#include<Eigen/Dense>

namespace SimpleBuildingSimulator
{
	class Building
	{
	public:
		Building();
		~Building();
		struct Room
		{
			/* Thermal Capacities */
			uint16 C = 2000;			// Thermal Capacity of Room (kJ/K)
			uint16 C_ = 200;			// Thermal Capacity of SPOT Region (kJ/K)

										/* Heat Transfer Coefficients */
			float alpha_o = 0.048f;		// Heat Transfer Coefficient for Outside (kJ/K.s)
			float alpha_r = 0.1425f;	// Heat Transfer Coefficient for Regions (kJ/K.s)

										/* Heat Loads */
			float Q_l = 0.1f;			// Heat Load Due to Lightening and Equipments (kW)
			float Q_h = 0.1f;			// Heat Load Due to Presence of Occupants (kW)
			float Q_s = 0.7f;			// Heat Load of SPOT Unit (kW)
		};

		struct Air
		{
			float density = 1.225f;
			float specific_heat = 1.003f;
		};

		uint8 get_num_zones() { return num_zones_; }
		uint8 get_num_rooms() { return num_rooms_; }

		void set_num_zones(uint8 num_zones) { num_zones_ = num_zones; }
		void set_num_rooms(uint8 num_rooms) { num_rooms_ = num_rooms; }

		void Simulate(uint32 duration, uint16 time_step, int control_type);
	private:
		uint8 num_zones_ = 1;			// Number of VAV zones in the building
		uint8 num_rooms_ = 1;			// Number of rooms in the building
		bool region = 0;				// If True implies SPOT is ON else OFF

		Eigen::MatrixXf Create_CoWI_CRT_Matrix(uint16 time_step);
		Eigen::MatrixXf Create_CoWI_OAT_Matrix(uint16 time_step);

		Eigen::MatrixXf Create_CoHI_CRT_Matrix(uint16 time_step);
		Eigen::MatrixXf Create_CoHI_SAT_Matrix(uint16 time_step);

		Eigen::MatrixXf Create_CoEI_OLEL_Matrix(uint16 time_step);
		Eigen::MatrixXf Create_CoOI_OHL_Matrix(uint16 time_step);

		Eigen::MatrixXf Create_CoSI_SCS_Matrix(uint16 time_step);

		Eigen::MatrixXf Create_CoRC_CiRT_Matrix(uint16 time_step);
		Eigen::MatrixXf Create_CoRC_CiR1T_Matrix(uint16 time_step);

		float GetMixedAirTemperature(Eigen::MatrixXf TR1, Eigen::MatrixXf T_ext);
		float GetAHUPower(float MixedAirTemperature, Eigen::MatrixXf SPOT_CurrentState, float SAT_Value, Eigen::MatrixXf SAV_Zones);
	};
} // namespace SimpleBuildingSimulator

#endif /* BUILDING_H_ */
