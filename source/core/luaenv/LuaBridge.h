/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.11.30
Description:		����Lua�ű��е���C++ Obj�����Ա������ͬʱ�ܵ���Lua�ڵĺ��������ض��ֵ

���LuaObject�ĸĽ���
1��֧�ֵ���Lua���������ض��ֵ
2��ʹ��lambda��ԭ����Ա������װ��Lua_CFunction
3��֧��Lua����Object�¼�
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

//��һ��tuple���ʵ���б�
template<std::size_t... S, typename T, typename Ret, typename... Args>
Ret CallMemberFunction(Indices<S...> indices, T* pObj, Ret(T::*mfunc)(Args...), std::tuple<Args...> paramTuple)
{
	return (pObj->*mfunc)(std::get<S>(paramTuple)...);
}

#define LUABRIDGE_DEFINE_MEMBER_FUNCTION(className, funcName)  \
int funcName##_LCF(lua_State* L){return (LambdaWrapper(this, &className::funcName))(L);}
//ԭ������ʹ��funcName�����ƣ�����֮��Ӧ��LuaCFunction������֮����������ᵼ��ģ���Ƶ�ʧ��

struct LuaListenerInfo {
	LuaListenerInfo() :cbkRef(0), objRef(0), cookie(0){};
	LuaListenerInfo(int cbk, int obj, unsigned long c) :cbkRef(cbk), objRef(obj), cookie(c) {};
	int cbkRef;
	int objRef;
	unsigned int cookie;
};

//LuaBridge�����಻��ֱ��������Lua�д�������Ҫ��C++�д��������١�
//Lua��ֻ��ʹ�á�DrivedClass��Ҫ�ṩ�������ݣ�className��Ҫע��ĺ�����Ϣ(methods)
template<typename DrivedClass>
class LuaBridge {
public:
																	LuaBridge();
	virtual															~LuaBridge();
	bool															RegisterGlobal(lua_State* L, const char* szName);
	template<typename Ret, typename... Args> Ret					CallLuaFunc(lua_State*, const char* szFuncName, Args... args);
	template<typename... Args> void									CallLuaFunc(lua_State*, const char* szFuncName, Args... args);
	int																AttachListener(lua_State* L);//�����ӿڲ��ṩ��C++��ֻ��¶��Lua
	int																DetachListener(lua_State* L);
	int																RemoveAllListener(lua_State* L);
	template<typename... Args> void									FireEvent(lua_State* L, const char* szEventName, Args... args);
	
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
	void															set(lua_State *L, int table_index, const char *key);
	static int														thunk(lua_State *L);
	static int														LuaErrorHandler(lua_State* L);
	//ռλ��ģ���Ա������ʵ��ʹ�õ���Զ�Ǹ�ģ�庯�����ػ�(ģ�����ģ���Ա�����޷��������ػ�)
	template<typename T> T											Read(lua_State* L, int index);
	template<> int													Read(lua_State* L, int index)					{ return (int)lua_tointeger(L, index); }
	template<> long long											Read(lua_State* L, int index)					{ return lua_tointeger(L, index); }
	template<> bool													Read(lua_State* L, int index)					{ return lua_toboolean(L, index) == 1; }
	template<> double												Read(lua_State* L, int index)					{ return lua_tonumber(L, index); }
	template<> float												Read(lua_State* L, int index)					{ return (float)lua_tonumber(L, index); }
	template<> const char*											Read(lua_State* L, int index)					{ return lua_tostring(L, index); }
	template<typename... Args> std::tuple<Args...>					Pop(lua_State* L)								{ auto ret = Get<Args...>(L); lua_pop(L, sizeof...(Args)); return ret; }

	template<typename T1, typename T2, typename... Args>
	tuple<T1, T2, Args...> ReadToTuple(lua_State* L, int index) {
		return std::tuple_cat(std::make_tuple(Read<T1>(L, index)), ReadToTuple<T2, Args...>(L, index + 1)); 
	}

	template<typename T>
	std::tuple<T> ReadToTuple(lua_State* L, int index) {
		return std::make_tuple(Read<T>(L, index));
	}

	template<typename... Args> 				
	std::tuple<Args...>	Get(lua_State* L) {
		return ReadToTuple<Args...>(L, -sizeof...(Args));
	}

public:
	typedef int (DrivedClass::*mfp)(lua_State *L);
	typedef struct { const char *name; mfp mfunc; }					LuaRegType;
	typedef std::list<LuaListenerInfo>								LuaListenerListType;
private:
	lua_State*														m_pLuaState;
	string															m_strGlobalName;
	DrivedClass**													m_userData;
	std::map<string, LuaListenerListType>							m_luaLisenerMap;
	unsigned long													m_curListenerCookie;
};

