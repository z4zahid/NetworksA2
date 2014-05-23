#include "headers.h"
#include <iostream>
#include <cctype>
#include <sys/types.h>
#include <sys/socket.h>

using namespace std;

//Source: the initial socket setup was shown in tutorial on May 18,2014

int main (int argc, char *argv[]) {

	// bad arguments
	if (argc != 3) {
		cerr << "usage : " << argv[0] << " <server name/ip> <server port>" << endl;
		return 0;
	}

	// parse the port number passed in
	unsigned short port;
	if (sscanf(argv[2], "%hu", &port) < 1) {
		cerr << "Not a valid port number" << endl;
		return 0;
	}

	// Create a socket for UDP client
	int socketId;
	if ((socketId = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		cerr << "Could not create socket" << endl;
		return 0;
	}

	//find a port to bind client socket to
	struct sockaddr_in socketInfo;
	socketInfo.sin_port = 0;
	socketInfo.sin_family = AF_INET;
	socketInfo.sin_addr.s_addr = INADDR_ANY;

	if (bind(socketId, (const struct sockaddr *) (&socketInfo), sizeof(struct sockaddr_in)) < 0) {
		cerr << "socket could not bind" << endl;
		return 0;
	}

	//support for both ip and domain name
	//socketSpecs tells getaddrinfo to get us udp sockets, in the iPv4 family
	struct addrinfo socketSpecs;
	memset(&socketSpecs, 0, sizeof(struct addrinfo));
	socketSpecs.ai_family = AF_INET;	//ipv4
	socketSpecs.ai_socktype = SOCK_DGRAM; //udp

	//as the port number is not defined, we get all matching sockets populted in results
	struct addrinfo *results;
	int addrInfo = getaddrinfo (argv[1], NULL, (const struct addrinfo *) (&socketSpecs), &results);
	if ( addrInfo != 0) {
		cerr << "Did not find matching sockets" << endl; 
		return 0;
	}

	struct sockaddr_in serverAddress; 
	serverAddress.sin_family = AF_INET;

	//we iterate through the matching sockets to find the correct one and populate our server address
	struct addrinfo *j;
	for (j = results; j != NULL; j = j->ai_next) {
		if (j->ai_family == AF_INET) {
			memcpy (&serverAddress, j->ai_addr, sizeof(struct sockaddr_in));
			break;
		}
	}

	//careful to add this after so its no over written in the memcpy
	serverAddress.sin_port = htons (port);

	char buf [BUF_SIZE];
	string command;

	while (getline(cin, command) || cin.eof()) {

		//make sure buffer is clear
		memset(buf, 0, BUF_SIZE);		

		//fill buffer with msg to be sent to server
		if (cin.eof()) {
			strcpy (buf, STOP_SESSION);
		} else if (command == STOP || command == STOP_L) {
			strcpy(buf, STOP);
		} else {
			if (command.length() > BUF_SIZE)
			{
				cerr << "Input is too large" << endl;
				return 0;
			}
			command = "GET " + command;
			strcpy(buf, command.c_str());
		}

		int len = sendto(socketId, buf, strlen(buf) + 1, 0, (const struct sockaddr *)(&serverAddress), sizeof(struct sockaddr_in));
		if (len < strlen(buf) + 1) {
			cerr << "Only sent: " << len << endl;
		}

		//terminate client upon reaching end of input or getting the STOP command
		if (cin.eof() || command == STOP || command == STOP_L)
			break;	

		//await reply from server, to be filled in to buffer
		memset(buf, 0, BUF_SIZE);		
		recvfrom(socketId, buf, BUF_SIZE, 0, NULL, NULL);
		
		string output(buf);
		if (output.find("error") != string::npos)
			cerr << buf << endl;
		else
			cout << buf << endl; 
	}

	close(socketId);
	shutdown(socket, SHUT_RDWR);	
	return 0;

}
