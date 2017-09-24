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
	if (m_d2d1BitmapPtr) { return m_d2d1BitmapPtr; }
	//尚未加载png
	if (m_pngWidth == 0) {
		auto ret = ReadPngFile(m_wstrFilePath);
		if (!ret) { return nullptr; }
	}
	D2D1_SIZE_U size;
	size.width = m_pngWidth;
	size.height = m_pngHeight;
	D2D1_PIXEL_FORMAT pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);
	D2D1_BITMAP_PROPERTIES properties = { pixelFormat, 96.0, 96.0 };
	HRESULT hr = pRenderTarget->CreateBitmap(size, (void*)m_rowPointers[0], m_pngWidth*m_colorChannels, properties, &m_d2d1BitmapPtr);

	return SUCCEEDED(hr) ? m_d2d1BitmapPtr : nullptr;
}
/////////////////////////////////////////GDI+渲染模式相关代码/////////////////////////////////////
#else
RESULT ResImage::OnDrawImpl(Graphics& graphics, const D2D1_RECT_F& rcWndPos)
{
	HRESULT hr = S_OK;
	return hr;
}
Gdiplus::Bitmap* ResImage::GetGDIBitmap(unsigned int width, unsigned int height)
{
	return nullptr;
}
#endif