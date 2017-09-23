#include "stdafx.h"
#include "ResImage.h"

using namespace Gear::Res;

ResImage::ResImage(const wstring& wstrFilePath)
{
	m_wstrFilePath = wstrFilePath;
	ReadPngFile(wstrFilePath);
}

///////////////////////////////////////Direct2D��Ⱦģʽ��ش���///////////////////////////////////
#ifdef USE_D2D_RENDER_MODE
ID2D1Bitmap* ResImage::GetD2D1Bitmap(unsigned int width, unsigned int height)
{

	return nullptr;
}
/////////////////////////////////////////GDI+��Ⱦģʽ��ش���/////////////////////////////////////
#else
Gdiplus::Bitmap* ResImage::GetGDIBitmap(unsigned int width, unsigned int height)
{

	return nullptr;
}
#endif