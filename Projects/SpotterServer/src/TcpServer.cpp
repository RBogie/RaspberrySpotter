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

bool sendMessage(int clientSocket, const char* message) {
	int32 messageSize = strlen(message);
	int32 networkOrderMessageSize = htonl(messageSize);
	if (send(clientSocket, &networkOrderMessageSize, sizeof(int32), 0) == 4) {
		if (send(clientSocket, message, messageSize, 0) > 0) {
			return true;
		}
	}
	return false;
}

char* receiveMessage(int socket) {
	int32 networkOrderMessageSize;
	int receiveSize = recv(socket, &networkOrderMessageSize, sizeof(int32), 0);
	int32 messageSize = ntohl(networkOrderMessageSize);
	if (receiveSize == sizeof(int32) && messageSize > 0 && messageSize < 8192) {
		char* message = new char[messageSize + 1];
		receiveSize = recv(socket, message, messageSize, 0);
		message[messageSize] = '\0';
		if (receiveSize == messageSize) {
			return message;
		} else {
			delete[] message;
		}
	}
	return nullptr;
}

TcpServer* TcpServer::serverInstance;

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
	serverInstance = this;
	int clientSocket;
	struct sockaddr_in client;
	int c = sizeof(client);
	while ((clientSocket = accept(serverSocket, (struct sockaddr *) &client,
			(socklen_t*) &c))) {
		const char* message = MessageConversion::getHandshakeInitiation();
		bool connectionRefused = false;
		StatusResponse response(false, nullptr);
		if (numCurrentConnections < maxConnections
				&& sendMessage(clientSocket, message)) {
			char* receivedMessage = receiveMessage(clientSocket);
			if (receivedMessage != nullptr) {
				if (MessageConversion::isHandshakeCorrect(receivedMessage)) {
					logDebug("Connection accepted");
					response.setSuccess(true);
					message = MessageConversion::convertResponseToJson(
							&response);
					if (sendMessage(clientSocket, message)) {
						TcpConnection* connection = new TcpConnection(
								clientSocket, this, spotifyRunner);
						connection->start();
						this->currentConnections.push_back(connection);
						numCurrentConnections++;
					}
					delete[] message;
				} else {
					response.setMessage("Handshake incorrect.");
					message = MessageConversion::convertResponseToJson(
							&response);
					sendMessage(clientSocket, message);
					delete[] message;
					connectionRefused = true;
				}
				delete[] receivedMessage;
			} else {
				response.setMessage("Error during receiving of message");
				message = MessageConversion::convertResponseToJson(&response);
				sendMessage(clientSocket, message);
				delete[] message;
				connectionRefused = true;
			}
		} else {
			response.setMessage("Too many connections");
			message = MessageConversion::convertResponseToJson(&response);
			sendMessage(clientSocket, message);
			delete[] message;
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
	while (it != currentConnections.end()) {
		TcpConnection* connection = *it;
		connection->stop();
		delete connection;
		currentConnections.erase(it++);
	}
}

void TcpServer::cleanupClosedConnections() {
	std::vector<TcpConnection*>::iterator it = closedConnections.begin();
	while (it != closedConnections.end()) {
		TcpConnection* connection = *it;
		delete connection;
		closedConnections.erase(it++);
	}
}

void TcpServer::connectionClosed(TcpConnection* connection) {
	std::vector<TcpConnection*>::iterator position = std::find(
			currentConnections.begin(), currentConnections.end(), connection);
	if (position != currentConnections.end()) {
		currentConnections.erase(position);
	}
	closedConnections.push_back(connection);
	numCurrentConnections--;
}

void TcpServer::broadcastMessage(ClientResponse* message) {
	char* json = MessageConversion::convertResponseToJson(message, true);
	if (json != nullptr) {
		broadcastMessage(json);
	}
	delete[] json;
}

void TcpServer::broadcastMessage(const char* json) {
	std::vector<TcpConnection*>::iterator position = currentConnections.begin();
	while (position != currentConnections.end()) {
		(*position)->sendResponse((char*) json, false); //Safe to cast that way, since when given false, sendResponse won't modify the value
		position++;
	}
}

TcpServer* TcpServer::getServerInstance() {
	return serverInstance;
}

} /* namespace fambogie */
