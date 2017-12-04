/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.11.30
Description:		能在Lua脚本中调用C++ Obj及其成员方法，同时能调用Lua内的函数并返回多个值

相比LuaObject的改进：
1，支持调用Lua函数、返回多个值
2，使用lambda将原生成员方法包装成Lua_CFunction
*****************************************************/

#pragma once
#include "../stdafx.h"
extern "C" {
#include "lua.h"
#include "lualib.h"  
#include "lauxlib.h" 
}

#pragma comment(lib, "lua.lib")


template<std::size_t... S>
struct Indices {};

template<std::size_t N, std::size_t... S>
struct IndicesBuilder :IndicesBuilder<N - 1, N - 1, S...> {};

template<std::size_t... S>
struct IndicesBuilder<0, S...> { typedef Indices<S...> type;};

template<std::size_t... S, typename T, typename Ret, typename... Args>
Ret CallMemberFunction(Indices<S...> indices, T* pObj, Ret(T::*mfunc)(Args...), std::tuple<Args...> paramTuple)
{
	return (pObj->*mfunc)(std::get<S>(paramTuple)...);
}

#define LUABRIDGE_DEFINE_MEMBER_FUNCTION(className, funcName)  \
int funcName(lua_State* L){return (LambdaWrapper(L, this, &className::funcName))(L);}


//LuaBridge的子类不能直接用来在Lua中创建对象。要在C++中创建、销毁。
//Lua中只管使用。DrivedClass需要提供两个内容：className跟要注册的函数信息(methods)
template<typename DrivedClass>
class LuaBridge {
public:
																	LuaBridge();
	virtual															~LuaBridge();
	bool															RegisterGlobal(lua_State* L, const char* szName);
	template<typename... Ret, typename... Args> std::tuple<Ret...>	CallLuaFunc(lua_State*, const char* szFuncName, Args... args);

private:
	lua_State*														m_pLuaState;
	string															m_strGlobalName;
	DrivedClass**													m_userData;

public:
	template<typename T1, typename T2, typename... Args> void		Push(lua_State* L, T1 t1, T2 t2, Args... args)	{ Push(L, t1); Push(L, t2, args...); }
	void															Push(lua_State* L, const int& value)			{ lua_pushinteger(L, value); }
	void															Push(lua_State* L, const long long& value)		{ lua_pushstring(L, value); }
	void															Push(lua_State* L, const double& value)			{ lua_pushnumber(L, value); }
	void															Push(lua_State* L, const float& value)			{ lua_pushnumber(L, value); }
	void															Push(lua_State* L, const bool& value)			{ lua_pushboolean(L, value); }
	void															Push(lua_State* L, const char* value)			{ lua_pushstring(L, value); }
	void															Push(lua_State* L, const string& value)			{ lua_pushstring(L, value.c_str()); }
	void															Push(lua_State* L, DrivedClass* pObj);
	bool															CheckLuaState(lua_State* L);
	void															RegisterMethods(lua_State* L);

	//占位用模板成员函数，实际使用的永远是该模板函数的特化(模板类的模板成员函数无法在类外特化)
	template<typename T> T											Read(lua_State* L, int index);
	template<> int													Read(lua_State* L, int index)					{ return (int)lua_tointeger(L, index); }
	template<> long long											Read(lua_State* L, int index)					{ return lua_tointeger(L, index); }
	template<> bool													Read(lua_State* L, int index)					{ return lua_toboolean(L, index) == 1; }
	template<> double												Read(lua_State* L, int index)					{ return lua_tonumber(L, index); }
	template<> float												Read(lua_State* L, int index)					{ return (float)lua_tonumber(L, index); }
	template<> const char*											Read(lua_State* L, int index)					{ return lua_tostring(L, index); }
	template<typename T> std::tuple<T>								ReadToTuple(lua_State* L, int index)			{ return std::make_tuple(Read<T>(L, index)); }
	template<class T1,class T2,class... Args> tuple<T1,T2,Args...>	ReadToTuple(lua_State* L, int index)			{ return std::tuple_cat(std::make_tuple(Read<T1>(L, index)), ReadToTuple<T2, Args...>(L, index + 1)); }
	template<typename... Args> std::tuple<Args...>					Get(lua_State* L)								{ return ReadToTuple<Args...>(L, -(sizeof...(Args))); }
	template<typename... Args> std::tuple<Args...>					Pop(lua_State* L)								{ auto ret = Get<Args...>(L); lua_pop(L, sizeof...(Args)); return ret; }

protected:
	typedef int (DrivedClass::*mfp)(lua_State *L);
	typedef struct { const char *name; mfp mfunc; }					LuaRegType;
};

