/*
 * TcpConnection.hpp
 *
 *  Created on: Dec 26, 2013
 *      Author: rob
 */

#ifndef TCPCONNECTION_HPP_
#define TCPCONNECTION_HPP_

#include <sys/socket.h>

#include "GlobalDefines.hpp"

namespace fambogie {
class TcpConnection;
}

#include "Thread.hpp"
#include "SpotifyRunner.hpp"

namespace fambogie {

class TcpConnection : public Thread{
public:
	TcpConnection(int connectionSocket, SpotifyRunner* spotifyRunner);
	virtual ~TcpConnection();

	void run();
	void stop();

	void handleMessage(const char* message);
private:
	int connectionSocket;
	SpotifyRunner* spotifyRunner;
	bool shouldStop;
	bool connectionOpen;
};

} /* namespace fambogie */

#endif /* TCPCONNECTION_HPP_ */