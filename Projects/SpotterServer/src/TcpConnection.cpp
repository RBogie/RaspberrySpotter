/*
 * TcpConnection.cpp
 *
 *  Created on: Dec 26, 2013
 *      Author: rob
 */

#include "TcpConnection.hpp"

#include <cstring>

#include "MessageConversion.hpp"

namespace fambogie {

TcpConnection::TcpConnection(int connectionSocket, TcpServer* server,
		SpotifyRunner* spotifyRunner) {
	this->connectionSocket = connectionSocket;
	this->server = server;
	this->spotifyRunner = spotifyRunner;
	this->shouldStop = false;
	this->connectionOpen = true;

	int keepAlive = 1;
	timeval sndTimeout;
	sndTimeout.tv_sec = 0;
	sndTimeout.tv_usec = 500 * 1000000; //500 milliseconds
	setsockopt(connectionSocket, SOL_SOCKET, SO_KEEPALIVE, &keepAlive,
			sizeof(keepAlive));
	setsockopt(connectionSocket, SOL_SOCKET, SO_SNDTIMEO, &sndTimeout,
			sizeof(sndTimeout));
}

void TcpConnection::run() {

	while (!shouldStop) {
		int networkOrderMessageSize;
		int readSize = recv(connectionSocket, &networkOrderMessageSize,
				sizeof(int), 0);
		int messageSize = ntohl(networkOrderMessageSize);
		if (readSize == 4) {
			if (messageSize > 0 && messageSize < 8192) { //We won't handle messages bigger than 8KB
				char* message = new char[messageSize + 1];
				message[messageSize] = '\0'; //Make it a cstring
				readSize = recv(connectionSocket, message, messageSize, 0);
				if (readSize == messageSize) {
					handleMessage(message);
				} else {
					logError("Connection error. Closing connection...");
					shouldStop = true;
					delete[] message;
				}
			} else {
				logError(
						"Received illegal message: Message to big\nClosing connection...");
				shouldStop = true;
			}
		} else if (readSize < 0) {
			logError("Connection error. Closing connection...");
			shouldStop = true;
		} else { //(readSize == 0)
			logDebug("Connection closed");
			shouldStop = true;
		}
	}

	shutdown(connectionSocket, SHUT_RDWR);
	connectionSocket = 0;
	connectionOpen = false;
	shouldStop = false;
	server->connectionClosed(this);
}

void TcpConnection::sendResponse(ClientResponse* response) {
	if (connectionOpen) {
		char* jsonResponse = MessageConversion::convertResponseToJson(response);
		sendResponse(jsonResponse);
	}
	delete response;
}
void TcpConnection::sendResponse(char* response, bool cleanup) {
	if (connectionOpen) {
		int32 messageSize = strlen(response);
		int32 networkOrderMessageSize = htonl(messageSize);
		if (send(connectionSocket, &networkOrderMessageSize, sizeof(int32), 0)
				<= 0) {
			logError("Could not send response!");
		} else {
			if (send(connectionSocket, response, messageSize, 0) <= 0) {
				logError("Could not send response!");
			}
		}
	}
	if (cleanup) {
		delete[] response;
	}
}

void TcpConnection::handleMessage(const char* message) {
	logDebug("Message received: %s", message);
	if (!shouldStop) {
		Task* task = MessageConversion::convertJsonToTask(message);
		if (task != nullptr) {
			task->setClient(this);
			spotifyRunner->addTask(task);
		}
	}
}

void TcpConnection::stop() {
	shouldStop = true;
	this->join();
}

TcpConnection::~TcpConnection() {
	if (this->isRunning()) {
		this->stop();
	}
}

} /* namespace fambogie */
