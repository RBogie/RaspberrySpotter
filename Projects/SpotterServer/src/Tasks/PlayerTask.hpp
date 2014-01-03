/*
 * PlayerTask.hpp
 *
 *  Created on: Dec 30, 2013
 *      Author: rob
 */

#ifndef PLAYERTASK_HPP_
#define PLAYERTASK_HPP_

#include "../GlobalDefines.hpp"

#include "Task.hpp"

namespace fambogie {
enum PlayerCommand {
	PlayerCommandUnknown,
	PlayerCommandPlay,
	PlayerCommandPause,
	PlayerCommandSeek,
};

union PlayerCommandInfo {
	int seekPosition;
};

class PlayerTask: public Task {
public:

	PlayerTask();
	virtual ~PlayerTask();

	PlayerCommand getCommand();
	void setCommand(PlayerCommand command);

	PlayerCommandInfo getCommandInfo();
	void setCommandInfo(PlayerCommandInfo info);

private:
	PlayerCommand command;
	PlayerCommandInfo commandInfo;
};

} /* namespace fambogie */

#endif /* PLAYERTASK_HPP_ */
