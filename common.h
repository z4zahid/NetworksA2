#ifndef COMMON_H
#define COMMON_H

#include "headers.h"
#include <map>
#include <vector>

#define	PORT_RANGE_LO	10000
#define PORT_RANGE_HI	10100

struct group {
   int groupId;
   std::map<int, std::string> students;
};

void populateGroups();
std::string getStudentNameWithGet(std::string command);
std::string getStudentName(std::string groupNum, std::string studentNum);
int mybind(int sockfd, struct sockaddr_in* addr);

#endif
