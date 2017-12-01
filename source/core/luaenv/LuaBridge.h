/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.11.30
Description:		����Lua�ű��е���C++ Obj�����Ա������ͬʱ�ܵ���Lua�ڵĺ��������ض��ֵ

���LuaObject�ĸĽ���
1��֧�ֵ���Lua���������ض��ֵ
2��ʹ��lambda��ԭ����Ա������װ��Lua_CFunction
*****************************************************/

#pragma once
#include "../stdafx.h"
extern "C" {
#include "lua.h"
#include "lualib.h"  
#include "lauxlib.h" 
}

#pragma comment(lib, "lua.lib")

#define LUABRIDGE_DEFINE_MEMBER_FUNCTION(className, funcName)  \
int funcName(lua_State* L){return LambdaWrapper(L, this, &className::funcName)(L);}


//LuaBridge�����಻��ֱ��������Lua�д�������Ҫ��C++�д��������١�
//Lua��ֻ��ʹ�á�DrivedClass��Ҫ�ṩ�������ݣ�className��Ҫע��ĺ�����Ϣ(methods)
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

private:
	template<typename T1, typename T2, typename... Args> void		Push(lua_State* L, T1 t1, T2 t2, Args... args)	{ Push(L, t1); Push(L, t2, args...); }
	void															Push(lua_State* L, const int& value)			{ lua_pushinteger(L, value); }
	void															Push(lua_State* L, const long long& value)		{ lua_pushstring(L, value); }
	void															Push(lua_State* L, const double& value)			{ lua_pushnumber(L, value); }
	void															Push(lua_State* L, const float& value)			{ lua_pushnumber(L, value); }
	void															Push(lua_State* L, const bool& value)			{ lua_pushboolean(L, value); }
	void															Push(lua_State* L, const char* value)			{ lua_pushstring(L, value); }
	void															Push(lua_State* L, const string& value)			{ lua_pushstring(L, value.c_str()); }
	void															Push(lua_State* L, DrivedClass* pObj);
	
	//ռλ��ģ���Ա������ʵ��ʹ�õ���Զ�Ǹ�ģ�庯�����ػ�(ģ�����ģ���Ա�����޷��������ػ�)
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
	//����DrivedClass::classNameԪ��mt��thisָ��λ�õ�UserData��Ķ���ָ��

	//��mt[this] = nil,����UserData�����ã��Ա�GC����UserData

	//���������ù�ȫ�ֶ����������ȫ�ֶ����UserData�����ã��Ա�GC����UserData
}

template<typename DrivedClass>
bool LuaBridge<DrivedClass>::RegisterGlobal(lua_State* L, const char* szName)
{

	return true;
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

//��pObj��UserData����ʽѹ��ջ��
template<typename DrivedClass>
void LuaBridge<DrivedClass>::Push(lua_State* L, DrivedClass* pObj)
{
	
}

//����C++����ͨ��Ա����(�����Lua_CFunction����)������ֵҪô1����Ҫôû��(void)
//ģ����ĳ�Աģ�庯����֧��ƫ�ػ����ʴ˴���LambdaWrapper������ȫ��
template<typename T, typename Ret, typename... Args>
std::function<int(lua_State*)> LambdaWrapper(lua_State* L, T* pObj, Ret(T::*mfun)(Args...)) {
	return  [pObj, mfun](lua_State* luaState)->int {
		auto paramTuple = Pop<Args...>(luaState);
		auto paramA = std::get<0>(paramTuple);
		auto paramB = std::get<1>(paramTuple);
		//��ν�paramTuple�ڵ����в����Զ��������������
		Ret result = (pObj->*mfun)(14, 15);
		Push(luaState, result);
		return 1;
	};
}

//�޷���ֵ��ԭ����Ա����ʹ�ø��ػ�����Wrapper
template<typename T, typename... Args>
std::function<int(lua_State*)> LambdaWrapper(lua_State* L, T* pObj, void(T::*mfun)(Args...)) {
	return  [pObj, mfun](lua_State* luaState)->int {
		auto paramTuple = Pop<Args...>(luaState);
		auto paramA = std::get<0>(paramTuple);
		auto paramB = std::get<1>(paramTuple);
		//��ν�paramTuple�ڵ����в����Զ��������������
		(pObj->*mfun)(14, 15);
		return 0;
	};
}