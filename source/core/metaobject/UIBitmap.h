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

class UIBitmap :public UIObject, public Gear::Res::ResPicture
{
public:
	UIBitmap();
#ifdef USE_D2D_RENDER_MODE
public:
	virtual HRESULT					OnDrawImpl(ID2D1RenderTarget* pRenderTarget, const RECT& rcInvalid);
	virtual ID2D1Bitmap*			GetD2D1Bitmap(unsigned int width, unsigned int height);
protected:
	virtual HRESULT					CreateDeviceDependentResources(ID2D1RenderTarget* pRenderTarget);
	virtual HRESULT					DiscardDeviceDependentResources();
#else
	virtual HRESULT					OnDrawImpl(Graphics& graphics, const RECT& rcInvalid);
	virtual Gdiplus::Bitmap*		GetGDIBitmap(unsigned int width, unsigned int height);
#endif
	bool							InitImpl(const XMLElement* pElement);
protected:
	void							InitAttrMap();
	void							InitEventMap();
	void							InitAttrValuePatternMap();
	void							InitAttrValueParserMap();
private:
	ResPicture*						m_picObject;
};