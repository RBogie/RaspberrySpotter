/*
 * ThreadingUtils.cpp
 *
 *  Created on: Dec 24, 2013
 *      Author: rob
 */

#include "ThreadingUtils.hpp"

#include <sys/time.h>
#include <unistd.h>

namespace fambogie {

struct timespec getPthreadTimeout(int nanosec) {
	timeval now;
	struct timespec timeout;

	gettimeofday(&now, nullptr);
	TIMEVAL_TO_TIMESPEC(&now, &timeout);
	timeout.tv_sec += (nanosec / 1000);
	timeout.tv_nsec += (nanosec % 1000) * 1000000;
	return timeout;
}

}
