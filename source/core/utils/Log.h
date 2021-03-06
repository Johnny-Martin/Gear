/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.8.2
Description:		包装spdlog
*****************************************************/

#pragma once
#include "../stdafx.h"
#include "spdlog.h"
class LogRapper;

#define SPDLOG_TRACE_ON

#define CONCAT_STRH(str) #str
#define CONCAT_STR(str) CONCAT_STRH(str)

//__VA_ARGS__中的第一个参数必须是const char *
#define TRC(format, ...) LogRapper::GetInstance().m_spLogger->trace(format" [File: " __FILE__ " Line: " CONCAT_STR(__LINE__)"]", __VA_ARGS__)
#define DBG(format, ...) LogRapper::GetInstance().m_spLogger->debug(format" [File: " __FILE__ " Line: " CONCAT_STR(__LINE__)"]", __VA_ARGS__)
#define INFO(format, ...) LogRapper::GetInstance().m_spLogger->info(format" [File: " __FILE__ " Line: " CONCAT_STR(__LINE__)"]", __VA_ARGS__)
#define INFO2(format, ...) LogRapper::GetInstance().m_spLogger->info(format, __VA_ARGS__)
#define WARN(format, ...) LogRapper::GetInstance().m_spLogger->warn(format" [File: " __FILE__ " Line: " CONCAT_STR(__LINE__)"]", __VA_ARGS__)
#define ERR(format, ...) LogRapper::GetInstance().m_spLogger->error(format" [File: " __FILE__ " Line: " CONCAT_STR(__LINE__)"]", __VA_ARGS__)


class LogRapper
{
public:
	static LogRapper& GetInstance();
	template<typename T, typename... Args>
	void Info(const T& info, const Args& ...rest)
	{
		m_spLogger->info(info);
		Info(rest...);
		return;
	}

	template<typename T>
	void Info(const T& info)
	{
		m_spLogger->info(info);
		return;
	}

	template<typename T>
	LogRapper& operator << (const T& info)
	{
		return this;
	}
public:
	std::shared_ptr<spdlog::logger> m_spLogger;
private:
	LogRapper();
};


namespace Gear {
	
}//namespace Gear