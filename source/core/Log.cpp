#include "stdafx.h"
#include "Log.h"
#include "ostream"

using namespace std;
using namespace spdlog;

LogRapper::LogRapper()
{
	//init spdlog
	spdlog::set_level(spdlog::level::trace);
	spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e][thread %t][%l] %v");
	string filePath = "D:\\test_spdlog.log";
	if (PathFileExistsA(filePath.c_str())) {
		DeleteFileA(filePath.c_str());
	}
	ofstream outFile;
	outFile.open(filePath, ios::out | ios::app | ios::binary);
	outFile.close();

	m_spLogger = spdlog::basic_logger_mt("TestLog", filePath);
	//LOG->trace("Hello spdlog");

	//LOG->flush();
}
LogRapper& LogRapper::GetInstance()
{
	static LogRapper logger{};
	return logger;
}