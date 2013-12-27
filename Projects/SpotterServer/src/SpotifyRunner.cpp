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
		mainLoopConditionMutex(PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP), mainLoopCondition(
		PTHREAD_COND_INITIALIZER){
	spotifySession = new SpotifySession(config, this);

	threadShouldStop = false;
	nextTimeout = 0;

	spotifySession->login("USERNAME", "PASSWORD", true);
}

void SpotifyRunner::run() {
	if (pthread_mutex_lock(&mainLoopConditionMutex) != 0) {
			logError("Could not aquire lock: %s %d", __FILE__, __LINE__);
			pthread_exit(0);
		}

		while (!threadShouldStop) {
			int loopTimeout = spotifySession->processEvents();
			if (loopTimeout == 0) {
				pthread_cond_wait(&mainLoopCondition, &mainLoopConditionMutex);
				processTasks();
			} else {
//				logDebug("LoopTimeout: %d", loopTimeout);
				timespec timeout = fambogie::getPthreadTimeout(loopTimeout);
				switch (pthread_cond_timedwait(&mainLoopCondition,
						&mainLoopConditionMutex, &timeout)) {
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

		if (pthread_mutex_unlock(&mainLoopConditionMutex) != 0) {
			logError("Could not release lock: %s %d", __FILE__, __LINE__);
			pthread_exit(0);
		}
}

void SpotifyRunner::stop() {
	this->threadShouldStop = true;
	this->notify();
}

void SpotifyRunner::addTask(Task* task) {
	if (pthread_mutex_lock(&mainLoopConditionMutex) != 0) {
		logError("Could not aquire lock: %s %d", __FILE__, __LINE__);
		pthread_exit(0);
	}

	taskList.push_back(task);

	this->notify();

	if (pthread_mutex_unlock(&mainLoopConditionMutex) != 0) {
		logError("Could not release lock: %s %d", __FILE__, __LINE__);
		pthread_exit(0);
	}
}

void SpotifyRunner::processTasks() {
	if (pthread_mutex_lock(&mainLoopConditionMutex) != 0) {
		logError("Could not aquire lock: %s %d", __FILE__, __LINE__);
		pthread_exit(0);
	}

	while(taskList.size() > 0) {
		spotifySession->processTask(taskList.front());
		taskList.pop_front();
	}

	if (pthread_mutex_unlock(&mainLoopConditionMutex) != 0) {
		logError("Could not release lock: %s %d", __FILE__, __LINE__);
		pthread_exit(0);
	}
}

void SpotifyRunner::notify() {
	if (pthread_cond_signal(&mainLoopCondition) != 0) {
		logError("pthread_cond_signal");
		pthread_exit(0);
	}
}

SpotifyRunner::~SpotifyRunner() {
	if (isRunning()) {
		this->stop();
		this->join();
	}

	pthread_cond_destroy(&mainLoopCondition);
	pthread_mutex_destroy(&mainLoopConditionMutex);

	delete spotifySession;
}

} /* namespace fambogie */
