/*
 * IntroduceError.h
 *
 *  Created on: Jul 26, 2016
 *      Author: m26jain
 */

#ifndef INTRODUCEERROR_H_
#define INTRODUCEERROR_H_

#include "defs.h"

PARAMS ErrorInParams(PARAMS& ParamsIn, float error);
MAT_FLOAT ErrorInWeather(MAT_FLOAT T_ext, float error);

#endif /* INTRODUCEERROR_H_ */
