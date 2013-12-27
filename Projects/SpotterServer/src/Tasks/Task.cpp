/*
 * Task.cpp
 *
 *  Created on: Dec 27, 2013
 *      Author: rob
 */

#include "Task.hpp"

namespace fambogie{

Task::Task() {
	client = nullptr;
	type = TaskTypeUnknown;
}

Task::Task(TaskType type, TcpConnection* client) {
	this->client = client;
	this->type = type;
}

Task::~Task() {
	client = nullptr;
}

void Task::setType(TaskType type) {
	this->type = type;
}

void Task::setClient(TcpConnection* client){
	this->client = client;
}

TaskType Task::getType(){
	return this->type;
}

TcpConnection* Task::getClient(){
	return this->client;
}

} /* namspace fambogie */
