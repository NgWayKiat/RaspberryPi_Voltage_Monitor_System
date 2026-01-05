#include "Logger.h"

using namespace std;
namespace fs = filesystem;

const int FileSizeLimit = FILE_SIZE;

string currentDirectory = "";
string logDir = "";

void checkLogDirectory()
{
	#if isRelease
		currentDirectory = "/BIO/Program/" + sProgramName;
		logDir = currentDirectory + "/log";
	#else
		currentDirectory = fs::current_path().generic_string();
		logDir = currentDirectory + "/log";
	#endif
	char buf[DEFAULT_BUFLEN] = {0};

	if (!fs::exists(logDir))
	{
		fs::create_directory(logDir);

		memset(buf, 0, sizeof(buf));
		sprintf(buf, "sudo chmod 777 %s",logDir.c_str());
		system(buf);
	}
}

int deleteLogFile()
{
	int ret = 0;
	ofstream file;
	time_t now = time(0);
	char buf[DEFAULT_BUFLEN] = {0};
	char tmBuf[DEFAULT_BUFLEN] = {0};
	checkLogDirectory();
	struct tm curTmStruct = *localtime(&now);
	time_t curTime = mktime(&curTmStruct);

	for (const auto& entry : fs::recursive_directory_iterator(logDir)) {
		struct stat t_stat;
		struct tm * timeinfo;
		string filepath = "";
		time_t tmpTime;

		filepath = entry.path().string();		
		stat(filepath.c_str(), &t_stat);
		timeinfo = localtime(&t_stat.st_ctime);
		tmpTime = mktime(timeinfo);
		memset(tmBuf, 0, sizeof(tmBuf));
		strftime(tmBuf, sizeof(tmBuf), "%Y-%m-%d %H:%M:%S", timeinfo);
        string formatted_time(tmBuf);

		double difference = difftime(curTime, tmpTime); //the different time in seconds
		double min = difference / 60;
		double hrs = min / 60;
		int days = hrs / 24;

		if(days > giLogDays)
		{
			if(remove(filepath.c_str())==0)
			{
				ret = 1;
				memset(buf, 0, sizeof(buf));
				sprintf(buf, "Truncate Log File Name[%s] CreationDate[%s] is successfully.", entry.path().filename().string().c_str(), formatted_time.c_str());
				writeToLog(INFO, buf);
			}
		}
	}

	return ret;
}

void writeToLog(LogLevel level, char* message)
{
	ofstream file;
	char buf[DEFAULT_BUFLEN] = {0};
	bool isOpen = false;
	string sType = "INFO";
	checkLogDirectory();

	string avaFileName = getAvailableFileName();
	string logFilePath = logDir + "/" + avaFileName + ".txt";
	string dateTime = currentDateTimeInString();
	
	memset(buf, 0, sizeof(buf));

	switch (level)
	{
	case ERRORS:
		sType = "ERROR";
		break;
	case DEBUG:
		sType = "ERROR";
		break;
	case WARNING:
		sType = "WARNING";
		break;
	case CRITICAL:
		sType = "CRITICAL";
		break;
	default:
		break;
	}
	
	sprintf(buf, "%s [%s] %s", dateTime.c_str(), sType.c_str(), message);
	
	//ios::out is set to write mode
	//ios::app is append the data in end of line of the file
	file.open(logFilePath, ios::out | ios::app);

	if (file.is_open())
	{
		cout << buf << endl;
		//printf("%s \r\n", buf);
		file << buf << endl;
		fflush(stdout);
	}		

	file.close();
}

string getAvailableFileName()
{
	long fileSizeB;
	double fileSizeKb;
	int fileNo = 0;
	bool isAvaFile = false;
	string fileNameRN;
	stringstream ss;
	string tempLogFilePath;
	string tempFileName;
	string defFileNameWithRN = "log_" + currentDateLogFileNameInString() + "_00";
	string defFileName = "log_" + currentDateLogFileNameInString();
	string defLogFilePath = logDir + "/" + defFileNameWithRN + ".txt";
	string avaFileName;

	if (fs::exists(defLogFilePath))
	{
		fileSizeB = static_cast<long>(fs::file_size(defLogFilePath));
		fileSizeKb = static_cast<double>(fileSizeB) / 1024.0;

		if (fileSizeKb > FileSizeLimit)
		{
			for (int i = 1; i < INT16_MAX; i++)
			{
				ss.str("");
				ss.clear();
				ss << setw(2) << setfill('0') << i;
				fileNameRN = ss.str();
				tempFileName = defFileName + "_" + fileNameRN;
				tempLogFilePath = logDir + "/" + tempFileName + ".txt";

				if (fs::exists(tempLogFilePath))
				{
					fileSizeB = static_cast<long>(fs::file_size(tempLogFilePath));
					fileSizeKb = static_cast<double>(fileSizeB) / 1024.0;

					if (fileSizeKb < FileSizeLimit)
					{
						avaFileName = tempFileName;
						isAvaFile = true;
						break;
					}
				}
				else
				{
					avaFileName = tempFileName;
					break;
				}
			}
		}
		else
		{
			avaFileName = defFileNameWithRN;
		}
	}
	else
	{
		avaFileName = defFileNameWithRN;
	}

	return avaFileName;
}