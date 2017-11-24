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
	void PushSelf(lua_State* luaState);
protected:
	typedef int (T::*mfp)(lua_State *L);
	typedef struct { const char *name; mfp mfunc; } RegType;
private:
	void RegisterMethods(lua_State* luaState);
	static int thunk(lua_State *L);
	static void set(lua_State *L, int table_index, const char *key);
};

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
	//假如Lua中使用self.Func,这里会check不通过，进而rise一个error
	T** obj = static_cast<T**>(luaL_checkudata(L, 1, T::className));
	//lua_remove(L, -1);
	if (obj && *obj)
	{
		return ((*obj)->*(T::methods[index].mfunc))(L);
	}
	return 0;
}
