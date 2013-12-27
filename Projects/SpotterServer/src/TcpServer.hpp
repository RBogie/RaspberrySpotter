/*
 * TcpServer.hpp
 *
 *  Created on: Dec 25, 2013
 *      Author: rob
 */

#ifndef TCPSERVER_HPP_
#define TCPSERVER_HPP_

#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>

namespace fambogie{
class TcpServer;
}
#include "GlobalDefines.hpp"
#include "TcpConnection.hpp"
#include "SpotifyRunner.hpp"

namespace fambogie {

class TcpServer {
public:
	TcpServer(uint16 port, int maxConnections, SpotifyRunner* spotifyRunner);
	void listen();
	virtual ~TcpServer();

private:
	SpotifyRunner* spotifyRunner;

	int maxConnections;
	int numCurrentConnections;
	int serverSocket;

	struct sockaddr_in serverAddress;

	std::vector<TcpConnection*> currentConnections;
};

} /* namespace fambogie */

#endif /* TCPSERVER_HPP_ */
