/*
 * defs.h
 *
 *  Created on: May 30, 2016
 *      Author: m26jain
 */

#ifndef DEFS_H_
#define DEFS_H_

#include <map>
#include<string>
#include <utility>

struct dtypes{
         int i;
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

typedef struct df_format {
			time_t t;
			float weather;
			int occ;
		} DF_OUTPUT;

#endif /* DEFS_H_ */
