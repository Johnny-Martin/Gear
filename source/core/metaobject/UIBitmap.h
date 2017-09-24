/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.7.26
Description:		界面图片资源管理
*****************************************************/
#pragma once
#include "../stdafx.h"
#include "../base/UIObject.h"
#include "../resource/ResPicture.h"
#include "../entry/RenderManager.h"

class UIBitmap :public UIObject
{
public:
	UIBitmap();
#ifdef USE_D2D_RENDER_MODE
public:
	virtual HRESULT					OnDrawImpl(ID2D1RenderTarget* pRenderTarget, const D2D1_RECT_F& rcWndPos);
protected:
	virtual HRESULT					CreateDeviceDependentResources(ID2D1RenderTarget* pRenderTarget);
	virtual HRESULT					DiscardDeviceDependentResources();
#else
	virtual HRESULT					OnDrawImpl(Graphics& graphics, const UIPos& rcWndPos);
	virtual Gdiplus::Bitmap*		GetGDIBitmap(unsigned int width, unsigned int height);
#endif
	bool							InitImpl(const XMLElement* pElement);
protected:
	void							InitAttrMap();
	void							InitEventMap();
	void							InitAttrValuePatternMap();
	void							InitAttrValueParserMap();
private:
	Gear::Res::ResPicture*			m_picObject;
};