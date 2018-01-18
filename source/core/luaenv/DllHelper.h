/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.12.15
Description:		��Lua�е���dll
*****************************************************/

#pragma once
#include "../stdafx.h"
#include "LuaAPI.h"
#include "LuaBridge.h"

extern "C" {
#include "lua.h"
#include "lualib.h"  
#include "lauxlib.h" 
}

#pragma comment(lib, "lua.lib")


class DllHelper :public LuaBridge<DllHelper> {
public:
	int LoadLibrary_LCF(lua_State* L);
};