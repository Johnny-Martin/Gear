/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.7.26
Description:		xml�����������xml�����ɶ����б�
*****************************************************/
#pragma once
#include "../stdafx.h"

class UIWindow;
class WndManager
{
public:
	static WndManager&					GetInstance();
	static void 						AddWindow(UIWindow* pUIWnd);
	static void 						RemoveWindow(UIWindow* pUIWnd);
	static void 						RePaintAllWnd();
	static void 						OnTryExit();
	~WndManager();
private:
	WndManager();
	static vector<UIWindow*>			m_allWndVec;
};
