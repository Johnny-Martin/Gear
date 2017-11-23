/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.11.9
Description:		脚本环境,用来编译Lua文件、注册全局对象、等
*****************************************************/

/*
1，如果所有的Lua文件都共用一个LuaState运行，回掉函数等极易重名。所以我们让每个Lua文件都有一个自己的LuaState。
这样做带来一个问题：全局变量、全局函数不易共享。所以得有一个LuaEnv来管理这些luastate,处理横跨luastate的
全局变量。
2，第一版本，所有lua文件都运行在主线程，ENV单例。
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
	LuaErrorHandlerType			m_errhandler_C;//编译时的错误处理回掉
	LuaErrorHandlerType			m_errhandler_R;//运行期的错误处理回调
	static int					m_initCFunctionArraySize;

private:
	LuaEnv();
	lua_State*					m_luaState;
	map<string, lua_State*>		m_mapPathToLuaState;
	map<lua_State*, LuaModule*>	m_mapLuaStateToModule;
};