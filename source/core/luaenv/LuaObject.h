/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.11.20
Description:		能在Lua脚本中调用C++ Obj及其的方法
*****************************************************/

#pragma once
#include "../stdafx.h"
extern "C" {
#include "lua.h"
#include "lualib.h"  
#include "lauxlib.h" 
}

#pragma comment(lib, "lua.lib")

//LuaObject的子类不能直接用来在Lua中创建对象。要在C++中创建、销毁。
//Lua中只管使用
template<class T>
class LuaObject {
public:
	void RegFuncs(lua_State* pLuaState);
	void RegAttrs(lua_State* pLuaState);
protected:
};

template<class T>
void LuaObject::RegFuncs(lua_State* pLuaState)
{

}

template<class T>
void LuaObject::RegAttrs(lua_State* pLuaState)
{

}