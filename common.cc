// Codestyle guide: https://google-styleguide.googlecode.com/svn/trunk/cppguide.xml

#include <sstream>
#include "common.h"

using namespace std;

vector<group> groups;

bool convertToInt(string s, int &result) {
    std::stringstream ss;
    ss << s;
    ss >> result;
    return (!ss.fail());
}

vector<string> parseParamsFromClient(string command) {

	stringstream commandStream(command);
	char delimiter = ' ';
	vector<string> params;

	string param;
	while (getline(commandStream, param, delimiter)) {
	    params.push_back(param);
	}

	return params;
}

std::vector<string> parseParamsFromStdIn(string command) {

	char delimiter = ' ';
	vector<string> params;

	string firstParam = command.substr(0, command.find(delimiter));
	params.push_back(firstParam);

	string secondParam = command.substr(command.find(firstParam) + firstParam.length() + 1);
	params.push_back(secondParam);

	return params;
}

string getStudentName(string groupNum, string studentNum) {
    int groupId, studentId;
    if (convertToInt(groupNum, groupId) && convertToInt(studentNum, studentId)) {
        for (vector<group>::iterator it = groups.begin() ; it != groups.end(); ++it) {
            
            group m_group = *it;
            if (m_group.groupId == groupId) {

                map<int,string>::iterator studentIterator = m_group.students.find(studentId);
                if (studentIterator != m_group.students.end()) {
                    return studentIterator->second;
                }
            }
        }
    } else {
	return "error: invalid command";
    }
	//doesn't exist
	return "error: " + groupNum + " " + studentNum;
}

string getStudentNameWithGet(string command) {
    vector<string> params = parseParamsFromClient(command);
    
    if (params.size() != 3) {
		return "error: invalid input";
    }
	string groupNum = params[1];
	string studentNum = params[2];
	return getStudentName(groupNum, studentNum);
}

void addGroup(int curGroupId, map<int, string> curStudents) {
	group m_group;
	m_group.groupId = curGroupId;
	m_group.students = curStudents;
	groups.push_back(m_group);
}

void flush(int curGroupId, map<int, string> *studentMap) {
	if (curGroupId != -1 && studentMap->size() > 0)
	{
		map<int, string> curStudents(*studentMap);
		addGroup(curGroupId, curStudents);
		studentMap->clear();
	}
}

//Note: assumes input is correct, that it does not try later to add to an existing group etc (asked TA)
void populateGroups() {
	string line;
	int curGroupId = -1;
	map<int, string> m_students;
    bool valid = false;
	while (getline(cin, line)) {
		if (!cin.eof()) {
			vector<string> params = parseParamsFromStdIn(line);
			if (params[0] == "Group") {
				flush(curGroupId, &m_students);
                valid = convertToInt(params[1], curGroupId);
			} else {
				int studentId;
                valid = convertToInt(params[0], studentId);
				string studentName = params[1];
                if (valid) {
                    m_students.insert (std::pair<int, string>(studentId, studentName));
                }
			}
		}
	}
    if (valid) {
        flush(curGroupId, &m_students);
    }
}

// AS PROVIDED BY PROF. TRIPUNITARA
int mybind(int sockfd, struct sockaddr_in* addr) {
	if(sockfd < 1) {
		fprintf(stderr, "mybind(): sockfd has invalid value %d\n", sockfd);
		return -1;
	}

	if(addr == NULL) {
		fprintf(stderr, "mybind(): addr is NULL\n");
		return -1;
	}

	if(addr->sin_port != 0) {
		fprintf(stderr, "mybind(): addr->sin_port is non-zero. Perhaps you want bind() instead?\n");
		return -1;
	}

	unsigned short p;
	for(p = PORT_RANGE_LO; p <= PORT_RANGE_HI; p++) {
		addr->sin_port = htons(p);
		int b = bind(sockfd, (const struct sockaddr *)addr, sizeof(struct sockaddr_in));
		if(b < 0) {
		    continue;
		}
		else {
		    break;
		}
	}

	if(p > PORT_RANGE_HI) {
		fprintf(stderr, "mybind(): all bind() attempts failed. No port available...?\n");
		return -1;
	}

	/* Note: upon successful return, addr->sin_port contains, in network byte order, the
	 * port to which we successfully bound. */
	return 0;
}


