#include "stdafx.h"
#include "LuaModule.h"

LuaModule::LuaModule(lua_State* pLuaState, const string& filePath)
	:m_pLuaState(pLuaState), m_bLoadSuccessed(false), m_luaFilePath()
{
	m_luaFilePath = filePath;
	int iError = luaL_loadfile(pLuaState, filePath.c_str());
	if (iError == 0) {
		m_bLoadSuccessed = true;
	} else {
		m_bLoadSuccessed = false;
		ERR("LuaModule error: load lua file failed, error code: {}, file: {}", iError, filePath);
	}
}

bool LuaModule::Run()
{
	if (m_bLoadSuccessed){
		int iError = lua_pcall(m_pLuaState, 0, 0, 0);
		if (iError != 0) {
			ERR("LuaModule Run error:  error code: {}", iError);
		}
	}
	return false;
}