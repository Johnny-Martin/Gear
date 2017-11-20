/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.11.9
Description:		Lua����������е�λ����Module�ķ�װ
*****************************************************/

/*
һ��Lua�ļ�����һ��Module��һ������Ҳ������Ϊһ��Module
*/
#pragma once
#include "../stdafx.h"
#include "LuaAPI.h"

extern "C" {
#include "lua.h"
#include "lualib.h"  
#include "lauxlib.h" 
}

#pragma comment(lib, "lua.lib")

class LuaModule
{
public:
	LuaModule(lua_State* pLuaState, const string& filePath);
	bool Run();
private:
	lua_State*	m_pLuaState;
	string		m_luaFilePath;
	bool		m_bLoadSuccessed;
};