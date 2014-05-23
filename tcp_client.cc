// Codestyle guide: https://google-styleguide.googlecode.com/svn/trunk/cppguide.xml

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>

#include "headers.h"

using namespace std;

int main(int argc, char **argv) {
 	int clientSocket;

	if (argc < 3) {
		fprintf (stderr, "usage : %s <server name/ip> <server port>\n", argv[0]);
		exit(0);
	}
	
	// Create the socket
 	if ((clientSocket = socket (AF_INET, SOCK_STREAM, 0)) <0) {
  		perror("Problem in creating the socket");
	  	exit(2);
 	}

	 // Set up the server address
	struct sockaddr_in serverAddress;
 
	struct addrinfo socketSpecs;
	memset(&socketSpecs, 0, sizeof(struct addrinfo));
	socketSpecs.ai_family = AF_INET;	//ipv4
	socketSpecs.ai_socktype = SOCK_STREAM; //tdp

	//as the port number is not defined, we get all matching sockets populted in results
	struct addrinfo *results;
	int addrInfo = getaddrinfo (argv[1], NULL, (const struct addrinfo *) (&socketSpecs), &results);
	if ( addrInfo != 0) {
		cerr << "Did not find matching sockets" << endl; 
		return 0;
	}

	serverAddress.sin_family = AF_INET;

	//we iterate through the matching sockets to find the correct one and populate our server address
	struct addrinfo *j;
	for (j = results; j != NULL; j = j->ai_next) {
		if (j->ai_family == AF_INET) {
			memcpy (&serverAddress, j->ai_addr, sizeof(struct sockaddr_in));
			break;
		}
	}
	serverAddress.sin_port = htons(atoi(argv[2]));
    
     	//Connection of the client to the socket
 	if (connect(clientSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress))<0) {
  		perror("Problem in connecting to the server");
  		exit(3);
 	}
	
	char buf [256];
	memset(buf, 0, 256);

	string command;
	while (getline(cin, command) || cin.eof()) {
		if (cin.eof()) {
			strcpy (buf, STOP_SESSION);
		} else if (command == STOP) {
			strcpy(buf, STOP);
		} else {
            if (command.length() + 4 > BUF_SIZE) {
				cerr << "Input is too large" << endl;
				continue;
			}
            command = string(GET) + " " + command;
			strcpy(buf, command.c_str());
		}

		fflush(stdout);
        
        	// Send message to the server
		int length = sendto(clientSocket, &buf, sizeof(buf) + 1, 0, (const struct sockaddr *)(&serverAddress), sizeof(struct sockaddr_in));
        
		if (length < strlen(buf) + 1) {
			fprintf(stderr, "Tried to send %lu, sent only %d\n", strlen(buf) + 1, length);
		}

		if (cin.eof() || command == STOP)
			break;	

		memset(buf, 0, 256);
        
       		// Read message from server
		recvfrom(clientSocket, buf, 256, 0, NULL, NULL);

		bool hasNullTerminate = false;
		// check if '/0' exists
		for (int i = 0; i < 256; i++) {
			if (buf[i] == '\0') {
				hasNullTerminate = true;
				break;
			}
		}

		string output(buf);
		if (output.find("error") != string::npos) {
			cerr << buf << endl;
		} else if (!hasNullTerminate) {
			cerr << "error: corrupted result" << endl;
		} else {
			cout << buf << endl; 
        	}
        
		memset(buf, 0, 256);
	}

	close(clientSocket);
	return 0;
}
