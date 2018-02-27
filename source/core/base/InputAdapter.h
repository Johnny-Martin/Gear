/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.11.2
Description:		��Ϣ�����������UIObject�Ĺ���
*****************************************************/

#pragma once
#include "../stdafx.h"

template<typename SubClass>
class InputAdapter
{
public:
	InputAdapter();
	//������ת�������Ϣ��x��y��ֵ������ڵ�ǰ����ĵ����Ͻǵ�ֵ��
	//������Ϣת���ӽڵ�ʱ����ҪתΪ�ӽڵ������ֵ
	//�ϳ�OnMouseEnter��OnMouseDrag����������Ϣ
	virtual LRESULT	MouseAdapter(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT	KeyboardAdapter(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
	void			  HandleTrackMouseEvent(HWND hWnd);
private:
	map<UINT, string> m_msgMap;
protected:
	bool			  m_bMouseIn;
	bool			  m_bMouseLButtonDown;
};

template<typename SubClass>
InputAdapter<SubClass>::InputAdapter()
	:m_bMouseIn(false)
	,m_bMouseLButtonDown(false)
{
	m_msgMap[WM_MOUSEMOVE]		= "OnMouseMove";
	//m_msgMap[WM_MOUSEENTER]	= "OnMouseEnter";//windows�в����������Ϣ������һ��OnMouseMove��ΪOnMouseEnter
	//m_msgMap[WM_MOUSEDRAG]	= "OnMouseDrag";
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
	//ATLASSERT((uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST)); //|| (uMsg >= 0x00A0 && uMsg <= 0x00AD)
	SubClass* pSubObj = static_cast<SubClass*>(this);
	if (pSubObj->m_attrMap["visible"] != "1" || pSubObj->m_attrMap["enable"] != "1"){
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
	
	//���¼����ű�����ǰ�ڵ㣩
	if (!m_msgMap[uMsg].empty()){
		string sEventName;
		UIEvent* pEvent = nullptr;

		if (uMsg == WM_LBUTTONDOWN){
			m_bMouseLButtonDown = true;
		} else if (uMsg == WM_LBUTTONUP) {
			m_bMouseLButtonDown = false;
		}

		if (uMsg == WM_MOUSEMOVE){
			if (!m_bMouseIn){
				//��һ���յ�OnMouseMove�¼���������OnMouseEnter
				pEvent = pSubObj->m_eventMap["OnMouseEnter"];
				HandleTrackMouseEvent(pSubObj->GetHostWndHandle());
			} else if(m_bMouseLButtonDown){
				pEvent = pSubObj->m_eventMap["OnMouseDrag"];
			}
			m_bMouseIn = true;
		} else if (uMsg == WM_MOUSELEAVE) {
			m_bMouseIn = false;
		}

		pEvent = pEvent ? pEvent : pSubObj->m_eventMap[m_msgMap[uMsg]];
		//INFO("Fire event : {}, UIObject name:{}", m_msgMap[uMsg], pSubObj->m_attrMap["id"]);
		
		if (pEvent){
			pEvent->Fire(pSubObj, x, y);
		}
	}

	//֪ͨ���ӽڵ�
	vector<PAIR>* pChildrenVec = pSubObj->m_pVecChildrenPair;
	if (pChildrenVec->empty()){
		return 0;
	}

	auto it = pChildrenVec->end();
	do{
		--it;
		auto pChildObj = it->second;
		if (pChildObj->m_attrMap["visible"] == "1" && pChildObj->m_attrMap["enable"] == "1") {
			UIPos pos = it->second->m_pos;
			if (x > pos.left && x < pos.left + pos.width && y > pos.top && y < pos.top + pos.height) {
				x -= pos.left;
				y -= pos.top;
				lParam =  MAKELPARAM(x, y);
				pChildObj->MouseAdapter(uMsg, wParam, lParam, bHandled);
				break;
			} else {
				if (uMsg == WM_MOUSEMOVE && pChildObj->m_bMouseIn){
					//pChildObj->MouseAdapter(WM_MOUSELEAVE, wParam, lParam, bHandled);
				}
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

template<typename SubClass>
void InputAdapter<SubClass>::HandleTrackMouseEvent(HWND hWnd)
{
	ATLASSERT(hWnd);
	auto pTrackMouseEvent = new TRACKMOUSEEVENT();
	pTrackMouseEvent->cbSize = sizeof(TRACKMOUSEEVENT);
	pTrackMouseEvent->dwFlags = TME_HOVER | TME_LEAVE;
	pTrackMouseEvent->hwndTrack = hWnd;
	pTrackMouseEvent->dwHoverTime = 2000;
	TrackMouseEvent(pTrackMouseEvent);
}