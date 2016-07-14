/*
 * Weather.cpp
 *
 *  Created on: May 30, 2016
 *      Author: m26jain
 */

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE
#endif

#include "Weather.h"
#include "ReadInput.h"

#include <ctime>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <stdlib.h>

using namespace SimpleBuildingSimulator;

/* Constructor */
Weather::Weather() {
}

/* Destructor */
Weather::~Weather() {
}

/* GetWeatherForecast() is a test function that returns constant
 * external temperature of 5 degC for a time range mentioned as the
 * parameters. Input for the function is:
 * 1. duration: For how long we are planning to run the simulations
 * 2. time_step: Time duration between each time instance
 * 3. num_zones: Number of VAV zones in the building
 * 4. num_rooms: Number of rooms in each VAV zone
 * 5. Amount of Error to introduce in each value
 * This function might depricate in future given lack of its requirement
 * */
MAT_FLOAT Weather::GetWeatherForecast(long int duration, int time_step,
		int num_zones, int num_rooms, float err) {
	long int n = (duration / time_step) + 1;	// Total duration of simulation
	int total_rooms = num_zones * num_rooms;// Total number of rooms in the building

	// Matrix of size (n x total_rooms) to assign external temperature equal to 5
	MAT_FLOAT ext_temperature = MAT_FLOAT::Ones(n, total_rooms) * 5;

	// Assign error (randomaly distributed) to each element
	for (size_t i = 0, nRows = ext_temperature.rows(), nCols =
			ext_temperature.cols(); i < nRows; ++i) {
		for (size_t j = 0; j < nCols; ++j) {
			float max_err = err * 100.0;				// Max Error Possible
			float min_err = err * 100.0 * (-1.0);	// Minimum Error Possible
			/* Random Error */
			float rand_err = (float) (rand() % (int) (max_err - min_err + 1.0f)
					+ min_err) / 100.0;
			/* Final Value of External Temperature */
			ext_temperature(i, j) = ext_temperature(i, j)
					+ (ext_temperature(i, j) * rand_err);
		}
	}
	return ext_temperature;		// Return Matrix
}

/*
 * ParseWeatherData: This function parses weather file having two columns i.e. first for datetime
 * and second for external temperature data. It reads the file and returns data in form of a map
 * whose format is custom defined - similar to a dataframe. This function also interpolates the
 * weather data as per sampling rate mentioned in the input file. It uses forward filling for
 * interpolating the weather data.
 * Paramters:
 * 1. Address of Map
 * 2. Name of Input Weather File
 * 3. Start Time of Data Collection - If not specified, data starts from first reading
 * 4. Stop Time of Data Collection - If not specified, data is read till end
 * 5. Sampling Rate
 * 6. Number of Initial Lines to Skip
 */
void Weather::ParseWeatherData(DF_FLOAT& theData, const std::string& filename,
		time_t &start_t, time_t &end_t, const int& time_step,
		const int& skip_lines) {

	ReadCSV csv;											// Call CSV Reader

	std::ifstream in(filename.c_str());					// Pointer to Input File
	if (in.fail()) {
		std::cout << "File not found" << std::endl;
	}  // File Reading - Error Control

	int counter = 0;				// Counter for Number of Significant Line

	struct tm tm = { 0 };// Either initialize or also put seconds in the string

	time_t ptr = start_t;					// Pointer in output file

	time_t current_ftime = 0;				// Current time read from the file
	time_t last_ftime = 0;					// Last time read from the file

	float current_freading = 0.0f;			// Current data read from the file
	float last_freading = 0.0f;				// Last data read from the file

	while (in.good()) {						// While file is valid
		std::vector<std::string> row = csv.csv_read_row(in, ',');// Read CSV row-wise
		if (counter <= skip_lines - 1) {					// Skip header lines
			counter++;
			continue;
		}
		if (row.size() == 2) {						// Validate two-column CSV
			if (strptime(row[0].c_str(), "%Y-%m-%d %H:%M", &tm) == NULL) {// Unmatched datetime format
				std::cout << row[0] << "\n";
				std::cout << "Error" << "\n";
			} else {											// Valid data
				current_ftime = mktime(&tm);
				current_freading = atof(row[1].c_str());
				if (current_ftime > end_t) {// Data doesn't exist in mentioned range
					break;
				} else if (current_ftime < start_t) {// Skip till start time is encountered
					continue;
				} else if ((current_ftime > start_t)
						and (counter == skip_lines)) {
					ptr = current_ftime;
					start_t = current_ftime;
					last_ftime = current_ftime;
					last_freading = current_freading;
					counter++;
				} else {
					while (ptr != current_ftime) {// Till ptr reaches current reading (Interpolation)
						theData[ptr] = last_freading;
						ptr = ptr + time_step;
					}
					last_ftime = current_ftime;
					last_freading = current_freading;
					counter++;
				}
			} // End of If Statement Validating Datetime Format
		} // End of If Statement Validating 2-Columns
	} // End of While Loop

	/*if (ptr < end_t) {						// Update end point
		end_t = ptr;
	}*/
	theData[ptr] = last_freading;			// Include end point
	in.close();

} // End of ParseWeatherData

/* GetWeatherMatrix() converts data parsed from input file to Matrix
 * for easy computations. Input to the function are:
 * 1. df - Output data frame
 * 2. n - Total duration of simulation
 * 3. total_rooms - Total number of rooms in the building
 */
MAT_FLOAT Weather::GetWeatherMatrix(DF_OUTPUT df[], const long int& n,
		const int& total_rooms) {

	// Create Matrix Object
	MAT_FLOAT ext_temperature = MAT_FLOAT::Ones(n, total_rooms);

	// Assign value from the data frame to the Matrix
	for (size_t i = 0; i < (size_t) n; i++) {
		for (size_t j = 0; j < (size_t) total_rooms; j++) {
			ext_temperature(i, j) = df[i].weather;
		}
	}
	return ext_temperature;	// Return Matrix
}
