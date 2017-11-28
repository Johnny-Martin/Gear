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



namespace LuaCFunction {
	template<typename T, typename... Args>
	void Push(lua_State* L, T t, Args... args) {
		Push(L, t);
		Push(L, args...);
	}
	//template<>
	//void Push(lua_State* L, const long long& value);
	////template<>
	//void Push(lua_State* L, const bool& value);
	////template<>
	//void Push(lua_State* L, const double& value);
	////template<>
	//void Push(lua_State* L, const char* value);

	template<typename T>
	T Read(lua_State* L, int index);

	template<typename T>
	std::tuple<T> _get(lua_State* L, int index) {
		return std::make_tuple<T>(Read<T>(L, index));
	}

	template<typename T1, typename T2, typename... Args>
	std::tuple<T1, T2, Args...> _get(lua_State* L, int index) {
		std::tuple<T1> head = std::make_tuple<T1>(Read<T1>(L, index));
		return std::tuple_cat(head, _get<T2, Args...>(L, index - 1));
	}

	template<typename... Args>
	std::tuple<Args...> PopFromTop(lua_State* L) {
		std::tuple<Args...> retTuple = _get<Args...>(L, -1);
		lua_pop(L, sizeof...(Args));
		return retTuple;
	}

	template<typename... Args>
	std::tuple<Args...> GetFromTop(lua_State* L) {
		std::tuple<Args...> retTuple = _get<Args...>(L, -1);
		return retTuple;
	}

}
void TestLuaObj_TestCode();
void TestLuaCFunction_TestCode();
