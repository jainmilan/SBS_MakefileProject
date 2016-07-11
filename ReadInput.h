/*
 * ReadInput.h
 *
 *  Created on: Jul 4, 2016
 *      Author: m26jain
 */

#ifndef READINPUT_H_
#define READINPUT_H_

#include <string>
#include <vector>
#include <istream>

namespace SimpleBuildingSimulator
{
	class ReadCSV {
	public:
		std::vector<std::string> csv_read_row(std::string &line, char delimiter);
		std::vector<std::string> csv_read_row(std::istream &in, char delimiter);
	};
}
#endif /* READINPUT_H_ */
