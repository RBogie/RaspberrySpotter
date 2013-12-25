/*
 * GlobalDefines.hpp
 *
 *  Created on: Nov 22, 2013
 *      Author: rob
 */

#ifndef GLOBALDEFINES_HPP_
#define GLOBALDEFINES_HPP_

#include <assert.h>

#define DEBUG 1

#define nullptr NULL

typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

typedef short int16;
typedef int int32;
typedef long long int64;

typedef float float32;
typedef double float64;
typedef long double float96;

void logError(const char *format, ...);

void logDebug(const char *format, ...);

#endif /* GLOBALDEFINES_HPP_ */
