#include "headers.h"
#include <iostream>
#include <cctype>
#include <sys/types.h>
#include <sys/socket.h>

using namespace std;

//Source: the initial socket setup was shown in tutorial on May 18,2014
int main (int argc, char *argv[]) {

	// bad arguments
	if (argc < 3) {
		cerr << "usage : " << argv[0] << " <server name/ip> <server port>" << endl;
		return 0;
	}

	// Create a socket for UDP
	int socketId;
	if ((socketId = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		cerr << "Could not create socket" << endl;
		return 0;
	}

	struct sockaddr_in socketInfo;
	socketInfo.sin_port = 0;
	socketInfo.sin_family = AF_INET;
	socketInfo.sin_addr.s_addr = INADDR_ANY;

	if (bind(socketId, (const struct sockaddr *) (&socketInfo), sizeof(struct sockaddr_in)) < 0) {
		cerr << "socket could not bind" << endl;
		return 0;
	}

	struct sockaddr_in sa; /* Server address */
	unsigned short portnum;

	if (sscanf(argv[2], "%hu", &portnum) < 1) {
		cerr << "sscanf() failed" << endl;
	}

	/* user getaddrinfo() to get server IP */
	struct addrinfo *res, hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;

	int addrInfo = getaddrinfo (argv[1], NULL, (const struct addrinfo *) (&hints), &res);
	if ( addrInfo != 0) {
		cerr << "Did not get AddrInfo" << endl; 
		return 0;
	}

	struct addrinfo *cai;
	for (cai = res; cai != NULL; cai = cai->ai_next) {
		if (cai->ai_family == AF_INET) {
			memcpy (&sa, cai->ai_addr, sizeof(struct sockaddr_in));
			break;
		}
	}

	sa.sin_family = AF_INET;
	sa.sin_port = htons (portnum);

	char buf [256];
	string command;

	while (getline(cin, command) || cin.eof()) {

		//make sure buffer is clear
		memset(buf, 0, 256);		

		//fill buffer with msg to be sent to server
		if (cin.eof()) {
			strcpy (buf, STOP_SESSION);
		} else if (command == STOP || command == STOP_L) {
			strcpy(buf, STOP);
		} else {
			command = "GET " + command;
			strcpy(buf, command.c_str());
		}

		int len = sendto(socketId, buf, strlen(buf) + 1, 0, (const struct sockaddr *)(&sa), sizeof(struct sockaddr_in));
		if (len < strlen(buf) + 1) {
			cerr << "Tried to send " << (strlen(buf) + 1) << ", sent only " << len << endl;
		}

		//terminate client upon reaching end of input or getting the STOP command
		if (cin.eof() || command == STOP)
			break;	

		//await reply from server, to be filled in to buffer
		memset(buf, 0, 256);		
		recvfrom(socketId, buf, 256, 0, NULL, NULL);
		
		string output(buf);
		if (output.find("error") != string::npos)
			cerr << buf << endl;
		else
			cout << buf << endl; 
	}

	close(socketId);	
	return 0;

}
