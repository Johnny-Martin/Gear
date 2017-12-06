/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.11.9
Description:		需要注册到Lua中的API
*****************************************************/

#pragma once
#include "../stdafx.h"
extern "C" {
#include "lua.h"
#include "lualib.h"  
#include "lauxlib.h" 
}

#pragma comment(lib, "lua.lib")

int MsgBox(lua_State* luaState);
int SpdLog(lua_State* luaState);
int SetGlobal(lua_State* luaState);
int GetGlobal(lua_State* luaState);
