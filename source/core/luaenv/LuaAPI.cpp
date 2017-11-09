#include "stdafx.h"
#include "LuaAPI.h"

int MsgBox(lua_State* luaState)
{
	const char* luaStr = lua_tostring(luaState, 1);
	wchar_t* wszInfo = NULL;

	bool ret = LuaStringToWideChar(luaStr, wszInfo);
	if (ret)
		MessageBox(NULL, wszInfo, L"MsgBox", MB_OK);
	else
		MessageBox(NULL, L"", L"MsgBox", MB_OK);

	delete wszInfo;
	return 0;
}

int SpdLog(lua_State* luaState)
{
	const char* luaStr = lua_tostring(luaState, 1);
	if (luaStr) {
		INFO("{}", luaStr);
	}
	return 0;
}