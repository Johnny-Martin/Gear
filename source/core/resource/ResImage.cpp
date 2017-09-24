#include "stdafx.h"
#include "ResImage.h"
#include "ResManager.h"

using namespace Gear::Res;

ResImage::ResImage()
{
	InitAttrMap();
	InitAttrValuePatternMap();
	InitAttrValueParserMap();
}

ResImage::ResImage(const string& strImageDesc)
{
	m_wstrFilePath = ResManager::GetInstance().GetResFilePathByName(strImageDesc);
	if (m_wstrFilePath.empty()) {
		ERR("Create ResImage failed! cannot find image file");
		return;
	}
}
ResImage::ResImage(const wstring& strImageDesc)
{

}
void ResImage::InitAttrMap()
{
	ADD_ATTR("file", "")
}
void ResImage::InitAttrValuePatternMap()
{

}
void ResImage::InitAttrValueParserMap()
{

}
///////////////////////////////////////Direct2D渲染模式相关代码///////////////////////////////////
#ifdef USE_D2D_RENDER_MODE
ID2D1Bitmap* ResImage::GetD2D1Bitmap(ID2D1RenderTarget* pRenderTarget, unsigned int width, unsigned int height)
{
	if (m_d2d1BitMapPtr) { return m_d2d1BitMapPtr; }
	//尚未加载png
	if (m_pngWidth == 0) {
		auto ret = ReadPngFile(m_wstrFilePath);
		if (!ret) { return nullptr; }
	}
	//m_d2d1BitMapPtr = new ID2D1Bitmap();
	return nullptr;
}
/////////////////////////////////////////GDI+渲染模式相关代码/////////////////////////////////////
#else
RESULT ResImage::OnDrawImpl(Graphics& graphics, const RECT& rcInvalid)
{
	HRESULT hr = S_OK;
	return hr;
}
Gdiplus::Bitmap* ResImage::GetGDIBitmap(unsigned int width, unsigned int height)
{
	return nullptr;
}
#endif