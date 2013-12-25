/*
 * SpotifyRunner.hpp
 *
 *  Created on: Dec 24, 2013
 *      Author: rob
 */

#ifndef SPOTIFYRUNNER_HPP_
#define SPOTIFYRUNNER_HPP_

#include <pthread.h>
#include <list>
#include "GlobalDefines.hpp"
#include "TaskInformation.hpp"

namespace fambogie {
class SpotifyRunner;
}

#include "SpotifySession.hpp"

namespace fambogie {

class SpotifyRunner {
public:
	SpotifyRunner(sp_session_config& config);
	virtual ~SpotifyRunner();
	void run();
	void stop();
	void notify();
	void addTask(TaskInformation* task);
	void processTasks();
private:
	void* threadMain();
	static void* threadBootStrapper(void* instancePtr);

	pthread_t runnerThread;
	pthread_mutex_t mainLoopConditionMutex;
	pthread_cond_t mainLoopCondition;
	bool running;
	bool threadShouldStop;
	int nextTimeout;

	std::list<TaskInformation*> taskList;

	SpotifySession* spotifySession;

};

} /* namespace fambogie */

#endif /* SPOTIFYRUNNER_HPP_ */
