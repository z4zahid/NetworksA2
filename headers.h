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

#define STOP "STOP"
#define STOP_SESSION "STOP_SESSION"
#define GET "GET"