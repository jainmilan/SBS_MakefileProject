/*
 * defs.h
 *
 *  Created on: May 30, 2016
 *      Author: m26jain
 */

#ifndef DEFS_H_
#define DEFS_H_

#include <map>
#include <string>
#include <utility>
#include <Eigen/Dense>

struct dtypes{
         time_t i;
         std::string str;
         double d;
         float f;
};

typedef signed short int int8;
typedef unsigned short int  uint8;

typedef signed int int16;
typedef unsigned int uint16;

typedef signed long int int32;
typedef unsigned long int uint32;

typedef std::map<std::pair<time_t, std::string>, dtypes> DataFrame;
typedef std::map<time_t, float> DF_FLOAT;
typedef std::map<time_t, int> DF_INT;
typedef std::map< time_t, std::map<int, int> > DF_INT2;

typedef struct df_format {
			time_t t;
			float weather;
			int *occ;
		} DF_OUTPUT;

typedef Eigen::MatrixXf MAT_FLOAT;
typedef Eigen::MatrixXi MAT_INT;

typedef struct Input {
		struct BParams {
			int num_zones_;			// Number of VAV zones in the building
			int num_rooms_;			// Number of Rooms in the building
		} CommonBuilding;
		struct Room {
			/* Thermal Capacities */
			int C;						// Thermal Capacity of Room (kJ/K)
			int C_;						// Thermal Capacity of SPOT Region (kJ/K)

			/* Heat Transfer Coefficients */
			float alpha_o;				// Heat Transfer Coefficient for Outside (kJ/K.s)
			float alpha_r;				// Heat Transfer Coefficient for Regions (kJ/K.s)

			/* Heat Loads */
			float Q_l;					// Heat Load Due to Lightening and Equipments (kW)
			float Q_h;					// Heat Load Due to Presence of Occupants (kW)
			float Q_s;					// Heat Load of SPOT Unit (kW)

			float fan_coef;				// Coefficient of Fan Power Consumption (-)
		} CommonRoom;

		struct Air {
			float density;				// Density of Air (kg/m3)
			float specific_heat;		// Specific Heat Capacity of Air (J/Kg-K)
		} CommonAir;

		struct PMV_Model {
			float P1;					// 1st Coefficient of Learned PMV Equation (-)
			float P2;					// 2nd Coefficient of Learned PMV Equation (-)
			float P3;					// 3rd Coefficient of Learned PMV Equation (-)
			float P4;					// 4th Coefficient of Learned PMV Equation (-)
		} PMV_Params;

		struct AHU {
			float HeatingEfficiency;	// Heating Efficiency of AHU (-)
			float CoolingEfficiency;	// Cooling Efficiency of AHU (-)
		} CommonAHU;

		struct Errors {
			float err_bparams;			// Error in Building Parameters (-)
			float err_text;				// Error in Weather Conditions (-)
		} CommonErrors;

		struct File_Names {
			std::string weather_file;	// Input File to Read Weather Data (-)
			std::string occupancy_file;	// Input File to Read Occupancy Data (-)
			std::string output_file;	// Output File Created by the Program (-)
		} Files;
	} PARAMS;
#endif /* DEFS_H_ */
