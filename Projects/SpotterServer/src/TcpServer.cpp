/*
 * TcpServer.cpp
 *
 *  Created on: Dec 25, 2013
 *      Author: rob
 */

#include "TcpServer.hpp"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <algorithm>

#include "MessageConversion.hpp"

namespace fambogie {

TcpServer::TcpServer(uint16 port, int maxConnections,
		SpotifyRunner* spotifyRunner) {
	this->spotifyRunner = spotifyRunner;
	this->maxConnections = maxConnections;

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(port);

	numCurrentConnections = 0;
	serverSocket = 0;
}

void TcpServer::listen() {
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1) {
		logError("Could not create socket");
		return;
	}

	if (bind(serverSocket, (struct sockaddr *) &serverAddress,
			sizeof(serverAddress)) < 0) {
		logError("Could not bind socket");
		return;
	}

	::listen(serverSocket, 1);
	logDebug("Waiting for incoming connections...");
	int clientSocket;
	struct sockaddr_in client;
	int c = sizeof(client);
	while ((clientSocket = accept(serverSocket, (struct sockaddr *) &client,
			(socklen_t*) &c))) {
		const char* message = MessageConversion::getHandshakeInitiation();
		bool connectionRefused = false;
		if (numCurrentConnections < maxConnections
				&& send(clientSocket, message, strlen(message) + 1, 0) > 0) {
			char messageBuffer[500];
			int received;
			if ((received = recv(clientSocket, &messageBuffer, 500, 0)) > 0) {
				assert(messageBuffer[received - 1] == '\0');
				if (MessageConversion::isHandshakeCorrect(messageBuffer)) {
					logDebug("Connection accepted");
					message = MessageConversion::handshakeStatusToJson(false, nullptr);
					if (send(clientSocket, message, strlen(message) + 1, 0)
							> 0) {
						TcpConnection* connection = new TcpConnection(
								clientSocket, this, spotifyRunner);
						connection->start();
						this->currentConnections.push_back(connection);
						numCurrentConnections++;
					}
					delete message;
				} else {
					message = MessageConversion::handshakeStatusToJson(true, "Handshake incorrect.");
					send(clientSocket, message, strlen(message) + 1, 0);
					delete message;
					connectionRefused = true;
				}
			} else {
				message = MessageConversion::handshakeStatusToJson(true, "Error during receiving of message");
				send(clientSocket, message, strlen(message) + 1, 0);
				delete message;
				connectionRefused = true;
			}
		} else {
			message = MessageConversion::handshakeStatusToJson(true, "Too many connections");
			send(clientSocket, message, strlen(message) + 1, 0);
			delete message;
			connectionRefused = true;
		}

		if (connectionRefused) {
			logDebug("Connection refused");
			shutdown(clientSocket, SHUT_RDWR);
		}
	}

	shutdown(serverSocket, SHUT_RDWR);
}

TcpServer::~TcpServer() {
	this->cleanupClosedConnections();

	std::vector<TcpConnection*>::iterator it = currentConnections.begin();
	while(it != currentConnections.end()) {
		TcpConnection* connection = *it;
		connection->stop();
		delete connection;
		currentConnections.erase(it++);
	}
}

void TcpServer::cleanupClosedConnections() {
	std::vector<TcpConnection*>::iterator it = closedConnections.begin();
	while(it != closedConnections.end()) {
		TcpConnection* connection = *it;
		delete connection;
		closedConnections.erase(it++);
	}
}

void TcpServer::connectionClosed(TcpConnection* connection) {
	std::vector<TcpConnection*>::iterator position = std::find(currentConnections.begin(), currentConnections.end(), connection);
	if (position != currentConnections.end()){
		currentConnections.erase(position);
	}
	closedConnections.push_back(connection);
	numCurrentConnections--;
}

} /* namespace fambogie */
