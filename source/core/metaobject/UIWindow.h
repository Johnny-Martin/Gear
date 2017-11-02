/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.9.5
Description:		UI窗口，重构UIFrameWnd
*****************************************************/

#pragma once
#include "../stdafx.h"
#include "../base/UIObject.h"
#include <list>
#include <xstring>

class UIWindow:public UIObject, public CWindowImpl<UIWindow>, public CMessageFilter
{
public:
	UIWindow();
	DECLARE_WND_CLASS(_T("CMyWindowClass"));

	BEGIN_MSG_MAP(UIWindow)
		MESSAGE_HANDLER(WM_CREATE,			OnCreate)
		MESSAGE_HANDLER(WM_ERASEBKGND,		OnEraseBkgnd)
		MESSAGE_HANDLER(WM_PAINT,			OnPaint)
		MESSAGE_HANDLER(WM_NCPAINT,			OnNcPaint)
		MESSAGE_HANDLER(WM_DESTROY,			OnDestroy)
		//////////////////鼠标事件///////////////////
		MESSAGE_HANDLER(WM_MOUSEMOVE,		MouseAdapter)
		MESSAGE_HANDLER(WM_MOUSEHOVER,		MouseAdapter)
		MESSAGE_HANDLER(WM_MOUSELEAVE,		MouseAdapter)
		MESSAGE_HANDLER(WM_LBUTTONDOWN,		MouseAdapter)
		MESSAGE_HANDLER(WM_LBUTTONUP,		MouseAdapter)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK,	MouseAdapter)
		MESSAGE_HANDLER(WM_RBUTTONDOWN,		MouseAdapter)
		MESSAGE_HANDLER(WM_RBUTTONUP,		MouseAdapter)
		MESSAGE_HANDLER(WM_RBUTTONDBLCLK,	MouseAdapter)
		MESSAGE_HANDLER(WM_MBUTTONDOWN,		MouseAdapter)
		MESSAGE_HANDLER(WM_MBUTTONUP,		MouseAdapter)
		MESSAGE_HANDLER(WM_MBUTTONDBLCLK,	MouseAdapter)
		MESSAGE_HANDLER(WM_XBUTTONDOWN,		MouseAdapter)
		MESSAGE_HANDLER(WM_XBUTTONUP,		MouseAdapter)
		MESSAGE_HANDLER(WM_XBUTTONDBLCLK,	MouseAdapter)
		MESSAGE_HANDLER(WM_MOUSEWHEEL,		MouseAdapter)

		/* //Gear使用popwnd做窗口，没有非客户区
		MESSAGE_HANDLER(WM_NCMOUSEMOVE,		OnMouseEvent)
		MESSAGE_HANDLER(WM_NCLBUTTONDOWN,	OnMouseEvent)
		MESSAGE_HANDLER(WM_NCLBUTTONUP,		OnMouseEvent)
		MESSAGE_HANDLER(WM_NCLBUTTONDBLCLK, OnMouseEvent)
		MESSAGE_HANDLER(WM_NCRBUTTONDOWN,	OnMouseEvent)
		MESSAGE_HANDLER(WM_NCRBUTTONUP,		OnMouseEvent)
		MESSAGE_HANDLER(WM_NCRBUTTONDBLCLK, OnMouseEvent)
		MESSAGE_HANDLER(WM_NCMBUTTONDOWN,	OnMouseEvent)
		MESSAGE_HANDLER(WM_NCMBUTTONUP,		OnMouseEvent)
		MESSAGE_HANDLER(WM_NCMBUTTONDBLCLK, OnMouseEvent)
		MESSAGE_HANDLER(WM_NCXBUTTONDOWN,	OnMouseEvent)
		MESSAGE_HANDLER(WM_NCXBUTTONUP,		OnMouseEvent)
		MESSAGE_HANDLER(WM_NCXBUTTONDBLCLK, OnMouseEvent)
		*/
	END_MSG_MAP()
protected:
	virtual BOOL						PreTranslateMessage(MSG* pMsg);
	virtual LRESULT						OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT						OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT						OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT						OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT						OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

#ifdef USE_D2D_RENDER_MODE
public:
	virtual HRESULT						OnDrawImpl(ID2D1RenderTarget* pRenderTarget, const D2D1_RECT_F& rcWndPos);
protected:
	virtual HRESULT						CreateDeviceDependentResources(ID2D1RenderTarget* pRenderTarget);
	virtual HRESULT						DiscardDeviceDependentResources();
private:
	ID2D1HwndRenderTarget*				m_pHwndRenderTarget;
#else
	virtual HRESULT						OnDrawImpl(Graphics& graphics, const UIPos& rcWndPos);
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