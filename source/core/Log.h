/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.8.2
Description:		包装spdlog
*****************************************************/

#pragma once
#include "stdafx.h"
#include "spdlog.h"
class LogRapper;

#define SPDLOG_TRACE_ON

#define CONCAT_STRH(str) #str
#define CONCAT_STR(str) CONCAT_STRH(str)
#define TRC(...) LogRapper::GetInstance().m_spLogger->trace("[File: " __FILE__ " Line: " CONCAT_STR(__LINE__) "] " __VA_ARGS__)
#define DBG(...) LogRapper::GetInstance().m_spLogger->debug("[File: " __FILE__ " Line: " CONCAT_STR(__LINE__) "] " __VA_ARGS__)
#define INFO(...) LogRapper::GetInstance().m_spLogger->info("[File: " __FILE__ " Line: " CONCAT_STR(__LINE__) "] " __VA_ARGS__)
#define WARN(...) LogRapper::GetInstance().m_spLogger->warn("[File: " __FILE__ " Line: " CONCAT_STR(__LINE__) "] " __VA_ARGS__)
#define ERR(...) LogRapper::GetInstance().m_spLogger->error("[File: " __FILE__ " Line: " CONCAT_STR(__LINE__) "] " __VA_ARGS__)

#define INTERVAL(str) #str
#define PARAM_INTERVAL(param) #param INTERVAL(]:)
#define PARAM_INTERVAL_HEADER(param) INTERVAL([) PARAM_INTERVAL(param)

#define INFO_1(param) INFO(PARAM_INTERVAL(param), param)

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