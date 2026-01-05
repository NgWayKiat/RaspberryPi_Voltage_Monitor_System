#ifndef GLOBAL_HDR_FILE_
#define GLOBAL_HDR_FILE_

#include <string>
#include <cstring>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <map>
#include "Versions.h"
#include "Misc.h"
#include "Logger.h"

using namespace std;

//To declare the log file stored location in DEBUG or RELEASE MODE
#define isRelease true
#define DEFAULT_BUFLEN 1024

int initiate_MappingTables();
void initiate_GlobalVariable();
int remoteServerMapping(string sText);
int globalVariableMapping(string sText);
int mcc118ConfigFlagMapping(string sText);
void disRemoteServerMappingList();
void disGlobalVariableMappingList();
string findGlobalVariableMap(string key);

#pragma region GlobalVariables

extern int giReadChannel;
extern int giHostPort;
extern int giLogDays;
extern bool gbMCC118ConfigFlag;
extern int giMSScan;
extern string delimiterL1;
extern char delimiterL2;

#pragma endregion GlobalVariables

#pragma region StructMap

struct ClientServer {
    string name;
    string ip;
    int port;
    int status;  // 0:DISCONNECTED 1:CONNECTED 2:ERROR
    int sok;  // Keep socket value
};

struct GlobalVar{
    string name;
    string value;
    string sDecription;
};

struct MCC118_Config_Flag{
    string name;
    int flag;
    string sDecription;
};

extern map<string, ClientServer> ClientServerMapList;
extern map<string, GlobalVar> GlobalVarList;
extern map<string, MCC118_Config_Flag> MCC118ConfigList;

#pragma endregion StructMap

#pragma region MCC118

struct MCC118HatInfoSub{
    int iChannel;
    string sName;
    string sDescription;
    double value;
    double oriSlope;
    double oriOffset;
    double newSlope;
    double newOffset;
    bool bStatus;
};

struct MCC118HatInfo{
    int iTotalChannel;
    int address;
    double scanRate;
    double actScanRate;
    double minADCCode;
    double maxADCCode;
    double minVoltage;
    double maxVoltage;
    double minRange;
    double maxRange;
    struct MCC118HatInfoSub subInfo[20];
};

extern MCC118HatInfo structMCC118HatInfo;

#pragma endregion MCC118

#endif