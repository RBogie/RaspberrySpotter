#include <iostream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <cstdlib>
#include <unistd.h>

using namespace std;

const char* expectedHandshakeMessage = "{"
		"\"ServerName\": \"SpotterServer\","
		"\"ProtocolVersion\": 1"
		"}";

const char* handshakeResponse = "{"
		"\"ClientName\": \"TestClient\","
		"\"ProtocolVersion\": 1"
		"}";

const char* expectedHandshakeStatus =
		"{\"Type\":\"Status\",\"TypeSpecific\":{\"Status\":\"Ok\"}}";

const char* listPlaylistsRequest = "{"
		"\"Type\": \"Playlist\","
		"\"TypeSpecific\": {"
		"\"Command\": \"List\","
		"\"CommandInfo\": ["
		"\"Name\",\"NumTracks\""
		"]"
		"}"
		"}";

const char* playPlaylistsRequest =
		"{\"Type\":\"Playlist\",\"TypeSpecific\":{\"Command\":\"PlayPlaylist\",\"PlaylistId\":20}}";
const char* playerPauseRequest =
		"{\"Type\":\"Player\",\"TypeSpecific\":{\"Command\":\"Pause\"}}";
const char* playerPlayRequest =
		"{\"Type\":\"Player\",\"TypeSpecific\":{\"Command\":\"Play\"}}";
const char* playerSeekRequest =
		"{\"Type\":\"Player\",\"TypeSpecific\":{\"Command\":\"Seek\", \"SeekPosition\":30000}}";

char* receiveMessage(int socket) {
	int networkOrderMessageSize;
	int receiveSize = recv(socket, &networkOrderMessageSize, sizeof(int), 0);
	int messageSize = ntohl(networkOrderMessageSize);
	if (receiveSize == sizeof(int) && messageSize > 0
			&& messageSize < 8192) {
		char* message = new char[messageSize + 1];
		receiveSize = recv(socket, message, messageSize, 0);
		message[messageSize] = '\0';
		if (receiveSize == messageSize) {
			return message;
		} else {
			delete[] message;
		}
	}
	return NULL;
}

bool sendMessage(int socket, const char* data) {
	int messageSize = strlen(data);
	int networkOrderMessageSize = htonl(messageSize);
	if (send(socket, &networkOrderMessageSize, sizeof(int), 0) == 4) {
		if (send(socket, data, messageSize, 0) > 0) {
			return true;
		}
	}
	return false;
}

int main(int argc, const char* argv[]) {
	if (argc < 3) {
		cout << "Not enough arguments. Usage: ./TestClient [ip] [port]" << endl;
		return -1;
	}
	int sock;

	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		cout << "Could not create socket!" << endl;
		return -1;
	}

	struct sockaddr_in server;
	memset(&server, 0, sizeof(server)); /* Clear struct */
	server.sin_family = AF_INET; /* Internet/IP */
	server.sin_addr.s_addr = inet_addr(argv[1]); /* IP address */
	server.sin_port = htons(atoi(argv[2])); /* server port */

	if (connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
		cerr << "Failed to connect with server" << endl;
		return -1;
	}

	char* message = receiveMessage(sock);
	if (message != NULL) {
		if (strcmp(message, expectedHandshakeMessage) == 0) {
			if (!sendMessage(sock, handshakeResponse)) {
				delete[] message;
				return -1;
				cerr << "Error during sending" << endl;
			}
		} else {
			cerr << "Received handshake is incorrect" << endl;
		}
		delete[] message;
		message = NULL;
	} else {
		cerr << "Error during receive" << endl;
		delete[] message;
		return -1;
	}

	message = receiveMessage(sock);
	if (message != NULL) {
		if (strcmp(message, expectedHandshakeStatus) == 0) {
			cout << "Handshake Complete" << endl;
		} else {
			cerr << "Handshake failed" << endl;
			delete[] message;
			return -1;
		}
		delete[] message;
		message = NULL;
	} else {
		cerr << "Error during receive" << endl;
	}

	sendMessage(sock, listPlaylistsRequest);

	message = receiveMessage(sock);
	if (message != NULL) {
		cout << message << endl;
	}

	sendMessage(sock, playPlaylistsRequest);

	message = receiveMessage(sock);
	if (message != NULL) {
		cout << message << endl;
	}

//	sleep(5);
//
//	send(sock, playerPauseRequest, strlen(playerPauseRequest) + 1, 0);
//	receiveSize = recv(sock, messageBuff, 5000, 0);
//	if (receiveSize > 0) {
//		cout << messageBuff << endl;
//	}
//	sleep(5);
//
//	send(sock, playerPlayRequest, strlen(playerPlayRequest) + 1, 0);
//	receiveSize = recv(sock, messageBuff, 5000, 0);
//	if (receiveSize > 0) {
//		cout << messageBuff << endl;
//	}
//	sleep(5);
//
//	send(sock, playerSeekRequest, strlen(playerSeekRequest) + 1, 0);
//	receiveSize = recv(sock, messageBuff, 5000, 0);
//	if (receiveSize > 0) {
//		cout << messageBuff << endl;
//	}

	shutdown(sock, SHUT_RDWR);
	return 0;
}