//AttachListener��DetachListener��DispatchEvent��������ʵ��Lua��C++֮����¼����ơ�
//ǰ������Lua������ã���ӡ��Ƴ������ߣ���������C++���ã�֪ͨLua��ļ�����
//
template<typename DrivedClass>
int LuaBridge<DrivedClass>::AttachListener(lua_State* L)
{
	int argsCount = lua_gettop(L);
	if (argsCount != 3 && argsCount != 4){
		ERR("AttachListener failed: argument error");
		return 0;
	}
	const char* szEventName = lua_tostring(L, 2);
	if (!szEventName){
		ERR("AttachListener failed: Event Name error");
		return 0;
	}
	int cbkRef = 0;
	int objRef = 0;
	if (argsCount == 4){
		objRef = luaL_ref(L, LUA_REGISTRYINDEX);
		cbkRef = luaL_ref(L, LUA_REGISTRYINDEX);
	} else {
		cbkRef = luaL_ref(L, LUA_REGISTRYINDEX);
	}

	//�˴�û���ظ������ļ�⣬ͬһ��Listener�ظ�����ͬһ��event���ᵼ��fires�¼�ʱ�ظ����ü�����
	//�迪�����Լ���֤
	LuaListenerListType& listenerList = m_luaLisenerMap[szEventName];
	LuaListenerInfo info(cbkRef, objRef, m_curListenerCookie++);
	listenerList.push_back(info);

	lua_pushinteger(L, info.cookie);
	return 1;
}

template<typename DrivedClass>
int LuaBridge<DrivedClass>::DetachListener(lua_State* L)
{
	int argsCount = lua_gettop(L);
	if (argsCount != 2) {
		ERR("DetachListener failed: argument error");
		return 0;
	}
	const char* szEventName = lua_tostring(L, 1);
	unsigned long cookie    = lua_tonumber(L, 2);
	LuaListenerListType& listenerList = m_luaLisenerMap[szEventName];
	for (auto it = listenerList.begin(); it != listenerList.end(); ++it){
		if (it->cookie == cookie){
			listenerList.erase(it);
			break;
		}
	}
	return 0;
}

template<typename DrivedClass>
int LuaBridge<DrivedClass>::RemoveAllListener(lua_State* L)
{
	const char* szEventName = lua_tostring(L, 1);
	if (szEventName && m_luaLisenerMap.find(szEventName) != m_luaLisenerMap.end()){
		LuaListenerListType& listenerList = m_luaLisenerMap[szEventName];
		listenerList.clear();
	}
	return 0;
}

