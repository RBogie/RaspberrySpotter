/*
 * Thread.cpp
 *
 *  Created on: Dec 26, 2013
 *      Author: rob
 */

#include "Thread.hpp"

namespace fambogie {

static void* runThread(void* arg) {
	((Thread*) arg)->run();
	return nullptr;
}

Thread::Thread() {
	threadId = 0;
	running = false;
	detached = false;
}

Thread::~Thread() {
	if (running && !detached) {
		pthread_detach(threadId);
	}
	if (running) {
		pthread_cancel(threadId);
	}
}

bool Thread::start() {
	int result = pthread_create(&threadId, NULL, runThread, this);
	if (result == 0) {
		running = true;
	}
	return result == 0;
}

bool Thread::detach() {
	int result = -1;
	if (running && !detached) {
		result = pthread_detach(threadId);
		if (result == 0) {
			detached = true;
		}
	}
	return result == 0;
}

bool Thread::join() {
	int result = -1;
	if (running && !detached) {
		result = pthread_join(threadId, nullptr);
		if (result == 0) {
			detached = true;
		}
	}
	return result == 0;
}

bool Thread::isRunning() {
	return running;
}

pthread_t Thread::self() {
    return threadId;
}

} /* namespace fambogie */
