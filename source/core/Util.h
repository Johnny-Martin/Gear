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
wstring			StringToWString(const std::string& str);
string			WStringToString(const wstring& wstr);

//string => wstring

//wstring => string

//string => Lua string
