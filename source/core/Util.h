/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.8.14
Description:		常用函数
*****************************************************/

#pragma once
#include "stdafx.h"

using namespace std;

double			CalcMathExpression(const string& sExp);
bool			CheckPngFileHead(LPWSTR wszFilePath);
wstring			StringToWString(const std::string& str, UINT codePage = CP_ACP);
string			WStringToString(const wstring& wstr, UINT codePage = CP_ACP);
string			UTF8AToUnicodeA(const std::string& str);
wstring			UTF8AToUnicodeW(const std::string& str);
void			EraseSpace(string& str);
string			CreateGUIDAsResID();

template<class Interface>
inline void SafeRelease(Interface** ppInterfaceToRelease)
{
	if (*ppInterfaceToRelease != NULL)
	{
		(*ppInterfaceToRelease)->Release();

		(*ppInterfaceToRelease) = NULL;
	}
}
//string => wstring

//wstring => string

//string => Lua string
