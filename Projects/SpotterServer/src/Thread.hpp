/*
 * Thread.hpp
 *
 *  Created on: Dec 26, 2013
 *      Author: rob
 */

#ifndef THREAD_HPP_
#define THREAD_HPP_

#include <pthread.h>
#include "GlobalDefines.hpp"
namespace fambogie {

class Thread {
public:
	Thread();
	virtual ~Thread();

	bool start();
	bool join();
	bool detach();
	pthread_t self();

	bool isRunning();

	virtual void run() = 0;

private:
	pthread_t threadId;
	bool running;
	bool detached;
};

} /* namespace fambogie */

#endif /* THREAD_HPP_ */
