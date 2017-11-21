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
	//检测索引narg处的值是否为相应的userdata，若是则返回一个指针，该指针指向类型T的对象
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
	//设置t[key]=value，t是索引为table_index对应的值，value为栈顶元素
	auto set = [](lua_State *L, int table_index, const char *key)->void {
		lua_pushstring(L, key);
		lua_insert(L, -2);			 //交换key和value
		lua_settable(L, table_index);//等效于t[key]=value，t位于table_index处，栈顶是value，栈顶之下是key
	};

	lua_newtable(luaState);
	int luaTableObjPos = lua_gettop(luaState);

	char mtName[64] = { 0 };
	::itoa(long(this), mtName, 16);
	luaL_newmetatable(luaState, mtName);
	int mtPos = lua_gettop(luaState);

	//隐藏luaTableObjPos的实质的元表，也就是说在Lua中
	//调用getmetatable(luaTableObjPos)得到的是luaTableObjPos本身，
	//而不是真正的metatable table，防止修改
	lua_pushvalue(L, luaTableObjPos);
	set(L, mtPos, "__metatable");

	//把类T中的方法保存到method table中，供Lua脚本使用
	for (RegType *l = T::methods; l->name; l++) {
		lua_pushstring(L, l->name);
		lua_pushlightuserdata(L, (void*)l); //以注册函数在数组的位置作为cclosure的upvalue
		lua_pushcclosure(L, thunk, 1); //在Lua调用的类方法，调用的都是c closure thunk，thunk通过upvalue获取实质调用的函数地址
		lua_settable(L, methods);
	}
	return 0;
}

template<class T>
int LuaObjectT::thunk(lua_State *L)
{
	//此时栈中元素是userdata和参数
	T *obj = check(L, 1); //检测是否是相应的userdata，若是，返回指向T对象的指针
	lua_remove(L, 1); //从栈中删除userdata，以便成员函数的参数的索引从1开始
	RegType *l = static_cast(lua_touserdata(L, lua_upvalueindex(1)));//利用upvalue获取相应的成员函数
	return (obj->*(l->mfunc))(L); //调用实质的成员函数
}

//检测索引narg处的值是否为相应的userdata，若是则返回一个指针，该指针指向类型T的对象
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