#pragma once
/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.9.5
Description:		UI´°¿Ú£¬ÖØ¹¹UIFrameWnd
*****************************************************/

#include "../stdafx.h"
#include "../UIObject.h"
#include <list>
#include <xstring>

class UIWindow;
class WndManager
{
public:
	static								WndManager& GetInstance();
	static void 						AddWindow(HWND hWnd);
	static void 						RemoveWindow(HWND hWnd);
	static void 						OnTryExit();
										~WndManager();
private:
										WndManager();
	static vector<HWND>					m_allWndVec;
};
class UIWindow:public UIObject, public CWindowImpl<UIWindow>, public CMessageFilter
{
public:
	UIWindow();
	DECLARE_WND_CLASS(_T("CMyWindowClass"));

	BEGIN_MSG_MAP(UIWindow)
		MESSAGE_HANDLER(WM_CREATE,		OnCreate)
		MESSAGE_HANDLER(WM_ERASEBKGND,  OnEraseBkgnd)
		MESSAGE_HANDLER(WM_PAINT,		OnPaint)
		MESSAGE_HANDLER(WM_NCPAINT,		OnNcPaint)
		MESSAGE_HANDLER(WM_MOUSEMOVE,	OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSELEAVE,	OnMouseLeave)
		MESSAGE_HANDLER(WM_DESTROY,		OnDestroy)
	END_MSG_MAP()
protected:
	virtual BOOL						PreTranslateMessage(MSG* pMsg);
	virtual LRESULT						OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT						OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT						OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT						OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT						OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT						OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT						OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
#ifdef USE_D2D_RENDER_MODE
public:
	virtual HRESULT						OnDrawImpl(ID2D1RenderTarget* pRenderTarget, const RECT& rcInvalid);
protected:
	virtual HRESULT						CreateDeviceDependentResources(ID2D1RenderTarget* pRenderTarget);
	virtual HRESULT						DiscardDeviceDependentResources();
private:
	ID2D1HwndRenderTarget*				m_pHwndRenderTarget;
#else
	virtual HRESULT						OnDrawImpl(Graphics& graphics, const RECT& rcInvalid);
#endif
protected:
	virtual bool						InitImpl(const XMLElement* pElement);
	void								InitAttrMap();
	void								InitEventMap();
	void								InitAttrValuePatternMap();
	void								InitAttrValueParserMap();
	bool								CreateUIWindow();

private:
	HWND								m_hWndParent;
};