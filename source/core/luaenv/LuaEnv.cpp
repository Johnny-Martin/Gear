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

//参数可以是一个文件路径，也可以是一段代码
bool LuaEnv::CompileLuaModule(const string& filePathOrCode)
{
	lua_State* pLuaState = luaL_newstate();
	int iError = luaL_loadfile(pLuaState, filePathOrCode.c_str());
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

lua_State* LuaEnv::GetLuaState(const string& filePath)
{
	auto ret = m_mapPathToLuaState.find(filePath);
	return ret == m_mapPathToLuaState.end() ? nullptr : ret->second;
}

lua_State* LuaEnv::LoadLuaModule(const string& filePath)
{
	//检查是否已被加载
	auto pos = m_mapPathToLuaState.find(filePath);
	if (pos != m_mapPathToLuaState.end()){
		WARN("Create Lua state failed! program abort");
		return false;
	}

	//创建LuaState
	auto pLuaState = luaL_newstate();
	if (pLuaState == NULL) {
		ERR("Create Lua state failed! program abort");
		abort();
	}

	m_mapPathToLuaState[filePath] = pLuaState;
	//打开库
	luaL_openlibs(pLuaState);

	//注册全局函数
	RegisterGlobalFunctions(pLuaState);

	//使用state创建LuaModule
	auto pModule = new LuaModule(pLuaState, filePath);

	m_mapLuaStateToModule[pLuaState] = pModule;
	/*int iError = luaL_dofile(pLuaState, filePathOrCode.c_str());
	if (iError != 0) {

	}*/
	return pLuaState;
}

bool LuaEnv::UnLoadLuaModule(const string& filePathOrCode)
{
	return true;
}