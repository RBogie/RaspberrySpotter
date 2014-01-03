#ifndef TASKINFORMATION_HPP_
#define TASKINFORMATION_HPP_

#include "../GlobalDefines.hpp"

namespace fambogie {
class Task;
class TcpConnection; //Ugly hack...
}

namespace fambogie {

enum TaskType {
	TaskTypeUnknown,
	TaskTypePlaylist,
	TaskTypePlayer,
};

class Task {
public:
	Task();
	Task(TaskType type, TcpConnection* client);
	virtual ~Task();

	void setType(TaskType type);
	void setClient(TcpConnection* client);

	TaskType getType();
	TcpConnection* getClient();
protected:
	TaskType type;
	TcpConnection* client;
};

}

#endif/*TASKINFORMATION_HPP_*/
