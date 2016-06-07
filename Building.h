/*
 * Building.h
 *
 *  Created on: May 30, 2016
 *      Author: m26jain
 */

#ifndef BUILDING_H_
#define BUILDING_H_

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
			int C;				// Thermal Capacity of Room (kJ/K)
			int C_;				// Thermal Capacity of SPOT Region (kJ/K)

			/* Heat Transfer Coefficients */
			float alpha_o;		// Heat Transfer Coefficient for Outside (kJ/K.s)
			float alpha_r;		// Heat Transfer Coefficient for Regions (kJ/K.s)

			/* Heat Loads */
			float Q_l;			// Heat Load Due to Lightening and Equipments (kW)
			float Q_h;			// Heat Load Due to Presence of Occupants (kW)
			float Q_s;			// Heat Load of SPOT Unit (kW)

			float fan_coef;
		} CommonRoom;

		struct Air
		{
			float density;
			float specific_heat;
		} CommonAir;

		struct PMV_Model {
			float P1;
			float P2;
			float P3;
			float P4;
		} PMV_Params;

		struct AHU {
			float HeatingEfficiency;
			float CoolingEfficiency;
		} CommonAHU;

		int get_num_zones() { return num_zones_; }
		int get_num_rooms() { return num_rooms_; }

		void set_num_zones(int num_zones) { num_zones_ = num_zones; }
		void set_num_rooms(int num_rooms) { num_rooms_ = num_rooms; }

		void Simulate(long int duration, int time_step, int control_type);
	private:
		int num_zones_;			// Number of VAV zones in the building
		int num_rooms_;			// Number of rooms in the building
		bool region;				// If True implies SPOT is ON else OFF

		Eigen::MatrixXf Create_CoWI_CRT_Matrix(int time_step);
		Eigen::MatrixXf Create_CoWI_OAT_Matrix(int time_step);

		Eigen::MatrixXf Create_CoHI_CRT_Matrix(int time_step);
		Eigen::MatrixXf Create_CoHI_SAT_Matrix(int time_step);

		Eigen::MatrixXf Create_CoEI_OLEL_Matrix(int time_step);
		Eigen::MatrixXf Create_CoOI_OHL_Matrix(int time_step);

		Eigen::MatrixXf Create_CoSI_SCS_Matrix(int time_step);

		Eigen::MatrixXf Create_CoRC_CiRT_Matrix(int time_step);
		Eigen::MatrixXf Create_CoRC_CiR1T_Matrix(int time_step);

		float GetMixedAirTemperature(Eigen::MatrixXf TR1, Eigen::MatrixXf T_ext);
		float GetAHUPower(float MixedAirTemperature, Eigen::MatrixXf SPOT_CurrentState, float SAT_Value, Eigen::MatrixXf SAV_Zones);
	};
} // namespace SimpleBuildingSimulator

#endif /* BUILDING_H_ */
