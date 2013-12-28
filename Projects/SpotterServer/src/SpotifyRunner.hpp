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

namespace fambogie {
class SpotifyRunner;
}
#include "Thread.hpp"
#include "Tasks/Task.hpp"
#include "SpotifySession.hpp"
#include "Mutex.hpp"
#include "CondVar.hpp"

namespace fambogie {

class SpotifyRunner : public Thread{
public:
	SpotifyRunner(sp_session_config& config);
	virtual ~SpotifyRunner();
	void run();
	void stop();
	void notify();
	void addTask(Task* task);
	void processTasks();
private:
	Mutex mainLoopConditionMutex;
	CondVar mainLoopCondition;
	bool threadShouldStop;
	int nextTimeout;

	std::list<Task*> taskList;

	SpotifySession* spotifySession;

};

} /* namespace fambogie */

#endif /* SPOTIFYRUNNER_HPP_ */
