/*
 * SpotifyRunner.cpp
 *
 *  Created on: Dec 24, 2013
 *      Author: rob
 */

#include "SpotifyRunner.hpp"

#include <stdio.h>
#include <cerrno>

#include "ThreadingUtils.hpp"

namespace fambogie {

SpotifyRunner::SpotifyRunner(sp_session_config& config) :
		mainLoopConditionMutex(true), mainLoopCondition(mainLoopConditionMutex){
	spotifySession = new SpotifySession(config, this);

	threadShouldStop = false;
	nextTimeout = 0;

	spotifySession->login("USERNAME", "PASSWORD", true);
}

void SpotifyRunner::run() {
	if (mainLoopConditionMutex.lock() != 0) {
			logError("Could not aquire lock: %s %d", __FILE__, __LINE__);
			pthread_exit(0);
		}

		while (!threadShouldStop) {
			int loopTimeout = spotifySession->processEvents();
			if (loopTimeout == 0) {
				mainLoopCondition.wait();
				processTasks();
			} else {
//				logDebug("LoopTimeout: %d", loopTimeout);
				timespec timeout = fambogie::getPthreadTimeout(loopTimeout);
				switch (mainLoopCondition.wait(&timeout)) {
				case 0:
					//Since we got a notification, we try to process everything from the jobqueue.
					processTasks();
					break;
				case EINVAL:
//					logError("Invalid parameters: %p, %p, (sec=%d, nsec=%d)",
//							&mainLoopCondition, &mainLoopConditionMutex,
//							timeout.tv_sec, timeout.tv_nsec);
					break;
				default:
					break;
				}
			}
		}

		if (mainLoopConditionMutex.unlock() != 0) {
			logError("Could not release lock: %s %d", __FILE__, __LINE__);
			pthread_exit(0);
		}
}

void SpotifyRunner::stop() {
	this->threadShouldStop = true;
	this->notify();
}

void SpotifyRunner::addTask(Task* task) {
	if (mainLoopConditionMutex.lock() != 0) {
		logError("Could not aquire lock: %s %d", __FILE__, __LINE__);
		pthread_exit(0);
	}

	taskList.push_back(task);

	this->notify();

	if (mainLoopConditionMutex.unlock() != 0) {
		logError("Could not release lock: %s %d", __FILE__, __LINE__);
		pthread_exit(0);
	}
}

void SpotifyRunner::processTasks() {
	if (mainLoopConditionMutex.lock() != 0) {
		logError("Could not aquire lock: %s %d", __FILE__, __LINE__);
		pthread_exit(0);
	}

	while(taskList.size() > 0) {
		spotifySession->processTask(taskList.front());
		taskList.pop_front();
	}

	if (mainLoopConditionMutex.unlock() != 0) {
		logError("Could not release lock: %s %d", __FILE__, __LINE__);
		pthread_exit(0);
	}
}

void SpotifyRunner::notify() {
	if (mainLoopCondition.signal() != 0) {
		logError("pthread_cond_signal");
		pthread_exit(0);
	}
}

SpotifyRunner::~SpotifyRunner() {
	if (isRunning()) {
		this->stop();
		this->join();
	}

	delete spotifySession;
}

} /* namespace fambogie */
