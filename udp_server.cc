#include "common.h"

using namespace std;

//Source: the initial socket setup was shown in tutorial on May 18,2014

int main (int argc, char *argv[]) {

	// bad arguments
	if (argc > 2) {
		cerr << "usage : " << argv[0] << " <optional server port>" << endl;
		return 0;
	}

	// Defaut port is 0
	unsigned short port = port = (argc < 2)?  0: (unsigned short)atoi(argv[1]);

	// Create a socket for UDP
	int socketId;
	if ((socketId = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		cerr << "Socket was not created" << endl;
		return 0;
	}

	struct sockaddr_in sockInfo;
	sockInfo.sin_port = htons(port);
	sockInfo.sin_family = AF_INET;
	sockInfo.sin_addr.s_addr = INADDR_ANY;

	if (mybind (socketId, (struct sockaddr_in *) (&sockInfo)) < 0) {
		cerr << "Could not bind to the socket" << endl;
		return 0;
	}

	//clear sockinfo
	memset (&sockInfo, 0, sizeof(struct sockaddr_in));

	int addrlen = sizeof(struct sockaddr_in);
	if (getsockname(socketId, (struct sockaddr *) (&sockInfo), (socklen_t*) (&addrlen)) < 0) {
		cerr << "Did not create socket name" << endl;
		return 0;
	}

	//prints out port number (with no embellishment whatsoever — the port number only)
	cout << ntohs(sockInfo.sin_port) << endl; 
	memset(&sockInfo, 0, sizeof(struct sockaddr_in));

	//Read from stdin information on groups till it sees an EOF. 
	populateGroups();

	char buf[256];
	//Accept client commands
	while (recvfrom(socketId, buf, 256, 0, (struct sockaddr*) (&sockInfo), (socklen_t*) (&addrlen))) {

		// tells the server to terminate; i.e., the server process dies. Termination must be graceful.
		if (strcmp("STOP", buf) == 0)
			break;

		// signals the server that the client will stop sending data.
		if (strcmp(STOP_SESSION, buf) == 0) {
			cout << "TODO: stop client session - handle multiple client case" << endl;

		} else {

			//look up corresponding student name
			string command(buf);
			string studentName = getStudentName(command);

			const char* name = studentName.c_str();
			strcpy(buf, name);

			int len;
			if ((len = sendto(socketId, buf, strlen(buf) + 1, 0, (const struct sockaddr *)(&sockInfo), sizeof(struct sockaddr_in))) < strlen(buf) + 1) {
				cerr << "Tried to send " << (strlen(buf) + 1) << ", sent only " << len << endl;
			}
		}

		memset(buf, 0, 256);
	}

	close(socketId);
	return 0;
}
