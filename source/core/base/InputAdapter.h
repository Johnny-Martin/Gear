/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.11.2
Description:		消息适配器，拆分UIObject的功能
*****************************************************/

#pragma once
#include "../stdafx.h"

template<typename SubClass>
class InputAdapter
{
public:
	InputAdapter();
	//负责处理、转发鼠标消息。x、y的值是相对于当前对象的的左上角的值，
	//当将消息转给子节点时，需要转为子节点的坐标值
	virtual LRESULT	MouseAdapter(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT	KeyboardAdapter(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
	map<UINT, string> m_msgMap;
};

template<typename SubClass>
InputAdapter<SubClass>::InputAdapter()
{
	m_msgMap[WM_MOUSEMOVE]		= "OnMouseMove";
	m_msgMap[WM_MOUSEHOVER]		= "OnMouseHover";
	m_msgMap[WM_MOUSELEAVE]		= "OnMouseLeave";
	m_msgMap[WM_LBUTTONDOWN]	= "OnLButtonDown";
	m_msgMap[WM_LBUTTONUP]		= "OnLButtonUp";
	m_msgMap[WM_LBUTTONDBLCLK]	= "OnLButtonDbClick";
	m_msgMap[WM_RBUTTONDOWN]	= "OnRButtonDown";
	m_msgMap[WM_RBUTTONUP]		= "OnRButtonUp";
	m_msgMap[WM_RBUTTONDBLCLK]	= "OnRButtonDbClick";
	m_msgMap[WM_MBUTTONDOWN]	= "OnMButtonDown";
	m_msgMap[WM_MBUTTONUP]		= "OnMButtonUp";
	m_msgMap[WM_MBUTTONDBLCLK]	= "OnMButtonDbClick";
	m_msgMap[WM_XBUTTONDOWN]	= "OnXButtonDown";
	m_msgMap[WM_XBUTTONUP]		= "OnXButtonUp";
	m_msgMap[WM_XBUTTONDBLCLK]	= "OnXButtonDbClick";
	m_msgMap[WM_MOUSEWHEEL]		= "OnMouseWheel";
}

template<typename SubClass>
LRESULT	InputAdapter<SubClass>::MouseAdapter(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ATLASSERT((uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST)); //|| (uMsg >= 0x00A0 && uMsg <= 0x00AD)
	SubClass* pWndObj = static_cast<SubClass*>(this);
	if (pWndObj->m_attrMap["visible"] != "1" || pWndObj->m_attrMap["enable"] != "1"){
		return 0;
	}

	if (uMsg == WM_MOUSEWHEEL)
	{
		int fwKeys = GET_KEYSTATE_WPARAM(wParam);
		int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);
	}
	WORD x = LOWORD(lParam);
	WORD y = HIWORD(lParam);
	//INFO("MouseAdapter : {}, Y:{}", x, y);
	
	//发事件给脚本（处理自己的）
	if (!m_msgMap[uMsg].empty()){
		UIEvent* pEvent = pWndObj->m_eventMap[m_msgMap[uMsg]];
		INFO("Fire event : {}, UIObject name:{}", m_msgMap[uMsg], pWndObj->m_attrMap["id"]);
		
		if (pEvent){
			//INFO("Fire event : {}, UIObject name:{}", m_msgMap[uMsg], pWndObj->m_attrMap["id"]);
			pEvent->Fire();
		}
	}

	//通知给子节点
	vector<PAIR>* pChildrenVec = pWndObj->m_pVecChildrenPair;
	if (pChildrenVec->empty()){
		return 0;
	}

	auto it = pChildrenVec->end();
	do{
		--it;
		if (it->second->m_attrMap["visible"] == "1" && it->second->m_attrMap["enable"] == "1") {
			UIPos pos = it->second->m_pos;
			if (x > pos.left && x < pos.left + pos.width && y > pos.top && y < pos.top + pos.height) {
				x -= pos.left;
				y -= pos.top;
				lParam =  MAKELPARAM(x, y);
				it->second->MouseAdapter(uMsg, wParam, lParam, bHandled);
				break;
			}
		}
	} while (it != pChildrenVec->begin());

	return 0;
}

template<typename SubClass>
LRESULT	InputAdapter<SubClass>::KeyboardAdapter(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}
