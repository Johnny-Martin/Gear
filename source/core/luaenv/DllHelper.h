/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.12.15
Description:		在Lua中调用dll
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