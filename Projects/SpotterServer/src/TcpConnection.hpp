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
#include "TcpServer.hpp"
#include "SpotifyRunner.hpp"
#include "Responses/ClientResponse.hpp"

namespace fambogie {

class TcpConnection : public Thread{
public:
	TcpConnection(int connectionSocket, TcpServer* server, SpotifyRunner* spotifyRunner);
	virtual ~TcpConnection();

	void run();
	void stop();

	void sendResponse(ClientResponse* response);
	void sendResponse(char* response);
	void handleMessage(const char* message);
private:
	int connectionSocket;
	TcpServer* server;
	SpotifyRunner* spotifyRunner;
	bool shouldStop;
	bool connectionOpen;
};

} /* namespace fambogie */

#endif /* TCPCONNECTION_HPP_ */
