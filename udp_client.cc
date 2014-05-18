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

	int s;
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		exit(0);
	}

	struct sockaddr_in a;
	a.sin_family = AF_INET;
	a.sin_port = 0;
	a.sin_addr.s_addr = INADDR_ANY;

	if (bind(s, (const struct sockaddr *) (&a), sizeof(struct sockaddr_in)) < 0) {
		perror("bind");
		exit(0);
	}

	struct sockaddr_in sa; /* Server address */
	unsigned short portnum;

	if (sscanf(argv[2], "%hu", &portnum) < 1) {
		fprintf(stderr, "sscanf() failed.\n");
	}

	/* user getaddrinfo() to get server IP */
	struct addrinfo *res, hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;

	int addrInfo = getaddrinfo (argv[1], NULL, (const struct addrinfo *) (&hints), &res);
	if ( addrInfo != 0) {
		perror("getaddrinfo");
		printf("%d\n", addrInfo);
		exit (0);
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
	memset(buf, 0, 256);		

	string command;
	while (getline(cin, command) || cin.eof()) {

		if (cin.eof()) {
			strcpy (buf, STOP_SESSION);
		} else if (command == STOP) {
			strcpy(buf, STOP);
		} else {
			//TODO: asked TA if 'GET' is necessary
			strcpy(buf, command.c_str());
		}

		fflush(stdout);

		int len = sendto(s, buf, strlen(buf) + 1, 0, (const struct sockaddr *)(&sa), sizeof(struct sockaddr_in));
		if (len < strlen(buf) + 1) {
			fprintf(stderr, "Tried to send %d, sent only %d\n", strlen(buf) + 1, len);
		}

		if (cin.eof() || command == STOP)
			break;	

		memset(buf, 0, 256);		
		recvfrom(s, buf, 256, 0, NULL, NULL);
		
		string output(buf);
		if (output.find("error") != string::npos)
			cerr << buf << endl;
		else
			cout << buf << endl; 

		memset(buf, 0, 256);	

	}

	close(s);	
	return 0;

}
