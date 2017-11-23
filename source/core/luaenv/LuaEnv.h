/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.11.9
Description:		�ű�����,��������Lua�ļ���ע��ȫ�ֶ��󡢵�
*****************************************************/

/*
1��������е�Lua�ļ�������һ��LuaState���У��ص������ȼ�������������������ÿ��Lua�ļ�����һ���Լ���LuaState��
����������һ�����⣺ȫ�ֱ�����ȫ�ֺ������׹������Ե���һ��LuaEnv��������Щluastate,������luastate��
ȫ�ֱ�����
2����һ�汾������lua�ļ������������̣߳�ENV������
*/
#pragma once
#include "../stdafx.h"
#include "LuaAPI.h"
#include "LuaModule.h"

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
	void						SetErrorHandler_C(LuaErrorHandlerType callback);
	void						SetErrorHandler_R(LuaErrorHandlerType callback);
	bool						CompileLuaModule(const string& filePathOrCode);
	lua_State*					GetLuaState(const string& filePath);
	lua_State*					LoadLuaModule(const string& filePathOrCode);
	bool						UnLoadLuaModule(const string& filePathOrCode);
private:
	void						RegisterGlobalFunctions(lua_State* pLuaStat);
private:
	LuaErrorHandlerType			m_errhandler_C;//����ʱ�Ĵ�����ص�
	LuaErrorHandlerType			m_errhandler_R;//�����ڵĴ�����ص�
	static int					m_initCFunctionArraySize;

private:
	LuaEnv();
	lua_State*					m_luaState;
	map<string, lua_State*>		m_mapPathToLuaState;
	map<lua_State*, LuaModule*>	m_mapLuaStateToModule;
};