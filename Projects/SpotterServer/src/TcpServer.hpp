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

	void cleanupClosedConnections();
	void connectionClosed(TcpConnection* connection);
	void broadcastMessage(ClientResponse* message);
	void broadcastMessage(const char* json);

	static TcpServer* getServerInstance();
private:
	SpotifyRunner* spotifyRunner;

	int maxConnections;
	int numCurrentConnections;
	int serverSocket;

	struct sockaddr_in serverAddress;

	std::vector<TcpConnection*> currentConnections;
	std::vector<TcpConnection*> closedConnections; //Can be deleted after some time

	static TcpServer* serverInstance;
};

} /* namespace fambogie */

#endif /* TCPSERVER_HPP_ */
