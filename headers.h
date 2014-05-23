#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <ifaddrs.h>
#include <unistd.h>

#include <string>
#include <iostream>

const string kStop = "STOP"
const string kStopL = "stop"
const string kStopSession = "STOP_SESSION"
const string kGet = "GET"
const int kBufSize = 256;