//��������Ǹ�C++�õģ�֪ͨ��Lua������
template<typename DrivedClass>
template<typename... Args>
void LuaBridge<DrivedClass>::FireEvent(lua_State* L, const char* szEventName, Args... args)
{
	if (!szEventName || m_luaLisenerMap.find(szEventName) == m_luaLisenerMap.end()) {
		return;
	}
	LuaListenerListType& listenerList = m_luaLisenerMap[szEventName];
	for (auto& it:listenerList){
		lua_pushcfunction(L, LuaBridge<DrivedClass>::LuaErrorHandler);
		int errIndex = lua_gettop(L);
		
		int argsCount = 0;
		lua_rawgeti(L, LUA_REGISTRYINDEX, it.cbkRef);
		if (it.objRef != 0){
			lua_rawgeti(L, LUA_REGISTRYINDEX, it.objRef);
			argsCount = 1;
		}
		
		Push(L, args...);
		argsCount += sizeof...(Args);
		int err = lua_pcall(L, argsCount, 0, errIndex);
		if (err != 0){
			const char* errInfo = lua_tostring(L, -1);
			ERR("FireEvent error: {}", errInfo);
			lua_pop(L, 1);
			//LuaStackTrace(L);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////
template<class DrivedClass>
LuaBridge<DrivedClass>::LuaBridge() :m_pLuaState(nullptr), m_strGlobalName(), m_userData(nullptr), m_curListenerCookie(1)
{

}

template<class DrivedClass>
LuaBridge<DrivedClass>::~LuaBridge()
{
	//����DrivedClass::classNameԪ��mt��thisָ��λ�õ�UserData��Ķ���ָ��
	if (!m_pLuaState) {
		return;
	}
	const char* szName = typeid(DrivedClass).name();
	luaL_getmetatable(m_pLuaState, szName);
	assert(!lua_isnil(m_pLuaState, -1));
	lua_pushlightuserdata(m_pLuaState, this);
	lua_rawget(m_pLuaState, -2);
	if (luaL_testudata(m_pLuaState, -1, szName)) {
		//����T::classNameԪ��mt��thisָ��λ�õ�UserData��Ķ���ָ��
		DrivedClass** ppT = (DrivedClass**)lua_topointer(m_pLuaState, -1);
		*ppT = nullptr;
	}

	lua_pop(m_pLuaState, 1);
	lua_pushlightuserdata(m_pLuaState, this);
	lua_pushnil(m_pLuaState);
	lua_rawset(m_pLuaState, -3);//��mt[this] = nil,����UserData�����ã��Ա�GC����
	lua_pop(m_pLuaState, 1);

	//���������ù�ȫ�ֶ����������ȫ�ֶ����UserData�����ã��Ա�GC����
	if (m_strGlobalName.empty()) {
		return;
	}

	lua_getglobal(m_pLuaState, m_strGlobalName.c_str());
	//��ȫ�ֶ����ѱ�Lua���������������Ϊһ����userdata����
	if (!luaL_testudata(m_pLuaState, -1, szName)) {
		lua_pop(m_pLuaState, 1);
		return;
	}

	DrivedClass** ppT = (DrivedClass**)lua_topointer(m_pLuaState, -1);
	//��ȫ�ֶ����ѱ�Lua�������
	if (ppT != m_userData) {
		lua_pop(m_pLuaState, 1);
		return;
	}

	lua_pop(m_pLuaState, 1);
	lua_pushnil(m_pLuaState);
	lua_setglobal(m_pLuaState, m_strGlobalName.c_str());
}

template<class DrivedClass>
void LuaBridge<DrivedClass>::set(lua_State *L, int table_index, const char *key)
{
	lua_pushstring(L, key);
	lua_insert(L, -2); //����key��value
	lua_settable(L, table_index);//��Ч��t[key]=value��tλ��table_index����ջ����value��ջ��֮����key
}

template<class DrivedClass>
int LuaBridge<DrivedClass>::thunk(lua_State *L)
{
	int index = (int)lua_tonumber(L, lua_upvalueindex(1));
	//����Lua��ʹ��self.Func,�����check��ͨ��������raiseһ��error
	DrivedClass** obj = static_cast<DrivedClass**>(luaL_checkudata(L, 1, typeid(DrivedClass).name()));
	//lua_remove(L, -1);
	if (obj && *obj) {
		return ((*obj)->*(DrivedClass::methods[index].mfunc))(L);
	} else {
		luaL_error(L, "LuaObject thunk error: can not get LuaObject from UserData, maybe destoryed");
	}
	return 0;
}

template<typename DrivedClass>
int LuaBridge<DrivedClass>::LuaErrorHandler(lua_State* L)
{
	lua_Debug debugInfo;
	int depth = 0;
	string stackInfo = "";
	
	while (lua_getstack(L, depth, &debugInfo)) {
		int state = lua_getinfo(L, "Sln", &debugInfo);
		int localVarIndex = 1;
		stackInfo += debugInfo.short_src;
		stackInfo += " : ";
		stringstream ss;
		ss << debugInfo.currentline;
		stackInfo += ss.str();
		stackInfo += " : ";
		stackInfo += debugInfo.name ? debugInfo.name : "?";
		stackInfo += " <== ";

		//��ʱ����ӡ�ֲ�����
		/*const char* localVarName = lua_getlocal(L, &debugInfo, localVarIndex);
		while (localVarName) {
			const char* value = lua_tostring(L, -1);
			lua_pop(L, 1);
			ERR("localVarName: {}  value: {}", localVarName, value ? value : "nil");

			localVarName = lua_getlocal(L, &debugInfo, ++localVarIndex);
		}*/
		depth++;
	}
	ERR("{}", stackInfo);
	return 0;
}

template<typename DrivedClass>
bool LuaBridge<DrivedClass>::RegisterGlobal(lua_State* L, const char* szName)
{
	CheckLuaState(L);

	lua_getglobal(L, szName);
	if (!lua_isnil(L, -1)) {
		//�Ѿ���ͬ����ȫ�ֱ�ʶ��,����ǲ�������
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

	//����userdata��ʵ�ʵ�Ԫ��Ҳ����˵��Lua��
	//����getmetatable(self)�õ�����self����������metatable table
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

//Ĭ�Ͻ�self����Lua
template<typename DrivedClass> 
template<typename Ret, typename... Args >
Ret	LuaBridge<DrivedClass>::CallLuaFunc(lua_State* L, const char* szFuncName, Args... args)
{
	lua_getglobal(L, szFuncName);
	Push(L, static_cast<DrivedClass*>(this), args...);
	int ret = lua_pcall(L, sizeof...(Args) + 1, 1, 0);
	if (ret == 0){
		Ret ret{};
		std::tie(ret) = Pop<Ret>(L);
		return ret;
	}
	const char* errorInfo = lua_tostring(L, -1);//��ӡ������
	ERR("LuaBridge CallLuaFunc error: {}", errorInfo);
	return Ret();
}

template<typename DrivedClass>
template<typename... Args>
void LuaBridge<DrivedClass>::CallLuaFunc(lua_State* L, const char* szFuncName, Args... args)
{
	lua_getglobal(L, szFuncName);
	Push(L, static_cast<DrivedClass*>(this), args...);
	int ret = lua_pcall(L, sizeof...(Args)+1, 0, 0);
	if (ret != 0) {
		const char* errorInfo = lua_tostring(L, -1);//��ӡ������
		ERR("LuaBridge CallLuaFunc error: {}", errorInfo);
	}
}

template<typename DrivedClass>
bool LuaBridge<DrivedClass>::CheckLuaState(lua_State* L)
{
	if (!m_pLuaState) {
		m_pLuaState = L;
		return true;
	} else if (m_pLuaState != L) {
		//һ��LuaObjectӦ����ʼ����������ͬһ��LuaState
		ERR("LuaObject CheckLuaState error: LuaObject should not run across luaState");
		luaL_error(L, "LuaObject should not run across different LuaState");
		return false;
	}
	return true;
}

//��pObj��UserData����ʽѹ��ջ��
template<typename DrivedClass>
void LuaBridge<DrivedClass>::Push(lua_State* L, DrivedClass* pObj)
{
	CheckLuaState(L);
	const char* szClassName = typeid(DrivedClass).name();
	//�Ľ���PushSelf,����Ķ����Ӧ��UserData���浽 ClassNameԪ���У���thisָ��Ϊkey��
	luaL_getmetatable(L, szClassName);
	if (lua_isnil(L, -1)) {
		lua_pop(L, 1);
		RegisterMethods(L);
		luaL_getmetatable(L, szClassName);
	}
	assert(!lua_isnil(L, -1));

	//��thisָ��Ϊkey����metatable�м���userdata
	lua_pushlightuserdata(L, this);
	lua_rawget(L, -2);
	if (!luaL_testudata(L, -1, szClassName)) {
		//�ö�����δ����userdata��������δע�ᵽlua
		lua_pop(L, 1);
		lua_pushlightuserdata(L, this);
		DrivedClass** ppT = (DrivedClass**)lua_newuserdata(L, sizeof(DrivedClass**));
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

//����C++����ͨ��Ա����(�����Lua_CFunction����)������ֵҪô1����Ҫôû��(void)
//ģ����ĳ�Աģ�庯����֧��ƫ�ػ����ʴ˴���LambdaWrapper������ȫ��
template<typename T, typename Ret, typename... Args>
std::function<int(lua_State*)> LambdaWrapper(T* pObj, Ret(T::*mfun)(Args...)) {
	return  [pObj, mfun](lua_State* luaState)->int {
		//��tuple���һ��parameter pack
		Ret result = CallMemberFunction(
			IndicesBuilder<sizeof...(Args)>::type(),
			pObj, 
			mfun, 
			pObj->Pop<Args...>(luaState)
		);
		pObj->Push(luaState, result);
		return 1;
	};
}

//�޷���ֵ��ԭ����Ա����ʹ�ø��ػ�����Wrapper
template<typename T, typename... Args>
std::function<int(lua_State*)> LambdaWrapper(T* pObj, void(T::*mfun)(Args...)) {
	return  [pObj, mfun](lua_State* luaState)->int {
		CallMemberFunction(
			IndicesBuilder<sizeof...(Args)>::type(),
			pObj,
			mfun,
			pObj->Pop<Args...>(luaState)
		);
		return 0;
	};
}
