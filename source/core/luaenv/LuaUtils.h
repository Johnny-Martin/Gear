/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.12.6
Description:		���÷���
*****************************************************/

#pragma once
#include "../stdafx.h"
extern "C" {
#include "lua.h"
#include "lualib.h"  
#include "lauxlib.h" 
}

#pragma comment(lib, "lua.lib")

namespace LuaUtil{
	void Push(lua_State* L, int value);
}
