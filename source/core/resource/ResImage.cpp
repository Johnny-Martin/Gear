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
ResImage::~ResImage()
{

}
ResImage::ResImage(const string& strImageDesc)
{
	m_wstrFilePath = ResManager::GetInstance().GetResFilePathByName(strImageDesc);
	if (m_wstrFilePath.empty()) {
		ERR("Create ResImage failed! cannot find image file");
		return;
	}
}
ResImage::ResImage(const wstring& wstrPath)
	:ResPicture(wstrPath)
{
}
ResImage::ResImage(png_bytep* rowPointers, png_uint_32 width, png_uint_32 height, png_byte colorType, png_byte colorChannels, png_byte bitDepth)
	:ResPicture(rowPointers, width, height, colorType, colorChannels, bitDepth)
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
ID2D1Bitmap* ResImage::GetD2D1Bitmap(ID2D1RenderTarget* pRenderTarget, unsigned int width, unsigned int height, unsigned int& retWidth, unsigned int& retHeight)
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
	
	HRESULT hr = pRenderTarget->CreateBitmap(size, (void*)m_rowPointers[0], m_pngWidth*m_colorChannels, RenderManager::m_gBitmapProperties, &m_d2d1BitmapPtr);
	if (FAILED(hr)){
		ERR("GetD2D1Bitmap error: create ID2D1Bitmap failed!!!!");
		SafeRelease(&m_d2d1BitmapPtr);
	}
	retWidth = m_pngWidth;
	retHeight = m_pngHeight;
	return m_d2d1BitmapPtr;
}
/////////////////////////////////////////GDI+渲染模式相关代码/////////////////////////////////////
#else
Gdiplus::Bitmap* ResImage::GetGDIBitmap(unsigned int width, unsigned int height, unsigned int& retWidth, unsigned int& retHeight)
{
	if (m_gdiplusBitmapPtr){ return m_gdiplusBitmapPtr; }
	//尚未加载png
	if (m_pngWidth == 0) {
		auto ret = ReadPngFile(m_wstrFilePath);
		if (!ret) { return nullptr; }
	}

	//--//这个参数好像不管用，Bitmap是以BGRA的顺序来解析m_rowPointers中的像素数据的
	PixelFormat pixFormat = PixelFormat32bppARGB;
	INT stride = m_pngWidth*4;
	m_gdiplusBitmapPtr = new Gdiplus::Bitmap(m_pngWidth, m_pngHeight, stride, pixFormat, (BYTE*)m_rowPointers[0]);
	Gdiplus::Status status = m_gdiplusBitmapPtr->GetLastStatus();
	if (status != Ok){
		ERR("GetGDIBitmap error: create Gdiplus::Bitmap failed!!!!");
		SafeDelete(&m_gdiplusBitmapPtr);
	} 
	retWidth = m_pngWidth;
	retHeight = m_pngHeight;
	return m_gdiplusBitmapPtr;
}
#endif