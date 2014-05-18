#include "common.h"

using namespace std;

//Much of the source below was shown in tutorial on May 18,2014
int main (int argc, char *argv[]) {

	int s;
	unsigned short portnum;

	// Grab port if provided, otherwise assign 0
	if (argc < 2) 
		portnum = 0;
	else 
		portnum = (unsigned short)atoi(argv[1]);

	// Create a socket for UDP
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		exit(0);
	}

	// Structure to provide socket info for bind
	struct sockaddr_in a;
	a.sin_family = AF_INET;
	a.sin_port = htons(portnum);
	a.sin_addr.s_addr = INADDR_ANY;

	if (mybind (s, (struct sockaddr_in *)(&a)) < 0) {
		perror ("bind");
		exit(0);
	}

	memset (&a, 0, sizeof(struct sockaddr_in));

	// In case port wasnt specified
	int addrlen = sizeof(struct sockaddr_in);
	if (getsockname(s, (struct sockaddr *) (&a), (socklen_t*) (&addrlen)) < 0) {
		perror ("getsockname");
		exit (0);
	}

	//prints out port number (with no embellishment whatsoever — the port number only)
	printf ("%hu\n", ntohs(a.sin_port));
	memset(&a, 0, sizeof(struct sockaddr_in));

	//Read from stdin information on groups till it sees an EOF. 
	populateGroups();

	char buf[256];

	//Accept client commands
	while (recvfrom(s, buf, 256, 0, (struct sockaddr*) (&a), (socklen_t*) (&addrlen))) {

		// tells the server to terminate; i.e., the server process dies. Termination must be graceful.
		if (strcmp("STOP", buf) == 0)
			break;

		// signals the server that the client will stop sending data.
		if (strcmp(STOP_SESSION, buf) == 0) {
			cout << "TODO: stop client session - handle multiple client case" << endl;

		} else {

			string command(buf);
			string studentName = getStudentName(command);

			const char* name = studentName.c_str();
			strcpy(buf, name);

			int len;
			if ((len = sendto(s, buf, strlen(buf) + 1, 0, (const struct sockaddr *)(&a), sizeof(struct sockaddr_in))) < strlen(buf) + 1) {
				fprintf(stderr, "Tried to send %d, sent only %d\n", strlen(buf) + 1, len );
			}
		}

		memset(buf, 0, 256);
	}

	close(s);
	return 0;
}
