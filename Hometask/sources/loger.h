#pragma once
#include <fstream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <thread>

#define LOG_ERROR(msg) logError(msg, __FILE__, __LINE__)

using namespace std;

inline void logError(const string& message, const char* file, int line)
{
	ofstream logFile("app.log", ios::app);
	if(!logFile.is_open()) return;

	auto now = chrono::system_clock::now();
	auto time = chrono::system_clock::to_time_t(now);
	tm tm_buf;

	localtime_r(&time, &tm_buf);

	char timeStr[20];
	strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &tm_buf);

	string type = "ERROR";

	ostringstream threadId;
	threadId << this_thread::get_id();

	logFile << timeStr << " " << type << " " << threadId.str() << " " << message << " "<< file << " :" << line << endl;



}