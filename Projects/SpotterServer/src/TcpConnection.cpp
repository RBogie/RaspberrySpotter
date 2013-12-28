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
	sndTimeout.tv_usec = 500 * 1000000;//500 milliseconds
	setsockopt(connectionSocket, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(keepAlive));
	setsockopt(connectionSocket, SOL_SOCKET, SO_SNDTIMEO, &sndTimeout, sizeof(sndTimeout));
}

void TcpConnection::run() {
	int existingSize = 0;
	char* message, messageBuff[RECEIVE_BUFF_SIZE];

	while (!shouldStop) {
		int readSize = recv(connectionSocket, messageBuff, RECEIVE_BUFF_SIZE,
				0);
		if (readSize > 0) {

			char* newMessage = new char[existingSize + readSize];
			if (existingSize > 0) {
				assert(message != nullptr);
				memcpy(newMessage, message, existingSize);
				delete message;
				message = nullptr;
			}
			memcpy(newMessage + existingSize, messageBuff, readSize);

			//Check whether the message has an end
			if (messageBuff[readSize - 1] == '\0') {

				//Handle the string message (convert to Task, and add it to the runner)
				handleMessage(newMessage);

				delete newMessage;
				//Reset for next message
				existingSize = 0;
			} else {
				existingSize += readSize;
				message = newMessage;
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
