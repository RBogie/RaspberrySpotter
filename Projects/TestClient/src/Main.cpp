#include <iostream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <cstdlib>
#include <unistd.h>

using namespace std;

const char* expectedHandshakeMessage = "{"
		"ServerName\": \"SpotterServer\","
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
		"{\"Type\":\"Playlist\",\"TypeSpecific\":{\"Command\":\"PlayPlaylist\",\"PlaylistId\":14}}";
const char* playerPauseRequest =
		"{\"Type\":\"Player\",\"TypeSpecific\":{\"Command\":\"Pause\"}}";
const char* playerPlayRequest =
		"{\"Type\":\"Player\",\"TypeSpecific\":{\"Command\":\"Play\"}}";
const char* playerSeekRequest =
		"{\"Type\":\"Player\",\"TypeSpecific\":{\"Command\":\"Seek\", \"SeekPosition\":30000}}";

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
		cout << "Failed to connect with server" << endl;
		return -1;
	}

	bool error = false;

	char messageBuff[5000];
	int receiveSize = recv(sock, messageBuff, 5000, 0);
	if (receiveSize > 0) {
		if (strcmp(messageBuff, expectedHandshakeMessage) == 0) {
			send(sock, handshakeResponse, strlen(handshakeResponse) + 1, 0);
		} else {
			cout << "Received handshake is incorrect" << endl;
		}
	} else {
		cout << "Error during receive" << endl;
	}

	if (error) {
		return -1;
	}

	receiveSize = recv(sock, messageBuff, 5000, 0);
	if (receiveSize > 0) {
		if (strcmp(messageBuff, expectedHandshakeStatus) == 0) {
			cout << "Handshake Complete" << endl;
		} else {
			cout << "Handshake failed" << endl;
			error = true;
		}
	} else {
		cout << "Error during receive" << endl;
		error = true;
	}

	send(sock, listPlaylistsRequest, strlen(listPlaylistsRequest) + 1, 0);

	receiveSize = recv(sock, messageBuff, 5000, 0);
	if (receiveSize > 0) {
		cout << messageBuff << endl;
	}

	send(sock, playPlaylistsRequest, strlen(playPlaylistsRequest) + 1, 0);

	receiveSize = recv(sock, messageBuff, 5000, 0);
	if (receiveSize > 0) {
		cout << messageBuff << endl;
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
