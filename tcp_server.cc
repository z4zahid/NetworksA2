// Codestyle guide: https://google-styleguide.googlecode.com/svn/trunk/cppguide.xml

#include "common.h"

#define BUFFER_SIZE 256

using namespace std;

int main (int argc, char *argv[]) {

	int serverSocket;
	unsigned short portNumber;

	// Grab port if provided, otherwise assign 0
	portNumber = (argc < 2) ? 0 : (unsigned short) atoi(argv[1]);

	// Create TCP socket
	if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(0);
	}

	// Structure to provide socket info for bind
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(portNumber);
	addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket
    // If it fails, exit the program
	if (mybind (serverSocket, (struct sockaddr_in *)(&addr)) < 0) {
		perror ("bind");
		exit(0);
	}

	struct ifaddrs *ifaddr;
	if (getifaddrs(&ifaddr) == -1) {
		cerr << "no ifaddrs found" << endl;
		return 0;
	}

	char buf[BUFFER_SIZE];
	struct ifaddrs *ifa;
	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		
		if (ifa->ifa_addr != NULL && ifa->ifa_addr->sa_family == AF_INET)
		{
			memset(buf, 0, BUFFER_SIZE);
			if (getnameinfo(ifa->ifa_addr,sizeof(struct sockaddr_in), buf, BUFFER_SIZE, NULL, 0, NI_NUMERICHOST) != 0) {
				cerr << "Not able to find the ip address" << endl;
				return 0;	
			}
		}
	}
	
	//prints out port number (with no embellishment whatsoever — the port number only)	
	cout << buf << " " << ntohs(addr.sin_port) << endl; 

	// Listen for request
    	if (listen(serverSocket, 8) == -1) {
        	perror("Issues listening");
        	exit(0);
    	}
    
    	    
	//Read from stdin information on groups till it sees an EOF.
	populateGroups();
	
	bool stop = false; 
 	while (!stop) {
		struct sockaddr_in clientAddr;
        	int clientAddrlen = sizeof(struct sockaddr_in);
    		// Accept request
    		int connectSocket = accept(serverSocket, (struct sockaddr *) (&clientAddr), (socklen_t*) (&clientAddrlen));
	
		while (recv(connectSocket, buf, BUFFER_SIZE, 0) > 0) {	
			string command(buf);
			// Terminate server command
			if (strcmp(STOP, buf) == 0) {
				stop = true;
				break;
			// Signals the server that client will stop sending data
			} else if (strcmp(STOP_SESSION, buf) == 0) {
                shutdown(connectSocket, SHUT_RDWR);
				close(connectSocket);
				break;
			} else if (strcmp(GET, command.substr(0, 3).c_str()) == 0) {
				string studentName = getStudentNameWithGet(command);
				const char* message = studentName.c_str();
				strcpy(buf, message);
                
				// Send the result back to the client
				int length;
				if ((length = sendto(connectSocket, buf, strlen(buf) + 1, 0, (const struct sockaddr *)(&addr), sizeof(struct sockaddr_in))) < strlen(buf) + 1) {
					fprintf(stderr, "Tried to send %lu, sent only %d\n", strlen(buf) + 1, length);
				}
			}
			memset(buf, 0, BUFFER_SIZE);
		}
		
	}
    
	close(serverSocket);
	return 0;
}
