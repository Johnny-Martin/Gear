#include "stdafx.h"
#include "ResTexture.h"

using namespace Gear::Res;

ResTexture::ResTexture(const wstring& wstrFilePath)
{
	m_wstrFilePath = wstrFilePath;
	if (ReadPngFile(wstrFilePath)) {
		DetectHorizontalLine();
		DetectVerticalLine();
	}
}
///////////////////////////////////////Direct2D��Ⱦģʽ��ش���///////////////////////////////////
#ifdef USE_D2D_RENDER_MODE
ID2D1Bitmap* ResTexture::GetD2D1Bitmap(unsigned int width, unsigned int height)
{

	return nullptr;
}
/////////////////////////////////////////GDI+��Ⱦģʽ��ش���/////////////////////////////////////
#else
Gdiplus::Bitmap* ResTexture::GetGDIBitmap(unsigned int width, unsigned int height)
{

	return nullptr;
}
#endif