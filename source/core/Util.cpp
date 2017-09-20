#include "stdafx.h"
#include "Util.h"
#include "Log.h"
#include <stack>
#include <memory>
#include "Rpcdce.h"
#pragma comment(lib, "Rpcrt4.lib")

using namespace std;

//仅支持+-/*()六种操作符,sExp中除了数字之外就是六种操作符，不再有其他字符
//不支持负数(可以用0-正数代替)，不支持小数(可以用分数表示)。
double CalcMathExpression(const string& sExp)
{
	if (sExp.empty()) return 0.0;
	stack<double> outputStack;
	stack<char> operatorStack;

	auto IsNumber = [](const char& test) { return test >= '0' && test <= '9'; };

	auto PushToOutputStack = [&IsNumber, &outputStack](const string& originalExp, string::size_type begin) {
		string::size_type end = begin;
		while (end < originalExp.size() && IsNumber(originalExp[end])) { ++end; };
		--end;
		string strOperand = originalExp.substr(begin, end - begin + 1);
		_ASSERT(!strOperand.empty());
		int iOperand = atoi(strOperand.c_str());
		outputStack.push(iOperand);
		//返回当前操作数的末尾在串中的位置
		return end;
	};

	auto StackSafeTop = [](auto& paramStack) {
		if (paramStack.empty()) { throw false; }
		return paramStack.top();
	};

	auto StackSafePop = [](auto& paramStack) {
		if (paramStack.empty()) { throw false; }
		paramStack.pop();
	};

	auto CalcOutputStack = [&](const char& op) {
		//当op是（），直接返回false。
		if (op != '+' && op != '-' && op != '*' && op != '/') { return false; }

		double rightOp = StackSafeTop(outputStack);
		StackSafePop(outputStack);
		double leftOp = StackSafeTop(outputStack);
		StackSafePop(outputStack);
		switch (op) {
		case '+':
			outputStack.push(leftOp + rightOp);
			break;
		case '-':
			outputStack.push(leftOp - rightOp);
			break;
		case '*':
			outputStack.push(leftOp * rightOp);
			break;
		case '/':
			if (rightOp == 0) throw false;
			outputStack.push(leftOp / rightOp);
			break;
		}
		return true;
	};

	for (string::size_type i = 0; i < sExp.size(); ++i) {
		//遇到操作数了，压入操作数栈
		if (IsNumber(sExp[i])) {
			i = PushToOutputStack(sExp, i);
			continue;
		}
		char curOperator = sExp[i];
		//如果是*/号，只要下一个字符是数字，就可以计算自己。否则入栈
		if (curOperator == '*' || curOperator == '/') {
			if (IsNumber(sExp[i + 1])) {
				//将下一个操作数找出、入栈，然后计算
				i = PushToOutputStack(sExp, i + 1);
				CalcOutputStack(curOperator);
				continue;
			}
			operatorStack.push(curOperator);
		}
		else if (curOperator == '+' || curOperator == '-') {
			//如果是+-号，只要栈顶不空、不是'(', 则先计算栈顶符号，再将自己入栈
			if (operatorStack.empty() || StackSafeTop(operatorStack) == '(') {
				operatorStack.push(curOperator);
				continue;
			}
			CalcOutputStack(StackSafeTop(operatorStack));
			StackSafePop(operatorStack);
			operatorStack.push(curOperator);
		}
		else if (curOperator == '(') {
			//左括号,直接入栈
			operatorStack.push(sExp[i]);
		}
		else if (curOperator == ')') {
			//边处理边计算，则符号栈中的()之间最多只有一个操作符
			if (CalcOutputStack(StackSafeTop(operatorStack))) {
				StackSafePop(operatorStack);
			}
			if (StackSafeTop(operatorStack) != '(')
				throw false;
			StackSafePop(operatorStack);
		}
		else {
			throw false;
		}
	}
	if (!operatorStack.empty()) {
		CalcOutputStack(StackSafeTop(operatorStack));
		StackSafePop(operatorStack);
	}
	return StackSafeTop(outputStack);
};

bool CheckPngFileHead(LPWSTR wszFilePath)
{
	//PNG file head: 89 50 4E 47 0D 0A 1A 0A
	BYTE pngFileHead[] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };

	std::ifstream inPngFile(wszFilePath, ios::binary);
	BYTE value = 0;
	for (int i = 0; i<sizeof(pngFileHead) / sizeof(BYTE); ++i)
	{
		inPngFile.read((char*)&value, sizeof(BYTE));
		if (value != pngFileHead[i])
			return false;
	}
	return true;
}

wstring StringToWString(const std::string& str, UINT codePage /*= CP_ACP*/)
{
	int num = MultiByteToWideChar(codePage, 0, str.c_str(), -1, NULL, 0);
	wchar_t *wide = new wchar_t[num];
	MultiByteToWideChar(codePage, 0, str.c_str(), -1, wide, num);
	std::wstring w_str(wide);
	delete[] wide;
	return w_str;
}
string WStringToString(const wstring& wstr, UINT codePage /*= CP_ACP*/)
{
	DWORD strLength = WideCharToMultiByte(codePage, 0, (LPCWSTR)wstr.c_str(), -1, nullptr, 0, nullptr, 0);
	if (strLength <= 0) { return false; }
	char* buf = new char[strLength];
	int count = WideCharToMultiByte(codePage, 0, (LPCWSTR)wstr.c_str(), -1, buf, strLength, NULL, NULL);
	string strRet(buf);
	delete buf;

	return strRet;
}

string UTF8AToUnicodeA(const std::string& strUTF8)
{
	wstring tmpWstr = UTF8AToUnicodeW(strUTF8);
	
	string tmpAStr = WStringToString(tmpWstr, CP_ACP);
	return tmpAStr;
}
wstring UTF8AToUnicodeW(const std::string& strUTF8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, wstr, len);
	wstring ret(wstr);
	delete[] wstr;

	return ret;
}
void EraseSpace(string& str)
{
	for (string::iterator it = str.end(); it != str.begin();) {
		--it;
		if ((*it) == ' ' || (*it) == '\t' || (*it) == '\n' || (*it) == '\r') {
			str.erase(it);
		}
	}
}
string CreateGUIDAsResID()
{
	UUID uuid = { 0 };
	string guid;
	::UuidCreate(&uuid);

	RPC_CSTR szUuid = NULL;
	if (::UuidToStringA(&uuid, &szUuid) == RPC_S_OK){
		guid = (char*)szUuid;
		::RpcStringFreeA(&szUuid);
	}

	std::replace(guid.begin(), guid.end(), '-', '_');
	return guid;
}