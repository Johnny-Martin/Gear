/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.8.2
Description:		°ü×°spdlog
*****************************************************/

#pragma once
#include "stdafx.h"
#include "spdlog.h"
class LogRapper;
#define INFO(...) LogRapper::GetInstance().Info(__VA_ARGS__)

class LogRapper
{
public:
	static LogRapper& GetInstance();
	template<typename T, typename... Args>
	void Info(const T& info, const Args& ...rest);

	template<typename T>
	LogRapper& operator << (const T& info);
private:
	LogRapper();
	std::shared_ptr<spdlog::logger> m_spLogger;
};


namespace Gear {
	
}//namespace Gear