#ifndef CLTSOK_HDR_FILE_
#define CLTSOK_HDR_FILE_

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <pthread.h>
#include <thread>
#include "Global.h"
#include "Logger.h"
#include "Versions.h"

using namespace std;

void connect();

#endif