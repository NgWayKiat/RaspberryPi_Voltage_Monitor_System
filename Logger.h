#ifndef LOG_HDR_FILE_
#define LOG_HDR_FILE_

#include <string>
#include <cstring>
#include <sys/stat.h>
#include <time.h>
#include <cstdio>
#include "unistd.h"
#include "fcntl.h"
#include "Global.h"
#include "Misc.h"
#include "Versions.h"


using namespace std;

#define DEFAULT_BUFLEN 1024
#define FILE_SIZE 5000

enum LogLevel { DEBUG, INFO, WARNING, ERRORS, CRITICAL };

void checkLogDirectory();
int deleteLogFile();
void writeToLog(LogLevel level, char* message);
string getAvailableFileName();

#endif