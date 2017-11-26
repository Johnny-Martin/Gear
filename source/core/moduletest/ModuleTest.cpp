#include "stdafx.h"
#include "ModuleTest.h"

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
}
