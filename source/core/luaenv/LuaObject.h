/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.11.20
Description:		����Lua�ű��е���C++ Obj����ķ���
*****************************************************/

#pragma once
#include "../stdafx.h"
extern "C" {
#include "lua.h"
#include "lualib.h"  
#include "lauxlib.h" 
}

#pragma comment(lib, "lua.lib")

//LuaObject�����಻��ֱ��������Lua�д�������Ҫ��C++�д��������١�
//Lua��ֻ��ʹ��
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