////////////////////////////////////////////////////////////////////////////////////////
template<class DrivedClass>
LuaBridge<DrivedClass>::LuaBridge() :m_pLuaState(nullptr), m_strGlobalName(), m_userData(nullptr)
{

}

template<class DrivedClass>
LuaBridge<DrivedClass>::~LuaBridge()
{
	//重置DrivedClass::className元表mt中this指针位置的UserData里的对象指针
	if (!m_pLuaState) {
		return;
	}
	const char* szName = typeid(DrivedClass).name();
	luaL_getmetatable(m_pLuaState, szName);
	assert(!lua_isnil(m_pLuaState, -1));
	lua_pushlightuserdata(m_pLuaState, this);
	lua_rawget(m_pLuaState, -2);
	if (luaL_testudata(m_pLuaState, -1, szName)) {
		//重置T::className元表mt中this指针位置的UserData里的对象指针
		DrivedClass** ppT = (DrivedClass**)lua_topointer(m_pLuaState, -1);
		*ppT = nullptr;
	}

	lua_pop(m_pLuaState, 1);
	lua_pushlightuserdata(m_pLuaState, this);
	lua_pushnil(m_pLuaState);
	lua_rawset(m_pLuaState, -3);//将mt[this] = nil,减少UserData的引用，以便GC回收
	lua_pop(m_pLuaState, 1);

	//若曾经设置过全局对象，则需清除全局对象对UserData的引用，以便GC回收
	if (m_strGlobalName.empty()) {
		return;
	}

	lua_getglobal(m_pLuaState, m_strGlobalName.c_str());
	//该全局对象已被Lua代码清除、或重置为一个非userdata对象
	if (!luaL_testudata(m_pLuaState, -1, szName)) {
		lua_pop(m_pLuaState, 1);
		return;
	}

	DrivedClass** ppT = (DrivedClass**)lua_topointer(m_pLuaState, -1);
	//该全局对象已被Lua代码更改
	if (ppT != m_userData) {
		lua_pop(m_pLuaState, 1);
		return;
	}

	lua_pop(m_pLuaState, 1);
	lua_pushnil(m_pLuaState);
	lua_setglobal(m_pLuaState, m_strGlobalName.c_str());
}

template<typename DrivedClass>
bool LuaBridge<DrivedClass>::RegisterGlobal(lua_State* L, const char* szName)
{
	CheckLuaState(L);

	lua_getglobal(L, szName);
	if (!lua_isnil(L, -1)) {
		//已经有同名的全局标识符,检查是不是自身
		if (luaL_testudata(L, -1, typeid(DrivedClass).name())) {
			DrivedClass** ppT = (DrivedClass**)lua_topointer(L, -1);
			if (*ppT == static_cast<DrivedClass*>(this)) {
				lua_pop(L, 1);
				WARN("RegisterGlobal warning: LuaObject already registered");
				return true;
			}
		}
		lua_pop(L, 1);
		ERR("RegisterGlobal error: different global object using the same name already exisit, name: {}", szName);
		return false;
	}
	lua_pop(L, 1);

	Push(L, static_cast<DrivedClass*>(this));
	lua_setglobal(L, szName);
	m_strGlobalName = szName;
	return true;
}

template<typename DrivedClass>
void LuaBridge<DrivedClass>::RegisterMethods(lua_State* L)
{
	lua_newtable(L);
	int methods = lua_gettop(L);

	luaL_newmetatable(L, typeid(DrivedClass).name());
	int metatable = lua_gettop(L);

	//隐藏userdata的实质的元表，也就是说在Lua中
	//调用getmetatable(self)得到的是self本身，而不是metatable table
	lua_pushvalue(L, methods);
	set(L, metatable, "__metatable");

	lua_pushvalue(L, methods);
	set(L, metatable, "__index");

	for (int i = 0; DrivedClass::methods[i].name; ++i)
	{
		lua_pushstring(L, DrivedClass::methods[i].name);
		lua_pushnumber(L, i);
		lua_pushcclosure(L, thunk, 1);
		lua_settable(L, methods);
	}

	lua_pop(L, 2);
}

