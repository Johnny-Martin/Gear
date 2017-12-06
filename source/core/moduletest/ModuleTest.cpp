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

const LuaBridge<UIObjectFake>::LuaRegType UIObjectFake::methods[] = {
	{ "AddNum", &UIObjectFake::AddNum_LCF },
	{ "SetSomething", &UIObjectFake::SetSomething_LCF},
	{ 0 }
};

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
		return (int)lua_tointeger(L, index);
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
		return lua_toboolean(L, index) == 1;
	}
}

void TestLuaCFunction_TestCode()
{
	lua_State* luaState = luaL_newstate();
	int retA = lua_gettop(luaState);

	LuaCFunction::Push(luaState, false, 1006, "Strin in Lua State", 66.996);
	int retB = lua_gettop(luaState);

	bool		paramA;
	int			paramB;
	const char* paramC;
	double		paramD;
	//std::tie(paramD, paramC, paramB, paramA) = LuaCFunction::PopFromTop<double, const char*, int, bool>(luaState);
	std::tie(paramA, paramB, paramC, paramD) = LuaCFunction::Pop<bool, int, const char*, double>(luaState);

	int retC = lua_gettop(luaState);

	TestLuaObj testObj("testObj");
	auto funWrapper = LuaCFunction::LambdaWrapper(luaState, &testObj, &TestLuaObj::RawMemFunc);

	int sizeLambda		= sizeof(funWrapper);
	int sizeMemberFunc	= sizeof(&TestLuaObj::RawMemFunc);
	int sizeMemberFunc2 = sizeof(&TestLuaObj::RawMemFuncEx);
	int sizeInt			= sizeof(int);

	LuaCFunction::Push(luaState, 15, 16);
	int retD = lua_gettop(luaState);
	funWrapper(luaState);
	int retE = lua_gettop(luaState);
	std::tie(paramD) = LuaCFunction::Pop< double>(luaState);

	int retF = lua_gettop(luaState);
	int i = 0;
	++i;
}

void TestClassName()
{
	//BaseClass* pBase = new DrivedClass();
	auto ret =  BaseClass::GetClassName();
	auto ret2 = DrivedClass::GetClassName();

	auto ret3 = typeid(BaseClass).name();
	auto ret4 = typeid(DrivedClass).name();

	//auto ret5 = BaseClassT::GetClassName();
	auto ret6 = DrivedClassT::GetClassName();
	auto ret7 = DDrivedClass::GetClassName();

	auto ret8 = DrivedClassT().m_finalClassName;
	auto ret9 = DDrivedClass().m_finalClassName;

	DrivedClassT* p = new DDrivedClass();
	auto ret10 = typeid(*p).name();
	int i = 0;
	i++;
}

void TestLuaBridge()
{
	UIObjectFake* fakeUIObj = new UIObjectFake();
	const char* pPath = "D:\\Code\\temp\\Gear\\docs\\ModuleTest.lua";
	lua_State* luaState = LuaEnv::GetInstance().GetLuaState(pPath);
	luaState = luaState ? luaState : LuaEnv::GetInstance().LoadLuaModule(pPath);

	auto size1 = lua_gettop(luaState);
	assert(luaState);
	const char* szRet = nullptr;
	int i = 0;
	
	//若想CallLuaFunc返回指定类型的返回值，必须手动实例化，指定CallLuaFunc模板的所有参数类型
	szRet = fakeUIObj->CallLuaFunc<const char*, int, int>(luaState, "TestLuaBridgeFuncA", 50, 20);
	fakeUIObj->CallLuaFunc(luaState, "TestLuaBridgeFuncA", 15, 20);

	//fakeUIObj->RegisterGlobal(luaState, "fakeUIObj");
	//lua_getglobal(luaState, "TestLuaGlobalObj");
	//if (lua_isnil(luaState, -1)) {
	//	lua_pop(luaState, 1);
	//	ERR("UIEvent Fire error: can not find lua function, name: TestLuaGlobalObj");
	//	return;
	//}
	//int ret = lua_pcall(luaState, 0, 1, 0);
	//if (ret != 0) {
	//	const char* error = lua_tostring(luaState, -1);//打印错误结果 
	//	ERR("UIEvent Fire error: script run error: {}", error);
	//}

	//delete pObjA;

	//lua_getglobal(luaState, "TestLuaGlobalObj");
	//ret = lua_pcall(luaState, 0, 1, 0);
	//if (ret != 0) {
	//	const char* error = lua_tostring(luaState, -1);//打印错误结果 
	//	ERR("UIEvent Fire error: script run error: {}", error);
	//}

	auto size2 = lua_gettop(luaState);
	if (szRet){
		i += 3;
	} else {
		i += 4;
	}
}