#include "Global.h"

map<string, ClientServer> ClientServerMapList;
map<string, GlobalVar> GlobalVarList;
map<string, MCC118_Config_Flag> MCC118ConfigList;
MCC118HatInfo structMCC118HatInfo;

#pragma region InitGlobalVariables

int giHostPort = 8888;
int giLogDays = 7;
bool gbMCC118ConfigFlag = false;
int giMSScan = 5000000; // Suspend execution for 100,000 microseconds
int giReadChannel = 0;
string delimiterL1 = "|:|";
char delimiterL2 = '=';

#pragma endregion InitGlobalVariables

int initiate_MappingTables()
{
    int ret = 0 ;
    bool isTemp = false;
    bool isRemoteServer = false;
    bool isGlobalVariables = false;
    bool isMCC118Config = false;
    int iTemp = 0;
    char buf[DEFAULT_BUFLEN] = {0};
    string sText = "";
    string sTempText = "";
    string sPrevTempText = "";
    string configDir = "";

    if(filesystem::current_path().generic_string() == "" || filesystem::current_path().generic_string()=="/")
    {
        configDir = "/BIO/Program/" + sProgramName + "/sys.config";
    }
    else
    {
        configDir = filesystem::current_path().generic_string() + "/sys.config";
    }

    ifstream file (configDir);
    //ifstream file("/BIO/SourcCode/MCC_DAQ_HATS/sys.config");  

    if (file.is_open())
    {
        while (getline (file, sText)) {
            if(sText == "[SYSTEM_VARIABLE]" || sText == "[REMOTE_SERVER]" || sText == "[MCC118_CHANNEL_FLAG]")
            {
                if(sText == "[REMOTE_SERVER]")
                {
                    isRemoteServer = true;
                    isGlobalVariables = false;
                    isMCC118Config = false;
                }
                else if(sText == "[SYSTEM_VARIABLE]")
                {
                    isRemoteServer = false;
                    isGlobalVariables = true;
                    isMCC118Config = false;
                }
                else if(sText == "[MCC118_CHANNEL_FLAG]")
                {
                    isRemoteServer = false;
                    isGlobalVariables = false;
                    isMCC118Config = true;
                }
                else{
                    isGlobalVariables = false;
                    isRemoteServer = false;
                    isMCC118Config = false;
                }

                if(sTempText == "")
                {
                    sTempText = sText;
                }
                else{
                    sPrevTempText = sTempText;
                    sTempText = sText;
                }
                isTemp  = true;
                continue;                     
            }
            else if (sText == ""){
                iTemp = 0;
                isTemp = false; 
                isRemoteServer = false;
                isGlobalVariables = false;
                isMCC118Config = false;
                continue;
            }

            if(isTemp)
            {
                iTemp++;

                if(file.eof())
                {
                    iTemp = 0;
                    isTemp = false; 
                }

                if (isRemoteServer)
                {
                    ret = remoteServerMapping(sText);
                    if(ret > 0) break;
                }
                
                if (isGlobalVariables)
                {
                    ret = globalVariableMapping(sText);
                    if(ret > 0) break;
                }

                if (isMCC118Config)
                {
                    ret = mcc118ConfigFlagMapping(sText);
                    if(ret > 0) break;
                }
            }
        }
    }
    else
    {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "The config file does not found. The path=%s", configDir.c_str());
        writeToLog(ERRORS, buf);
        ret = 1;
    }

    return ret;
}

void initiate_GlobalVariable()
{
    string sTemp = "";
    int iTemp = 0;
    char buf[DEFAULT_BUFLEN] = {0};

    //Assign Host Port
    sTemp = findGlobalVariableMap("HOST_PORT");

    if(sTemp != "")
    {
        if(is_int(sTemp))
        {
            giHostPort = atoi(sTemp.c_str());
        }
    }

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "The Global Variable [HOST_PORT] : giHostPort=%d", giHostPort);
    writeToLog(INFO, buf);

    //Assign Truncate Log Day
    sTemp = findGlobalVariableMap("LOG_DAY");

    if(sTemp != "")
    {
        if(is_int(sTemp))
        {
            giLogDays = atoi(sTemp.c_str());
        }
    }

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "The Global Variable [LOG_DAY] : giLogDays=%d", giLogDays);
    writeToLog(INFO, buf);

    //Assign MCC118 Config Flag
    sTemp = findGlobalVariableMap("MCC118_CHANNEL_FLAG");

    if(sTemp != "")
    {
        if(is_int(sTemp))
        {
            gbMCC118ConfigFlag = atoi(sTemp.c_str()) ? true:false;
        }
    }

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "The Global Variable [MCC118_CHANNEL_FLAG] : gbMCC118ConfigFlag=%s", gbMCC118ConfigFlag ? "Enable":"Disable");
    writeToLog(INFO, buf);
}

