/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.11.9
Description:		�ű�����,��������Lua�ļ���ע��ȫ�ֶ��󡢵�
*****************************************************/

#pragma once
#include "../stdafx.h"
#include "LuaAPI.h"

extern "C" {
#include "lua.h"
#include "lualib.h"  
#include "lauxlib.h" 
}

#pragma comment(lib, "lua.lib")

void DefaultErrorHandler_C(const char* errInfo);
void DefaultErrorHandler_R(const char* errInfo);

typedef void(*LuaErrorHandlerType)(const char* errInfo);

struct _LuaCFunctionInfo
{
	char* funcName;
	lua_CFunction func;
};

#define BEGIN_LUACFUNCTION_DECLARE()\
	private:\
	static const _LuaCFunctionInfo* _GetCFunctionArray() {\
			static const _LuaCFunctionInfo functions[] = {

#define REGISTER_CFUNCTION(fun)\
				{#fun, fun},

#define END_LUACFUNCTION_DECLARE()\
			};m_initCFunctionArraySize = sizeof(functions);\
			return functions;}

class LuaEnv
{
public:
	static LuaEnv&				GetInstance();
public:
	BEGIN_LUACFUNCTION_DECLARE()
		REGISTER_CFUNCTION(MsgBox)
		REGISTER_CFUNCTION(SpdLog)
	END_LUACFUNCTION_DECLARE()

public:
	static void					SetErrorHandler_C(LuaErrorHandlerType callback);
	static void					SetErrorHandler_R(LuaErrorHandlerType callback);
	static bool					CompileLuaFile(const string& filePath);
private:
	static void					RegisterGlobalFunctions(lua_State* pLuaStat);
private:
	static LuaErrorHandlerType  m_errhandler_C;//����ʱ�Ĵ�����ص�
	static LuaErrorHandlerType  m_errhandler_R;//�����ڵĴ�����ص�
	static int					m_initCFunctionArraySize;

private:
	LuaEnv();
	lua_State*					m_luaState;
};