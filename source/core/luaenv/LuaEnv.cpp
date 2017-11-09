#include "stdafx.h"
#include "LuaEnv.h"

void DefaultErrorHandler_C(const char* errInfo)
{
	//ERR("Compile Lua file error: {}", errInfo);
	cout << errInfo << endl;
}

void DefaultErrorHandler_R(const char* errInfo)
{
	//ERR("Execute Lua file error: {}", errInfo);
	cout << errInfo << endl;
}

LuaErrorHandlerType LuaEnv::m_errhandler_C = DefaultErrorHandler_C;
LuaErrorHandlerType LuaEnv::m_errhandler_R = DefaultErrorHandler_R;

void LuaEnv::SetErrorHandler_C(LuaErrorHandlerType callback)
{
	if (m_errhandler_C){
		INFO("m_errhandler_C already exisit, will be covered");
	}
	m_errhandler_C = callback;
}

void LuaEnv::SetErrorHandler_R(LuaErrorHandlerType callback)
{
	if (m_errhandler_R) {
		INFO("m_errhandler_R already exisit, will be covered");
	}
	m_errhandler_R = callback;
}

bool LuaEnv::CompileLuaFile(const string& filePath)
{
	lua_State* pLuaState = luaL_newstate();
	int iError = luaL_loadfile(pLuaState, filePath.c_str());
	if (iError != 0){
		const char* pszErrInfo = lua_tostring(pLuaState, -1);
		if (m_errhandler_C){
			m_errhandler_C(pszErrInfo);
		}
		delete[] pszErrInfo;
	}

	lua_close(pLuaState);
	return (iError == 0);
}