#include "cstring"
#include <thread>
#include "Versions.h"
#include "MCC_DAQHATS.h"
#include "ServerSocket.h"
#include "ClientSocket.h"
#include "Logger.h"
#include "Global.h"

using namespace std;

#define DEFAULT_BUFLEN 1024

void Init(int& retVal)
{
    char buf[DEFAULT_BUFLEN];
    int ret = 0;

    while(true)
    {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "Start the Log File Housekeeping Function (> %d days)....", giLogDays);
        writeToLog(INFO, buf);
        ret = deleteLogFile();
        if(ret == 0)
        {
            memset(buf, 0, sizeof(buf));
            sprintf(buf, "No action for the Log FIle Housekeeping Function....");
            writeToLog(INFO, buf);
        }
        sleep(86400); //24hrs 24 * 60 * 60
    }
}

int main(void){
    char buf[DEFAULT_BUFLEN];
    int ret = 0;
    int retMCCThread, retServerThread, retTruncThread = 0;
    int iHatCount = hat_list(HAT_ID_ANY, NULL);

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "===============================================================================================================");
    writeToLog(INFO, buf);
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "The MCC DAQ HATS Program Started.");
    writeToLog(INFO, buf);
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "The Program Version [%s] MODIFY_DATE [%s]", sVersion.c_str(), sModifyDate.c_str());
    writeToLog(INFO, buf);

    ret = initiate_MappingTables();

    if(ret > 0)
    {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "The Initiate Mapping Tables Failure.");
        writeToLog(INFO, buf);
        return 0;
    }

    disRemoteServerMappingList();
    disGlobalVariableMappingList();

    initiate_GlobalVariable();

    if(iHatCount > 0)
    {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "Found the MCC DAQ HATS in the system.");
        writeToLog(INFO, buf);

        thread TRUNC_THREAD(Init, ref(retTruncThread));
        thread MCC_THREAD(MCC_DAQHATS_INIT, &iHatCount, ref(retMCCThread));
        //The thread Wait for 0.5 Second. To prevent server thread conflict.
        this_thread::sleep_for(500ms);
        thread SERVERSOCK_THREAD(serverSok, ref(retServerThread));

        TRUNC_THREAD.join();
        MCC_THREAD.join();
        SERVERSOCK_THREAD.join();

        if (retTruncThread != 0)
        {
            memset(buf, 0, sizeof(buf));
            sprintf(buf, "Init Error: %d", retTruncThread);
            writeToLog(ERRORS, buf);
        }

        if (retMCCThread != 0)
        {
            memset(buf, 0, sizeof(buf));
            sprintf(buf, "MCC HATS Error: %d", retMCCThread);
            writeToLog(ERRORS, buf);
        }

        if (retServerThread != 0)
        {
            memset(buf, 0, sizeof(buf));
            sprintf(buf, "Server Socket Error: %d", retServerThread);
            writeToLog(ERRORS, buf);
        }      
    }
    else
    {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "No Found Any HAT Device List");
        writeToLog(ERRORS, buf);
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "Please Intall the MCC118 DAQHATS board in the RaspPi5. Reboot the device and try again.");
        writeToLog(ERRORS, buf);
    }

    return 0;
}