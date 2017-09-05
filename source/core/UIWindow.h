#pragma once
/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.9.5
Description:		UI´°¿Ú£¬ÖØ¹¹UIFrameWnd
*****************************************************/

#include "stdafx.h"
#include "UIObject.h"
#include <list>
#include <xstring>

class WndBase
{
public:
	WndBase();
	~WndBase();
	virtual	LRESULT					OnCreate(UINT message, WPARAM wParam, LPARAM lParam);
	virtual	LRESULT					OnPaint(UINT message, WPARAM wParam, LPARAM lParam);
	virtual	LRESULT					OnMove(UINT message, WPARAM wParam, LPARAM lParam);
	virtual	LRESULT					OnSize(UINT message, WPARAM wParam, LPARAM lParam);
	HWND							GetWndHandle();
protected:
	HRESULT							Initialize();
	void							RunMessageLoop();
	static LRESULT CALLBACK			WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
protected:
	HWND							m_hWnd;
};

class UIWindow:public UIObject
{
public:
	UIWindow();
public:

protected:
	void							InitAttrMap();
	void							InitEventMap();
	void							InitAttrValuePatternMap();
	void							InitAttrValueParserMap();
};