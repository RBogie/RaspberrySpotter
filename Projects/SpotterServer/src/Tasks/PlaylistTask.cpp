/*
 * PlaylistTask.cpp
 *
 *  Created on: Dec 27, 2013
 *      Author: rob
 */

#include "PlaylistTask.h"

namespace fambogie {

PlaylistTask::PlaylistTask() : Task(TaskTypePlaylist, nullptr){
	this->command = CommandUnknown;
}

PlaylistTask::PlaylistTask(TcpConnection* client, Command command) :
		Task(TaskTypePlaylist, client) {
	this->command = command;
}

PlaylistTask::~PlaylistTask() {
}

void PlaylistTask::setCommand(Command command) {
	this->command = command;
}

Command PlaylistTask::getCommand() {
	return command;
}

void PlaylistTask::setCommandInfo(CommandInfo info) {
	this->info = info;
}

CommandInfo PlaylistTask::getCommandInfo() {
	return info;
}

} /* namespace fambogie */
