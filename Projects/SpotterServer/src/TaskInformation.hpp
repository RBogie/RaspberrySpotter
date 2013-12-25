#ifndef TASKINFORMATION_HPP_
#define TASKINFORMATION_HPP_

namespace fambogie {

enum Command {
		ListPlaylists
};

typedef struct TaskInformation {
	Command command;
};

}

#endif/*TASKINFORMATION_HPP_*/
