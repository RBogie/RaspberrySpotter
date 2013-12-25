/*
 * ThreadingUtils.hpp
 *
 *  Created on: Dec 24, 2013
 *      Author: rob
 */

#ifndef THREADINGUTILS_HPP_
#define THREADINGUTILS_HPP_

#include "GlobalDefines.hpp"

typedef struct timespec;

namespace fambogie {

timespec getPthreadTimeout(int nanoSeconds);

}

#endif /* THREADINGUTILS_HPP_ */
