#pragma once
/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.11.20
Description:		µ¥Ôª²âÊÔ
*****************************************************/
#include "..\stdafx.h"
#include "..\luaenv/LuaObject.h"
#include "..\luaenv/LuaEnv.h"

class TestLuaObj :public LuaObject<TestLuaObj>
{
public:
	TestLuaObj(const char* name);
	static const char className[];
	static RegType methods[];

	int Minus(lua_State* L) {
		lua_pushstring(L, "call TestLuaObj member function Minus");
		return 1;
	}

	int RawMemFunc(int a, int b) {
		return a + b * 2;
	}

	string m_strName;
};

void TestLuaObj_TestCode();
