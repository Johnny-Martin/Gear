#ifndef LUAOBJECT_H
#define LUAOBJECT_H
#include "stdafx.h"

extern "C" {  
#include "lua.h"
#include "lualib.h"  
#include "lauxlib.h" 
}

#pragma comment(lib, "lua.lib")

#include <string>
#include <map>
using namespace std;

template<class T>
class LuaObjectT
{
public:
	typedef int (T::*mfp)(lua_State*);
	typedef struct {const char* funcName, mfp pFunc} LuaRegType;
	
	int Register(lua_State* luaState);

private:
	static int thunk(lua_State *L);
	//�������narg����ֵ�Ƿ�Ϊ��Ӧ��userdata�������򷵻�һ��ָ�룬��ָ��ָ������T�Ķ���
	static T *check(lua_State *L, int narg);
	typedef struct {T *pT;} userdataType;
};

class TestClass : public LuaObjectT<TestClass>
{
public:
	int Add(lua_State *L);
	int Minus(lua_State *L);
public:
	static LuaRegType methodsRegInfo[] = { 
		{"Add", Add}, 
		{"Minus",Minus}, 
		{nullptr, nullptr} 
	};
};

template<class T>
int LuaObjectT::Register(lua_State* luaState)
{
	//����t[key]=value��t������Ϊtable_index��Ӧ��ֵ��valueΪջ��Ԫ��
	auto set = [](lua_State *L, int table_index, const char *key)->void {
		lua_pushstring(L, key);
		lua_insert(L, -2);			 //����key��value
		lua_settable(L, table_index);//��Ч��t[key]=value��tλ��table_index����ջ����value��ջ��֮����key
	};

	lua_newtable(luaState);
	int luaTableObjPos = lua_gettop(luaState);

	char mtName[64] = { 0 };
	::itoa(long(this), mtName, 16);
	luaL_newmetatable(luaState, mtName);
	int mtPos = lua_gettop(luaState);

	//����luaTableObjPos��ʵ�ʵ�Ԫ��Ҳ����˵��Lua��
	//����getmetatable(luaTableObjPos)�õ�����luaTableObjPos����
	//������������metatable table����ֹ�޸�
	lua_pushvalue(L, luaTableObjPos);
	set(L, mtPos, "__metatable");

	//����T�еķ������浽method table�У���Lua�ű�ʹ��
	for (RegType *l = T::methods; l->name; l++) {
		lua_pushstring(L, l->name);
		lua_pushlightuserdata(L, (void*)l); //��ע�ắ���������λ����Ϊcclosure��upvalue
		lua_pushcclosure(L, thunk, 1); //��Lua���õ��෽�������õĶ���c closure thunk��thunkͨ��upvalue��ȡʵ�ʵ��õĺ�����ַ
		lua_settable(L, methods);
	}
	return 0;
}

template<class T>
int LuaObjectT::thunk(lua_State *L)
{
	//��ʱջ��Ԫ����userdata�Ͳ���
	T *obj = check(L, 1); //����Ƿ�����Ӧ��userdata�����ǣ�����ָ��T�����ָ��
	lua_remove(L, 1); //��ջ��ɾ��userdata���Ա��Ա�����Ĳ�����������1��ʼ
	RegType *l = static_cast(lua_touserdata(L, lua_upvalueindex(1)));//����upvalue��ȡ��Ӧ�ĳ�Ա����
	return (obj->*(l->mfunc))(L); //����ʵ�ʵĳ�Ա����
}

//�������narg����ֵ�Ƿ�Ϊ��Ӧ��userdata�������򷵻�һ��ָ�룬��ָ��ָ������T�Ķ���
template<class T>
T* LuaObjectT::check(lua_State *L, int narg)
{
	userdataType *ud = static_cast(luaL_checkudata(L, narg, T::className));
	if (!ud) {
		luaL_typerror(L, narg, T::className);
		return NULL;
	}
	return ud->pT;
}
#endif