template<typename DrivedClass> template<typename... Ret, typename... Args>
std::tuple<Ret...>	LuaBridge<DrivedClass>::CallLuaFunc(lua_State*, const char* szFuncName, Args... args)
{
	const int argCount = sizeof...(Args);
	const int retCount = sizeof...(Ret);
	lua_getglobal(L, szFuncName);
	Push<Args...>(args...);
	lua_pcall(L, argCount, retCount, 0);
	return Pop<Ret...>();
}

template<typename DrivedClass>
bool LuaBridge<DrivedClass>::CheckLuaState(lua_State* L)
{
	if (!m_pLuaState) {
		m_pLuaState = L;
		return true;
	} else if (m_pLuaState != L) {
		//一个LuaObject应该自始至终运行在同一个LuaState
		ERR("LuaObject CheckLuaState error: LuaObject should not run across luaState");
		luaL_error(L, "LuaObject should not run across different LuaState");
		return false;
	}
	return true;
}

//将pObj以UserData的形式压入栈中
template<typename DrivedClass>
void LuaBridge<DrivedClass>::Push(lua_State* L, DrivedClass* pObj)
{
	CheckLuaState(L);
	const char* szClassName = typeid(DrivedClass).name;
	//改进版PushSelf,将类的对象对应的UserData保存到 ClassName元表中，以this指针为key。
	luaL_getmetatable(L, szClassName);
	if (lua_isnil(L, -1)) {
		lua_pop(L, 1);
		RegisterMethods(L);
		luaL_getmetatable(L, szClassName);
	}
	assert(!lua_isnil(L, -1));

	//以this指针为key，在metatable中检索userdata
	lua_pushlightuserdata(L, this);
	lua_rawget(L, -2);
	if (!luaL_testudata(L, -1, szClassName)) {
		//该对象尚未创建userdata——即尚未注册到lua
		lua_pop(L, 1);
		lua_pushlightuserdata(L, this);
		DrivedClassT** ppT = (DrivedClass**)lua_newuserdata(L, sizeof(T**));
		m_userData = ppT;
		*ppT = static_cast<DrivedClass*>(this);

		lua_pushvalue(L, -1);
		lua_insert(L, -4);
		lua_rawset(L, -3);

		lua_setmetatable(L, -2);
	} else {
		lua_insert(L, -2);
		lua_pop(L, 1);
	}
}

//对于C++的普通成员函数(相对于Lua_CFunction来讲)，返回值要么1个，要么没有(void)
//模板类的成员模板函数不支持偏特化，故此处将LambdaWrapper定义在全局
template<typename T, typename Ret, typename... Args>
std::function<int(lua_State*)> LambdaWrapper(lua_State* L, T* pObj, Ret(T::*mfun)(Args...)) {
	return  [pObj, mfun](lua_State* luaState)->int {
		//将tuple变成一个parameter pack
		Ret result = CallMemberFunction(
			typename IndicesBuilder<sizeof...(Args)>::type(),
			pObj, 
			mfun, 
			pObj->Pop<Args...>(luaState));

		pObj->Push(luaState, result);
		return 1;
	};
}

//无返回值的原生成员方法使用改特化生成Wrapper
template<typename T, typename... Args>
std::function<int(lua_State*)> LambdaWrapper(lua_State* L, T* pObj, void(T::*mfun)(Args...)) {
	return  [pObj, mfun](lua_State* luaState)->int {
		CallMemberFunction(
			IndicesBuilder<sizeof...(Args)>::type(),
			pObj,
			mfun,
			Pop<Args...>(luaState));

		return 0;
	};
}

template<typename T, T> struct LambdaWrapperProxy;

template<typename T, typename Ret, typename... Args, Ret(T::*ptmf)(Args...)>
struct LambdaWrapperProxy<Ret(T::*)(Args...), ptmf>
{
	static std::function<int(lua_State*)> LambdaWrapper(T* pObj, Ret(T::*ptmf)(Args...))
	{
		return [pObj, ptmf](ua_State* luaState)->int {
			CallMemberFunction(
				typename IndicesBuilder<sizeof...(Args)>::type(),
				pObj,
				ptmf,
				pObj->Pop<Args...>(luaState));

			pObj->Push(luaState, result);
			return 1;
		};
	}
};

