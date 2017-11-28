#include "stdafx.h"
#include "ModuleTest.h"

//namespace LCF = LuaCFunction;

const char TestLuaObj::className[] = "TestLuaObject";
TestLuaObj::RegType TestLuaObj::methods[] = {
	{ "Minus", &TestLuaObj::Minus },
	{ 0 }
};

TestLuaObj::TestLuaObj(const char* name):m_strName(name)
{
}

void TestLuaObj_TestCode()
{
	TestLuaObj* pObjA = new TestLuaObj("TestLuaObj_ObjA");
	const char* pPath = "D:\\Code\\Gear\\docs\\ModuleTest.lua";
	lua_State* luaState = LuaEnv::GetInstance().GetLuaState(pPath);
	luaState = luaState ? luaState : LuaEnv::GetInstance().LoadLuaModule(pPath);

	assert(luaState);
	pObjA->RegisterGlobal(luaState, pObjA->m_strName.c_str());
	lua_getglobal(luaState, "TestLuaGlobalObj");
	if (lua_isnil(luaState, -1)) {
		lua_pop(luaState, 1);
		ERR("UIEvent Fire error: can not find lua function, name: TestLuaGlobalObj");
		return;
	}
	int ret = lua_pcall(luaState, 0, 1, 0);
	if (ret != 0) {
		const char* error = lua_tostring(luaState, -1);//打印错误结果 
		ERR("UIEvent Fire error: script run error: {}", error);
	}

	delete pObjA;

	lua_getglobal(luaState, "TestLuaGlobalObj");
	ret = lua_pcall(luaState, 0, 1, 0);
	if (ret != 0) {
		const char* error = lua_tostring(luaState, -1);//打印错误结果 
		ERR("UIEvent Fire error: script run error: {}", error);
	}

	lua_getglobal(luaState, "TestLuaGlobalObj");
	ret = lua_pcall(luaState, 0, 1, 0);
	if (ret != 0) {
		const char* error = lua_tostring(luaState, -1);//打印错误结果 
		ERR("UIEvent Fire error: script run error: {}", error);
	}
}

namespace LuaCFunction {
	void Push(lua_State* L) {}
	void Push(lua_State* L, const long long& value) {
		lua_pushinteger(L, value);
	}
	void Push(lua_State* L, const int& value) {
		lua_pushinteger(L, value);
	}
	void Push(lua_State* L, const bool& value) {
		lua_pushboolean(L, value);
	}
	void Push(lua_State* L, const double& value) {
		lua_pushnumber(L, value);
	}
	void Push(lua_State* L, const char* value) {
		lua_pushstring(L, value);
	}

	template<>
	int Read(lua_State* L, int index) {
		return lua_tointeger(L, index);
	}
	template<>
	long long Read(lua_State* L, int index) {
		return lua_tointeger(L, index);
	}
	template<>
	double Read(lua_State* L, int index) {
		return lua_tonumber(L, index);
	}
	template<>
	const char* Read(lua_State* L, int index) {
		return lua_tostring(L, index);
	}
	template<>
	bool  Read(lua_State* L, int index) {
		return lua_toboolean(L, index);
	}
}

void TestLuaCFunction_TestCode()
{
	//const char* pPath = "D:\\Code\\Gear\\docs\\LuaCFunction.lua";
	//lua_State* luaState = LuaEnv::GetInstance().GetLuaState(pPath);
	//luaState = luaState ? luaState : LuaEnv::GetInstance().LoadLuaModule(pPath);

	lua_State* luaState = luaL_newstate();
	int retA = lua_gettop(luaState);

	LuaCFunction::Push(luaState, false);
	LuaCFunction::Push(luaState, 1006);
	LuaCFunction::Push(luaState, "Strin in Lua State");
	LuaCFunction::Push(luaState, 66.996);
	int retB = lua_gettop(luaState);

	bool		paramA;
	int			paramB;
	const char* paramC;
	double		paramD;
	std::tie(paramD, paramC, paramB, paramA) = LuaCFunction::PopFromTop<double, const char*, int, bool>(luaState);

	int retC = lua_gettop(luaState);
	int i = 0;
	++i;
}