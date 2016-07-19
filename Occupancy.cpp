/*
 * Occupancy.cpp
 *
 *  Created on: May 30, 2016
 *      Author: m26jain
 */

#ifndef _XOPEN_SOURCE
	#define _XOPEN_SOURCE
#endif

#include "Occupancy.h"
#include "ReadInput.h"

#include <ctime>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <stdlib.h>

using namespace SimpleBuildingSimulator;

Occupants::Occupants()
{
}

Occupants::~Occupants()
{
}

MAT_INT Occupants::GetOccupancyForecast(long int duration, int time_step, int num_zones, int num_rooms) {
	long int n = duration;
	int total_rooms = num_zones * num_rooms;

	MAT_INT occupancy = MAT_INT::Ones(n, total_rooms);

	return occupancy;
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
void Occupants::ParseOccupancyData(DF_INT2& theData, const int& total_rooms, const std::string& filename, time_t &start_t, time_t &end_t,
		const int& time_step, const int& skip_lines) {

	ReadCSV csv;													// Call CSV Reader

	std::ifstream in(filename.c_str());								// Pointer to Input File
	if (in.fail()) { std::cout << "File not found" << std::endl; }  // File Reading - Error Control

    std::string label = "Occupancy";  // Label of Weather Data in the Map

   	int counter = 0;						// Counter for Number of Significant Line

   	struct tm tm = {0};						// Either initialize or also put seconds in the string

    time_t ptr = start_t;					// Pointer in output file

    time_t current_ftime = 0;				// Current time read from the file
    time_t last_ftime = 0;					// Last time read from the file

    float current_freading = 0.0f;			// Current data read from the file
    float last_freading = 0.0f;				// Last data read from the file

    while(in.good()) {						// While file is valid
    	std::vector<std::string> row = csv.csv_read_row(in, ',');				// Read CSV row-wise
    	if (counter <= skip_lines - 1) {										// Skip header lines
    		counter++;
    		continue;
    	}
    	if (row.size() == total_rooms + 1) {									// Validate two-column CSV
    		if (strptime(row[0].c_str(), "%Y-%m-%d %H:%M", &tm) == NULL ) {		// Unmatched datetime format
        		std::cout << row[0] << "\n";
        		std::cout << "Error" << "\n";
        	}
        	else {																// Valid data
        		current_ftime = mktime(&tm);
        		current_freading = atof(row[1].c_str());
        		if (current_ftime > end_t) {									// Data doesn't exist in mentioned range
        			break;
        		} else if (current_ftime < start_t) {							// Skip till start time is encountered
        			continue;
        		} else if ((current_ftime > start_t) and (counter == skip_lines)) {
        			ptr = current_ftime;
        			start_t = current_ftime;
        			last_ftime = current_ftime;
        			last_freading = current_freading;
        			counter++;
        		} else {
        			while (ptr != current_ftime) {									// Till ptr reaches current reading (Interpolation)
        				for (size_t room = 0; room < (size_t) total_rooms; room++) {
        					theData[ptr][room] = last_freading;
        				}
        				ptr = ptr + time_step;
        			}
        			last_ftime = current_ftime;
        			last_freading = current_freading;
        			counter++;
        		}
        	} // End of If Statement Validating Datetime Format
	    } // End of If Statement Validating 2-Columns
    } // End of While Loop

    if (ptr < end_t) {						// Update end point
    	end_t = ptr;
    }
	for (size_t room = 0; room < (size_t) total_rooms; room++) {
		theData[ptr][room] = last_freading;
	}
    in.close();
} // End of ParseWeatherData

/* GetOccupancyMatrix() converts data parsed from input file to Matrix
 * for easy computations. Input to the function are:
 * 1. df - Output data frame
 * 2. n - Total duration of simulation
 * 3. total_rooms - Total number of rooms in the building
 */
MAT_FLOAT Occupants::GetOccupancyMatrix(DF_OUTPUT df[], const long int& n, const int& total_rooms) {

	// Create Matrix Object
	MAT_FLOAT occupancy = MAT_FLOAT::Ones(n, total_rooms);

	// Assign value from the data frame to the Matrix
	for (size_t i = 0; i < (size_t) n; i++) {
		for (size_t j = 0; j < (size_t) total_rooms; j++) {
			occupancy(i, j) = df[i].occ[j];
		}
	}
	return occupancy;	// Return Matrix
}



