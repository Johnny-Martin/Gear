#include "stdafx.h"
#include "UIBitmap.h"
#include "../resource/ResManager.h"

using namespace Gear::Res;

UIBitmap::UIBitmap() :m_picObject(nullptr)
{
	InitAttrMap();
	InitEventMap();
	InitAttrValuePatternMap();
	InitAttrValueParserMap();
}
void UIBitmap::InitAttrMap()
{
	ADD_ATTR("stretch", "1")
	ADD_ATTR("frequent", "0")
	ADD_ATTR("res", "")
}
void UIBitmap::InitEventMap()
{
	//ADD_EVENT("OnCreate", nullptr)
}
void UIBitmap::InitAttrValuePatternMap()
{
	ADD_ATTR_PATTERN("stretch", R_CHECK_BOOL);
	ADD_ATTR_PATTERN("frequent", R_CHECK_BOOL);
}
void UIBitmap::InitAttrValueParserMap()
{
	//frequent属性只能在初始化时(在xml中)指定，其他时候不得更改
	auto ParseFrequent = [&](const string& sAttrName)->bool {
		if (m_bInit == true){
			ATLASSERT(FALSE);
			return false;
		}
		return true;
	};

	//ADD_ATTR_PARSER("frequent", ParseFrequent)
}

///////////////////////////////////////Direct2D渲染模式相关代码///////////////////////////////////
#ifdef USE_D2D_RENDER_MODE
HRESULT	UIBitmap::OnDrawImpl(ID2D1RenderTarget* pRenderTarget, const D2D1_RECT_F& rcWndPos)
{
	HRESULT hr = S_OK;
	if (!m_picObject){
		auto resID = m_attrMap["res"];
		m_picObject = ResManager::GetInstance().GetPicObject(resID); 
	}
	if (!m_picObject) {
		ATLASSERT(FALSE);
		return S_FALSE;
	}

	auto sFrequent = m_attrMap["frequent"];
	//frequent属性专为texture的优化而设计，对于经常变更大小的地方，如窗口背景，请将frequent置为“1”！！！！！！
	if (sFrequent == "0"){
		unsigned int realWidth{ 0 };
		unsigned int realHeight{ 0 };
		ID2D1Bitmap* bitmapPtr = m_picObject->GetD2D1Bitmap(pRenderTarget, m_pos.width, m_pos.height, realWidth, realHeight);
		if (!bitmapPtr) {
			ERR("GetD2D1Bitmap error: get nullptr!!!!");
			return S_FALSE;
		}
		auto sStretch = m_attrMap["stretch"];
		D2D1_RECT_F realRect(rcWndPos);
		if (sStretch == "0") {
			realRect.right = realRect.left + realWidth;
			realRect.bottom = realRect.top + realHeight;
		}
		pRenderTarget->DrawBitmap(bitmapPtr, realRect);
	} else {
		hr = m_picObject->OnDrawImplEx(pRenderTarget, rcWndPos, this);
	}

	return hr;
}
HRESULT	UIBitmap::CreateDeviceDependentResources(ID2D1RenderTarget* pRenderTarget)
{
	HRESULT hr = S_OK;

	return hr;
}
HRESULT	UIBitmap::DiscardDeviceDependentResources() 
{
	HRESULT hr = S_OK;
	if (m_picObject){
		m_picObject->DiscardD2D1Bitmap();
	}
	return hr;
}
/////////////////////////////////////////GDI+渲染模式相关代码/////////////////////////////////////
#else
HRESULT	UIBitmap::OnDrawImpl(Graphics& graphics, const UIPos& rcWndPos)
{
	HRESULT hr = S_OK;
	if (!m_picObject) {
		auto resID = m_attrMap["res"];
		m_picObject = ResManager::GetInstance().GetPicObject(resID);
	}
	if (!m_picObject) {
		ATLASSERT(FALSE);
		return S_FALSE;
	}
	unsigned int realWidth{ 0 };
	unsigned int realHeight{ 0 };
	Gdiplus::Bitmap* pBitmap = m_picObject->GetGDIBitmap(m_pos.width, m_pos.height, realWidth, realHeight);
	if (!pBitmap){
		ATLASSERT(FALSE);
		return S_FALSE;
	}

	Status status = GenericError;
	auto spStretch = m_attrMap["stretch"];
	if (spStretch == "0") {
		status = graphics.DrawImage(pBitmap, rcWndPos.left, rcWndPos.top, realWidth, realHeight);
	} else {
		status = graphics.DrawImage(pBitmap, rcWndPos.left, rcWndPos.top, rcWndPos.width, rcWndPos.height);
	}

	return status == Ok ? S_OK : S_FALSE;
}
#endif