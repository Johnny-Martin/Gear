#pragma once
#include "../stdafx.h"
#include "png.h"
#include "ResPicture.h"

namespace Gear {
	namespace Res {

class ResTexture :public ResPicture
{
public:
	ResTexture(const wstring& wstrFilePath);
#ifdef USE_D2D_RENDER_MODE
	virtual ID2D1Bitmap*			GetD2D1Bitmap(unsigned int width, unsigned int height);
#else
	virtual Gdiplus::Bitmap*		GetGDIBitmap(unsigned int width, unsigned int height);
#endif
};

	}//end of namespace Res
}//end of namespace Gear