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
	ADD_ATTR("fill", "1")
		ADD_ATTR("res", "")
}

void UIBitmap::InitEventMap()
{
	//ADD_EVENT("OnCreate", nullptr)
}

bool UIBitmap::InitImpl(const XMLElement* pElement)
{
	auto ret = UIObject::InitImpl(pElement);
	if (ret && !m_attrMap["res"].empty()) {
		//����res���Զ�Ӧ��png
		//auto resID = m_attrMap["res"];
		//m_picObject = ResManager::GetInstance().GetPicObject(resID);
	}
	return true;
}

/*************************************************************************
*�������ֵ�Ƿ�Ϸ�
*	pos="leftexp, topexp, widthexp, heightexp"
*	leftexp��topexp    :֧��0-9��#mid��#width��#height��()��+��-��*��/
*	widthexp��heightexp:֧��0-9��#width��#height��()��+��-��*��/
**************************************************************************/
void UIBitmap::InitAttrValuePatternMap()
{
	ADD_ATTR_PATTERN("fill", R_CHECK_BOOL);
}
void UIBitmap::InitAttrValueParserMap()
{

}

///////////////////////////////////////Direct2D��Ⱦģʽ��ش���///////////////////////////////////
#ifdef USE_D2D_RENDER_MODE
HRESULT	UIBitmap::OnDrawImpl(ID2D1RenderTarget* pRenderTarget, const D2D1_RECT_F& rcWndPos, const RECT* rcInvalidPtr/* = nullptr*/)
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

	//return m_picObject->Draw(pRenderTarget, *rcInvalidPtr, m_picObject);
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

	return hr;
}
/////////////////////////////////////////GDI+��Ⱦģʽ��ش���/////////////////////////////////////
#else
Gdiplus::Bitmap* UIBitmap::GetGDIBitmap(unsigned int width, unsigned int height)
{
	return nullptr;
}
HRESULT	UIBitmap::OnDrawImpl(Graphics& graphics, const UIPos& rcWndPos)
{
	HRESULT hr = S_OK;

	return hr;
}
#endif