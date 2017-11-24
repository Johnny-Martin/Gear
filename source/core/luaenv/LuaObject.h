/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.11.20
Description:		����Lua�ű��е���C++ Obj����ķ���

ȱ�㣺1��T�еĳ�Ա��������LuaCfunction. ��Ҫ��C++����ԭ��������дһ��wrapper
2,��֧����Lua��ֱ�ӷ���obj�ĳ�Ա����
�����汾ʹ��template����LuaCfunction��ʽ��wrapper��֧�ַ�������
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
//Lua��ֻ��ʹ�á�T��Ҫ�ṩ�������ݣ�className��Ҫע��ĺ�����Ϣ(methods)
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
	lua_insert(L, -2); //����key��value
	lua_settable(L, table_index);//��Ч��t[key]=value��tλ��table_index����ջ����value��ջ��֮����key
}

template<class T>
void  LuaObject<T>::PushSelf(lua_State* L)
{
	//�Ľ���PushSelf,����Ķ����Ӧ��UserData���浽 ClassNameԪ���У���thisָ��Ϊkey��
	luaL_getmetatable(L, T::className);
	if (lua_isnil(L, -1)) {
		lua_pop(L, 1);
		RegisterMethods(L);
		luaL_getmetatable(L, T::className);
	}
	assert(!lua_isnil(L, -1));

	//��thisָ��Ϊkey����metatable�м���userdata
	lua_pushlightuserdata(L, this);
	lua_rawget(L, -2);
	if (!luaL_testudata(L, -1, T::className)){
		//�ö�����δ����userdata��������δע�ᵽlua
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

	//����userdata��ʵ�ʵ�Ԫ��Ҳ����˵��Lua��
	//����getmetatable(self)�õ�����self����������metatable table
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
	//����Lua��ʹ��self.Func,�����check��ͨ��������riseһ��error
	T** obj = static_cast<T**>(luaL_checkudata(L, 1, T::className));
	//lua_remove(L, -1);
	if (obj && *obj)
	{
		return ((*obj)->*(T::methods[index].mfunc))(L);
	}
	return 0;
}
