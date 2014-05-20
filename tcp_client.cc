#include "headers.h"
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>

using namespace std;

int main (int argc, char *argv[]) {

	if (argc < 3) {
		fprintf (stderr, "usage : %s <server name/ip> <server port>\n", argv[0]);
		exit(0);
	}

    // Create socket in the client
	int _socket;
	if ((_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		exit(0);
	}
    
    // Create socket address
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = 0;
	addr.sin_addr.s_addr = INADDR_ANY;
    
	if (bind(_socket, (const struct sockaddr *) (&addr), sizeof(struct sockaddr_in)) < 0) {
		perror("bind");
		exit(0);
	}

    // Create server address
	struct sockaddr_in servaddr;
	unsigned short portnum;

	if (sscanf(argv[2], "%hu", &portnum) < 1) {
		fprintf(stderr, "sscanf() failed.\n");
	}
    
	/* user getaddrinfo() to get server IP */
	struct addrinfo *res, hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	int addrInfo = getaddrinfo (argv[1], NULL, (const struct addrinfo *) (&hints), &res);
	if ( addrInfo != 0) {
		perror("getaddrinfo");
		printf("%d\n", addrInfo);
		exit(0);
	}

	struct addrinfo *clientAddressInfo;
	for (clientAddressInfo = res; clientAddressInfo != NULL; clientAddressInfo = clientAddressInfo->ai_next) {
		if (clientAddressInfo->ai_family == AF_INET) {
			memcpy (&servaddr, clientAddressInfo->ai_addr, sizeof(struct sockaddr_in));
			break;
		}
	}

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons (portnum);

    // Connection of the client to the socket
    if (connect(_socket, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("Problem in connecting to the server");
        exit(3);
    }
    
    printf("connect\n");
    
	char buf [256];
	memset(buf, 0, 256);		

	string command;
	while (getline(cin, command) || cin.eof()) {
		if (cin.eof()) {
			strcpy (buf, STOP_SESSION);
		} else if (command == STOP) {
			strcpy(buf, STOP);
		} else {
            command = string(GET) + " " + command;
			strcpy(buf, command.c_str());
            printf("\n");
            printf(command.c_str());
            printf("\n");
		}

		fflush(stdout);
        
        printf("moo\n");

        // Send message to the server
		int length =
            sendto(_socket, buf, strlen(buf) + 1, 0, (const struct sockaddr *)(&servaddr), sizeof(struct sockaddr_in));
        
        printf("RAWR\n");
		if (length < strlen(buf) + 1) {
			fprintf(stderr, "Tried to send %lu, sent only %d\n", strlen(buf) + 1, length);
		}

		if (cin.eof() || command == STOP)
			break;	

		memset(buf, 0, 256);
        
        // Read message from server
		recvfrom(_socket, buf, 256, 0, NULL, NULL);
		
		string output(buf);
		if (output.find("error") != string::npos) {
			cerr << buf << endl;
		} else {
			cout << buf << endl; 
        }
        
		memset(buf, 0, 256);
	}

	close(_socket);
	return 0;

}
