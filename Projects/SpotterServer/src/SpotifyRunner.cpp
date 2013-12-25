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

	runnerThread = 0;
	running = false;
	threadShouldStop = false;
	nextTimeout = 0;

	spotifySession->login("USERNAME", "PASSWORD", true);
}

void SpotifyRunner::run() {
	if (pthread_create(&this->runnerThread, nullptr, threadBootStrapper,
			static_cast<void*>(this)) != 0) {
		logError("Could not create Spotify Runner Thread");
	} else {
		running = true;
	}
}

void SpotifyRunner::stop() {
	this->threadShouldStop = true;
	this->notify();
}

void* SpotifyRunner::threadMain() {
	if (pthread_mutex_lock(&mainLoopConditionMutex) != 0) {
		logError("Could not aquire lock: %s %d", __FILE__, __LINE__);
		pthread_exit(0);
	}

	while (!threadShouldStop) {
		int loopTimeout = 100;spotifySession->processEvents();
		if (loopTimeout == 0) {
			pthread_cond_wait(&mainLoopCondition, &mainLoopConditionMutex);
			processTasks();
		} else {
			//logDebug("LoopTimeout: %d", loopTimeout);
			timespec timeout = fambogie::getPthreadTimeout(loopTimeout);
			switch (pthread_cond_timedwait(&mainLoopCondition,
					&mainLoopConditionMutex, &timeout)) {
			case 0:
				//Since we got a notification, we try to process everything from the jobqueue.
				processTasks();
				break;
			case ETIMEDOUT:
				break;
			case EINVAL:
//				logError("Invalid parameters: %p, %p, (sec=%d, nsec=%d)",
//						&mainLoopCondition, &mainLoopConditionMutex,
//						timeout.tv_sec, timeout.tv_nsec);
				break;
			default:
				//logError("pthread_cond_timedwait");
				//pthread_exit(0);
				break;
			}
		}
	}

	if (pthread_mutex_unlock(&mainLoopConditionMutex) != 0) {
		logError("Could not release lock: %s %d", __FILE__, __LINE__);
		pthread_exit(0);
	}

	return nullptr;
}

void SpotifyRunner::addTask(TaskInformation* task) {
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
	if (pthread_mutex_lock(&mainLoopConditionMutex) != 0) {
		logError("Could not aquire lock: %s %d", __FILE__, __LINE__);
		pthread_exit(0);
	}

	if (pthread_cond_signal(&mainLoopCondition) != 0) {
		logError("pthread_cond_signal");
		pthread_exit(0);
	}

	if (pthread_mutex_unlock(&mainLoopConditionMutex) != 0) {
		logError("Could not release lock: %s %d", __FILE__, __LINE__);
		pthread_exit(0);
	}
}

void* SpotifyRunner::threadBootStrapper(void* instancePtr) {
	return static_cast<SpotifyRunner*>(instancePtr)->threadMain();
}

SpotifyRunner::~SpotifyRunner() {
	if (running) {
		this->stop();
		void* status;
		pthread_join(runnerThread, &status);
	}

	pthread_cond_destroy(&mainLoopCondition);
	pthread_mutex_destroy(&mainLoopConditionMutex);

	delete spotifySession;
}

} /* namespace fambogie */
