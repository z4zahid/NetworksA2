#include "headers.h"
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>

#define MAXLINE 4096 /*max text line length*/
using namespace std;

int main(int argc, char **argv) {
 	int clientSocket;
 	struct sockaddr_in servaddr;
 	char sendline[MAXLINE], recvline[MAXLINE];

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
 	memset(&servaddr, 0, sizeof(servaddr));
 	servaddr.sin_family = AF_INET;
 	servaddr.sin_addr.s_addr= inet_addr(argv[1]);
 	servaddr.sin_port =  htons(atoi(argv[2])); //convert to big-endian order

 	//Connection of the client to the socket
 	if (connect(clientSocket, (struct sockaddr *) &servaddr, sizeof(servaddr))<0) {
  		perror("Problem in connecting to the server");
  		exit(3);
 	}
	
	cout << "Connected to the server" << endl;

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
		}

		fflush(stdout);

        	// Send message to the server
		int length =  send(clientSocket, &buf, sizeof(buf), 0);
        
		if (length < strlen(buf) + 1) {
			fprintf(stderr, "Tried to send %lu, sent only %d\n", strlen(buf) + 1, length);
		}

		if (cin.eof() || command == STOP)
			break;	

		memset(buf, 0, 256);
        
       		// Read message from server
		recvfrom(clientSocket, buf, 256, 0, NULL, NULL);

		string output(buf);
		if (output.find("error") != string::npos) {
			cerr << buf << endl;
		} else {
			cout << buf << endl; 
        	}
        
		memset(buf, 0, 256);
	}

	close(clientSocket);
	return 0;
}
