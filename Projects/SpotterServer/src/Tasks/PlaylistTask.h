/*
 * PlaylistTask.h
 *
 *  Created on: Dec 27, 2013
 *      Author: rob
 */

#ifndef PLAYLISTTASK_H_
#define PLAYLISTTASK_H_

namespace fambogie {
class PlaylistTask;
}

#include "Task.hpp"

namespace fambogie {

enum Command {
	CommandUnknown,
	CommandList,
};

class PlaylistTask: public Task {
public:
	PlaylistTask();
	PlaylistTask(TcpConnection* client, Command command);
	virtual ~PlaylistTask();

	void setCommand(Command command);
	Command getCommand();

protected:
	Command command;
};

} /* namespace fambogie */

#endif /* PLAYLISTTASK_H_ */
