#include "common.h"

using namespace std;

const string kStop = "STOP";
const string kStopSession = "STOP_SESSION";
const int kBufSize = 256;

//Source: the initial socket setup was shown in tutorial on May 18,2014

int main (int argc, char *argv[]) {

	// bad arguments
	if (argc > 2) {
		cerr << "usage : " << argv[0] << " <optional server port>" << endl;
		return 0;
	}

	// Defaut port is 0
	unsigned short port = (argc < 2)?  0: (unsigned short)atoi(argv[1]);

	// Create a socket for UDP server
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

	// prints out publicly reachable IP address/domain name and port number
	// source: http://man7.org/linux/man-pages/man3/getifaddrs.3.html

	struct ifaddrs *ifaddr;
	if (getifaddrs(&ifaddr) == -1) {
		cerr << "no ifaddrs found" << endl;
		return 0;
	}

	char buf[kBufSize];
	struct ifaddrs *ifa;
	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		
		if (ifa->ifa_addr != NULL && ifa->ifa_addr->sa_family == AF_INET)
		{
			memset(buf, 0, kBufSize);
			if (getnameinfo(ifa->ifa_addr,sizeof(struct sockaddr_in), buf, kBufSize, NULL, 0, NI_NUMERICHOST) != 0) {
				cerr << "Not able to find the ip address" << endl;
				return 0;	
			}
		}
	}

	freeifaddrs(ifaddr);
	cout << buf << " " << ntohs(sockInfo.sin_port) << endl; 
	memset(&sockInfo, 0, sizeof(struct sockaddr_in));

	//Read from stdin information on groups till it sees an EOF. 
	populateGroups();

	memset(buf, 0, kBufSize);
	//Accept client commands
	while (recvfrom(socketId, buf, kBufSize, 0, (struct sockaddr*) (&sockInfo), (socklen_t*) (&addrlen))) {

		// tells the server to terminate; i.e., the server process dies. Termination must be graceful.
		if (strcmp(kStop.c_str(), buf) == 0)
			break;

		// signals the server that the client will stop sending data.
		if (strcmp(kStopSession.c_str(), buf) == 0) {
			// we ignore this case for the udp server, no connection to 'close'

		} else {

			//look up corresponding student name
			string command(buf);
			string studentName = getStudentNameWithGet(command);

			const char* name = studentName.c_str();
			strcpy(buf, name);

			int len;
			if ((len = sendto(socketId, buf, strlen(buf) + 1, 0, (const struct sockaddr *)(&sockInfo), sizeof(struct sockaddr_in))) < strlen(buf) + 1) {
				cerr << "Tried to send " << (strlen(buf) + 1) << ", sent only " << len << endl;
			}
		}

		memset(buf, 0, kBufSize);
	}

	close(socketId);
	return 0;
}
