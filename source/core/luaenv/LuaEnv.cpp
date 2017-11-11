#include "stdafx.h"
#include "LuaEnv.h"

int LuaEnv::m_initCFunctionArraySize = 0;

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

LuaEnv& LuaEnv::GetInstance()
{
	static LuaEnv mainEnv;
	return mainEnv;
}

LuaEnv::LuaEnv()
{
	m_luaState = luaL_newstate();
	if (m_luaState == NULL) {
		ERR("Create Lua state failed! program abort");
		abort();
	}
		
	luaL_openlibs(m_luaState);
	RegisterGlobalFunctions(m_luaState);
}
void LuaEnv::RegisterGlobalFunctions(lua_State* pLuaStat)
{
	const _LuaCFunctionInfo* pArray = LuaEnv::_GetCFunctionArray();
	int iArrayLen = m_initCFunctionArraySize / sizeof(_LuaCFunctionInfo);
	for (int i = 0; i < iArrayLen; ++i){
		//Register the cpp function into lua
		lua_pushcfunction(pLuaStat, pArray[i].func);
		//Bind the global name to the function
		lua_setglobal(pLuaStat, pArray[i].funcName);
	}
}

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
		lua_pop(pLuaState, 1);
		if (m_errhandler_C){
			m_errhandler_C(pszErrInfo);
		}
	}

	lua_close(pLuaState);
	return (iError == 0);
}