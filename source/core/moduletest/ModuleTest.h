#pragma once
/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.11.20
Description:		单元测试
*****************************************************/
#include "..\stdafx.h"
#include "..\luaenv/LuaObject.h"
#include "..\luaenv/LuaBridge.h"
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

	double RawMemFunc(int a, int b) {
		return a + b * 2;
	}

	double RawMemFuncEx(int a, int b, double c, int d, double e, int f, double g, int h) {
		return a + b + c + d + e + f + g + h;
	}

	string m_strName;
};



namespace LuaCFunction {
	template<typename T1, typename T2, typename... Args>
	void Push(lua_State* L, T1 t1, T2 t2, Args... args) {
		Push(L, t1);
		Push(L, t2, args...);
	}

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

	template<typename T>
	std::tuple<T> _get2(lua_State* L, int index) {
		return std::make_tuple<T>(Read<T>(L, index));
	}

	template<typename T1, typename T2, typename... Args>
	std::tuple<T1, T2, Args...> _get2(lua_State* L, int index) {
		std::tuple<T1> head = std::make_tuple<T1>(Read<T1>(L, index));
		return std::tuple_cat(head, _get2<T2, Args...>(L, index + 1));
	}

	template<typename... Args>
	std::tuple<Args...> Pop(lua_State* L) {
		const int length = sizeof...(Args);
		std::tuple<Args...> retTuple = _get2<Args...>(L, -length);
		lua_pop(L, length);
		return retTuple;
	}

	template<typename... Args>
	std::tuple<Args...> Get(lua_State* L) {
		const int length = sizeof...(Args);
		std::tuple<Args...> retTuple = _get2<Args...>(L, -length);
		return retTuple;
	}

	template<typename T, typename Ret, typename... Args>
	std::function<int(lua_State*)> LambdaWrapper(lua_State* L, T* pObj, Ret(T::*mfun)(Args...)) {
		return  [pObj, mfun](lua_State* luaState)->int {
			auto paramTuple = Pop<Args...>(luaState);
			auto paramA = std::get<0>(paramTuple);
			auto paramB = std::get<1>(paramTuple);
			//如何将paramTuple内的所有参数自动拆出来？？？？
			Ret result = (pObj->*mfun)(14, 15);
			Push(luaState, result);
			return 1;
		};
	}

	template<typename... Ret, typename... Args>
	std::tuple<Ret...> CallLuaFunc(lua_State* L, const char* funcName, const Args&... args) {
		const int argCount = sizeof...(Args);
		const int retCount = sizeof...(Ret);

		lua_getglobal(L, funcName);
		Push(args...);
		lua_pcall(L, argCount, retCount, 0);
		return Pop<Ret...>();
	}

	template<typename T>
	bool PushSelf(lua_State* L, T* pObj) {

		return true;
	}

	template<typename... Ret, typename T, typename... Args>
	std::tuple<Ret...> CallLuaFunc2(lua_State* L, const char* funcName, T* pObj, const Args&... args) {
		const int argCount = sizeof...(Args) + 1;
		const int retCount = sizeof...(Ret);
		lua_getglobal(L, funcName);
		PushSelf(L, pObj);
		Push(args...);
		lua_pcall(L, argCount, retCount, 0);
		return Pop<Ret...>();
	}
}


class TestBridge :public LuaBridge<TestBridge>
{

};
void TestLuaObj_TestCode();
void TestLuaCFunction_TestCode();