//[REMOTE_SERVER]
//No.,RemoteName,IP,Port,Description
//1,MCC_DAQ_HOST,10.60.144.79,5000,This is MCC DAQ Host Server
int remoteServerMapping(string sText)
{
    int ret = 0;
    char* str;
    char buf[DEFAULT_BUFLEN] = {0};
    int iToken = 0;
    string sName = "";
    string sIP = "";
    int iPort = 0;

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%s", sText.c_str());
    str = strtok(buf, ",");
    
    while(str != NULL){
        iToken ++;
        switch (iToken){
        case 2:
            sName = str;
            break;
        case 3:
            sIP = str;
            break;
        case 4:
            bool isInt = is_int(str);
            if (isInt){
                iPort = atoi(str);
            }
            else{
                iPort = 0;
            }
                            
            break;
        }
        str = strtok(NULL, ",");
    }

    if(iToken > 4)
    {
        ClientServerMapList[sName] = { sName, sIP, iPort, 0, 0};
    }
    else{
        ret = 1;
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "The Remote Server Mapping List error. NAME [%s]", sName.c_str());
        writeToLog(ERRORS, buf);
    }

    return ret;
}

//[SYSTEM_VARIABLE]
//No.,Name,Value,Descriptiom
//1,HOST_PORT,8888,This is Host Port Number
int globalVariableMapping(string sText)
{
    int ret = 0;
    char* str;
    char buf[DEFAULT_BUFLEN] = {0};
    int iToken = 0;
    string sName = "";
    string sValue = "";
    string sDescription = "";

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%s", sText.c_str());
    str = strtok(buf, ",");

     while(str != NULL){
        iToken ++;
        switch (iToken){
        case 2:
            sName = str;
            break;
        case 3:
            sValue = str;
            break;
        case 4:
            sDescription = str;                         
            break;
        }
        str = strtok(NULL, ",");
    }

    if (iToken > 3)
    {
        GlobalVarList[sName] = {sName, sValue, sDescription};
    }
    else{
        ret = 1;
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "The Global Variable Mapping List error. NAME [%s]", sName.c_str());
        writeToLog(ERRORS, buf);
    }
    
    return ret;
}

//[MCC118_CHANNEL_FLAG]
//1,CHANNEL_0,0,This is channel setting flag. 0 is disable, 1 is enable
int mcc118ConfigFlagMapping(string sText)
{
    int ret = 0;
    char* str;
    char buf[DEFAULT_BUFLEN] = {0};
    int iToken = 0;
    string sName = "";
    bool bValue = false;
    string sDescription = "";

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%s", sText.c_str());
    str = strtok(buf, ",");

     while(str != NULL){
        iToken ++;
        switch (iToken){
        case 2:
            sName = str;
            break;
        case 3:
            if(is_int(str))
            {
                bValue = atoi(str) ? true:false;
            }
            break;
        case 4:
            sDescription = str;                         
            break;
        }
        str = strtok(NULL, ",");
    }

    if (iToken > 3)
    {
        MCC118ConfigList[sName] = {sName, bValue, sDescription};
    }
    else{
        ret = 1;
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "The MCC118 Config Mapping List error. NAME [%s]", sName.c_str());
        writeToLog(ERRORS, buf);
    }
    
    return ret;
}

void disRemoteServerMappingList()
{
    char buf[DEFAULT_BUFLEN] = {0};
    string sName = "";
    string sIP = "";
    int iPort = 0;
    int size = 0;
    int iStatus = 0;
    int i = 0;

    size = ClientServerMapList.size();

    if(size > 0)
    {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "The remote client connection mapping list as below: Total[%d]", size);
        writeToLog(INFO, buf);

        for (const auto& key : ClientServerMapList) {
            i ++;
            sName = key.second.name;
            sIP = key.second.ip;
            iPort = key.second.port;
            iStatus = key.second.status;
            memset(buf, 0, sizeof(buf));
            sprintf(buf, "%d. Name [%s] IP [%s] Port [%d] Status [%s]", i, sName.c_str(), sIP.c_str(), iPort, iStatus ? "Connected" : "Disconnected");
            writeToLog(INFO, buf);
        }
    }
    else
    {
        memset(buf,0,sizeof(buf));
        sprintf(buf,"The Remote Server Mapping List is Empty.");
        writeToLog(WARNING, buf);
    }
}

void disGlobalVariableMappingList()
{
    char buf[DEFAULT_BUFLEN] = {0};
    string sName = "";
    string sValue = "";
    string sDescription = "";
    int i = 0;
    int size = 0;

    size = GlobalVarList.size();

    if(size > 0)
    {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "The global variable mapping list as below: Total[%d]", size);
        writeToLog(INFO, buf);

        for (const auto& key : GlobalVarList) {
            i++;
            sName = key.second.name;
            sValue = key.second.value;
            sDescription = key.second.sDecription;
            memset(buf, 0, sizeof(buf));
            sprintf(buf, "%d. Name [%s] Value [%s] Description [%s]", i, sName.c_str(), sValue.c_str(), sDescription.c_str());
            writeToLog(INFO, buf);
        }
    }
    else
    {
        memset(buf,0,sizeof(buf));
        sprintf(buf,"The Global Variable Mapping List is Empty.");
        writeToLog(WARNING, buf);
    }
}

string findGlobalVariableMap(string key)
{
    string sValue = "";

    auto findKey = GlobalVarList.find(key);

    if (findKey != GlobalVarList.end())
    {
        sValue = findKey->second.value;
    }

    return sValue;
}
