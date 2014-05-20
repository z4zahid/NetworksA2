#include "common.h"

using namespace std;

int main (int argc, char *argv[]) {

	int _socket;
	unsigned short portnum;

	// Grab port if provided, otherwise assign 0
	if (argc < 2) 
		portnum = 0;
	else 
		portnum = (unsigned short) atoi(argv[1]);

	// Create TCP socket
	if ((_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(0);
	}

	// Structure to provide socket info for bind
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(portnum);
	addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket
    // If it fails, exit the program
	if (mybind (_socket, (struct sockaddr_in *)(&addr)) < 0) {
		perror ("bind");
		exit(0);
	}

	memset (&addr, 0, sizeof(struct sockaddr_in));

	// Check if the port gets specified
	int addrlen = sizeof(struct sockaddr_in);
	if (getsockname(_socket, (struct sockaddr *) (&addr), (socklen_t*) (&addrlen)) < 0) {
		perror ("getsockname");
		exit(0);
	}

	//prints out port number (with no embellishment whatsoever — the port number only)
	printf ("%hu\n", ntohs(addr.sin_port));
	memset(&addr, 0, sizeof(struct sockaddr_in));

    // Listen for request
    if (listen(_socket, 256) == -1) {
        perror("Issues listening");
        exit(0);
    }
    
    struct sockaddr_in clientAddr;
    int clientAddrlen = sizeof(struct sockaddr_in);
    
    
	//Read from stdin information on groups till it sees an EOF.
    printf("start\n");
	populateGroups();

    printf("WHOO!\n");
    
    // Accept request
    accept(_socket, (struct sockaddr *) (&clientAddr), (socklen_t*) (&clientAddrlen));
    printf("done!\n");
    /*
	char buf[256];
	// Accept client commands
	while (recvfrom(_socket, buf, 256, 0, (struct sockaddr*)(&addr), (socklen_t*)(&addrlen))) {
		// Terminate server command
		if (strcmp(STOP, buf) == 0)
			break;

		// Signals the server that client will stop sending data
		if (strcmp(STOP_SESSION, buf) == 0) {
			cout << "TODO: stop client session - handle multiple client case" << endl;
            
		} else if (strcmp(GET, buf) == 0) {
            string command(buf);
            string studentName = getStudentNameWithGet(command);
            const char* message = studentName.c_str();
            strcpy(buf, message);

            // Send the result back to the client
            int length;
            if ((length = sendto(_socket, buf, strlen(buf) + 1, 0, (const struct sockaddr *)(&addr), sizeof(struct sockaddr_in))) < strlen(buf) + 1) {
                fprintf(stderr, "Tried to send %lu, sent only %d\n", strlen(buf) + 1, length);
            }
		} else {
            fprintf(stderr, "Invalid command %s\n", buf);
        }
		memset(buf, 0, 256);
	}*/
    
	close(_socket);
	return 0;
}
