/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.11.20
Description:		能在Lua脚本中调用C++ Obj及其的方法

缺点：1，T中的成员函数得是LuaCfunction. 需要给C++风格的原生方法重写一个wrapper
2,不支持在Lua中直接访问obj的成员属性
后续版本使用template生成LuaCfunction格式的wrapper、支持访问属性
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
//Lua中只管使用。T需要提供两个内容：className跟要注册的函数信息(methods)
template<class T>
class LuaObject
{
public:
							LuaObject();
							~LuaObject();
	void					PushSelf(lua_State* luaState);
	bool					RegisterGlobal(lua_State* luaState, const char* name);
protected:
	typedef int (T::*mfp)(lua_State *L);
	typedef struct { const char *name; mfp mfunc; } RegType;
private:
	bool					CheckLuaState(lua_State* L);
	void					RegisterMethods(lua_State* luaState);
	static int				thunk(lua_State *L);
	static void				set(lua_State *L, int table_index, const char *key);
	lua_State*				m_pLuaState;
	string					m_strGlobalName;
	T**						m_userData;
};

template<class T>
LuaObject<T>::LuaObject():m_pLuaState(nullptr), m_strGlobalName(), m_userData(nullptr)
{

}

template<class T>
LuaObject<T>::~LuaObject()
{
	if (!m_pLuaState){
		return;
	}
	
	luaL_getmetatable(m_pLuaState, T::className);
	assert(!lua_isnil(m_pLuaState, -1));
	lua_pushlightuserdata(m_pLuaState, this);
	lua_rawget(m_pLuaState, -2);
	if (luaL_testudata(m_pLuaState, -1, T::className)) {
		//重置T::className元表mt中this指针位置的UserData里的对象指针
		T** ppT = (T**)lua_topointer(m_pLuaState, -1);
		*ppT = nullptr;
	}

	lua_pop(m_pLuaState, 1);
	lua_pushlightuserdata(m_pLuaState, this);
	lua_pushnil(m_pLuaState);
	lua_rawset(m_pLuaState, -3);//将mt[this] = nil,减少UserData的引用，以便GC回收
	lua_pop(m_pLuaState, 1);

	//若曾经设置过全局对象，则需清除全局对象对UserData的引用，以便GC回收
	if (m_strGlobalName.empty()){
		return;
	}

	lua_getglobal(m_pLuaState, m_strGlobalName.c_str());
	//该全局对象已被Lua代码清除、或重置为一个非userdata对象
	if (!luaL_testudata(m_pLuaState, -1, T::className)) {
		lua_pop(m_pLuaState, 1);
		return;
	}

	T** ppT = (T**)lua_topointer(m_pLuaState, -1);
	//该全局对象已被Lua代码更改
	if (ppT != m_userData) {
		lua_pop(m_pLuaState, 1);
		return;
	}

	lua_pop(m_pLuaState, 1);
	lua_pushnil(m_pLuaState);
	lua_setglobal(m_pLuaState, m_strGlobalName.c_str());
}

template<class T>
void LuaObject<T>::set(lua_State *L, int table_index, const char *key)
{
	lua_pushstring(L, key);
	lua_insert(L, -2); //交换key和value
	lua_settable(L, table_index);//等效于t[key]=value，t位于table_index处，栈顶是value，栈顶之下是key
}

template<class T>
void  LuaObject<T>::PushSelf(lua_State* L)
{
	CheckLuaState(L);
	//改进版PushSelf,将类的对象对应的UserData保存到 ClassName元表中，以this指针为key。
	luaL_getmetatable(L, T::className);
	if (lua_isnil(L, -1)) {
		lua_pop(L, 1);
		RegisterMethods(L);
		luaL_getmetatable(L, T::className);
	}
	assert(!lua_isnil(L, -1));

	//以this指针为key，在metatable中检索userdata
	lua_pushlightuserdata(L, this);
	lua_rawget(L, -2);
	if (!luaL_testudata(L, -1, T::className)){
		//该对象尚未创建userdata——即尚未注册到lua
		lua_pop(L, 1);
		lua_pushlightuserdata(L, this);
		T** ppT = (T**)lua_newuserdata(L, sizeof(T**));
		m_userData = ppT;
		*ppT = static_cast<T*>(this);

		lua_pushvalue(L, -1);
		lua_insert(L, -4);
		lua_rawset(L, -3);

		lua_setmetatable(L, -2);
	} else {
		lua_insert(L, -2);
		lua_pop(L, 1);
	}
}

template<class T>
bool LuaObject<T>::RegisterGlobal(lua_State* L, const char* name)
{
	CheckLuaState(L);

	lua_getglobal(L, name);
	if (!lua_isnil(L, -1)){
		//已经有同名的全局标识符,检查是不是自身
		if (luaL_testudata(L, -1, T::className)) {
			T** ppT = (T**)lua_topointer(L, -1);
			if (*ppT == this){
				lua_pop(L, 1);
				//WARN("RegisterGlobal warning: LuaObject already registered");
				return true;
			}
		}
		lua_pop(L, 1);
		ERR("RegisterGlobal error: different global object using the same name already exisit, name: {}", name);
		return false;
	}
	lua_pop(L, 1);

	PushSelf(L);
	lua_setglobal(L, name);
	m_strGlobalName = name;
	return true;
}
template<class T>
void LuaObject<T>::RegisterMethods(lua_State* L)
{
	lua_newtable(L);
	int methods = lua_gettop(L);

	luaL_newmetatable(L, T::className);
	int metatable = lua_gettop(L);

	//隐藏userdata的实质的元表，也就是说在Lua中
	//调用getmetatable(self)得到的是self本身，而不是metatable table
	lua_pushvalue(L, methods);
	set(L, metatable, "__metatable");

	lua_pushvalue(L, methods);
	set(L, metatable, "__index");

	for (int i = 0; T::methods[i].name; ++i)
	{
		lua_pushstring(L, T::methods[i].name);
		lua_pushnumber(L, i);
		lua_pushcclosure(L, thunk, 1);
		lua_settable(L, methods);
	}

	lua_pop(L, 2);
}

template<class T>
int LuaObject<T>::thunk(lua_State *L)
{
	int index = (int)lua_tonumber(L, lua_upvalueindex(1));
	//假如Lua中使用self.Func,这里会check不通过，进而raise一个error
	T** obj = static_cast<T**>(luaL_checkudata(L, 1, T::className));
	//lua_remove(L, -1);
	if (obj && *obj){
		return ((*obj)->*(T::methods[index].mfunc))(L);
	} else {
		luaL_error(L, "LuaObject thunk error: can not get LuaObject from UserData, maybe destoryed");
	}
	return 0;
}

template<class T>
bool LuaObject<T>::CheckLuaState(lua_State* L)
{
	if (!m_pLuaState){
		m_pLuaState = L;
		return true;
	} else if(m_pLuaState != L){
		//一个LuaObject应该自始至终运行在同一个LuaState
		ERR("LuaObject CheckLuaState error: LuaObject should not run across luaState");
		luaL_error(L, "LuaObject should not run across different LuaState");
		return false;
	}
	return true;
}
