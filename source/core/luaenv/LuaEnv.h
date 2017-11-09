/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.11.9
Description:		�ű�����,��������Lua�ļ���ע��ȫ�ֶ��󡢵�
*****************************************************/

#pragma once
#include "../stdafx.h"
extern "C" {
#include "lua.h"
#include "lualib.h"  
#include "lauxlib.h" 
}

#pragma comment(lib, "lua.lib")

void DefaultErrorHandler_C(const char* errInfo);
void DefaultErrorHandler_R(const char* errInfo);

typedef void(*LuaErrorHandlerType)(const char* errInfo);
class LuaEnv
{
public:

public:
	static void SetErrorHandler_C(LuaErrorHandlerType callback);
	static void SetErrorHandler_R(LuaErrorHandlerType callback);
	static bool CompileLuaFile(const string& filePath);

private:
	static LuaErrorHandlerType m_errhandler_C;//����ʱ�Ĵ�����ص�
	static LuaErrorHandlerType m_errhandler_R;//�����ڵĴ�����ص�
};