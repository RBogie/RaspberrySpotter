/*
 * PlayerTask.cpp
 *
 *  Created on: Dec 30, 2013
 *      Author: rob
 */

#include "PlayerTask.hpp"

namespace fambogie {

PlayerTask::PlayerTask() :
		Task(TaskTypePlayer, nullptr) {
	command = PlayerCommandUnknown;
	commandInfo.seekPosition = 0;
}

PlayerTask::~PlayerTask() {
	// TODO Auto-generated destructor stub
}

void PlayerTask::setCommand(PlayerCommand command) {
	this->command = command;
}

void PlayerTask::setCommandInfo(PlayerCommandInfo info) {
	this->commandInfo = info;
}

PlayerCommand PlayerTask::getCommand() {
	return command;
}

PlayerCommandInfo PlayerTask::getCommandInfo() {
	return commandInfo;
}

} /* namespace fambogie */
