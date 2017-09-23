#pragma once
#include "../stdafx.h"
#include "../utils/UIError.h"
#include "png.h"

namespace Gear {
	namespace Res {

		class ResPicture
		{
		public:
			ResPicture();
			~ResPicture();
			ResPicture(const wstring& wstrFilePath);
#ifdef USE_D2D_RENDER_MODE
			virtual ID2D1Bitmap*			GetD2D1Bitmap(unsigned int width, unsigned int height) = 0;
#else
			virtual Gdiplus::Bitmap*		GetGDIBitmap(unsigned int width, unsigned int height) = 0;
#endif
		protected:
			RESERROR						ReadPngFile(const string& strFilePath);
			bool							ReadPngFile(const wstring& wstrFilePath);
			bool							IsVerticalLine(unsigned int horizontalPos, COLORREF lineColor);
			bool							IsHorizontalLine(unsigned int verticalPos, COLORREF lineColor);
			RESERROR						DetectVerticalLine();
			RESERROR						DetectHorizontalLine();
		protected:
			bool							m_bPngFileLoaded;
			wstring							m_wstrFilePath;
			png_uint_32						m_pngWidth;
			png_uint_32						m_pngHeight;
			png_byte						m_colorType;
			png_byte						m_bitDepth;
			png_byte						m_colorChannels;
			png_bytep*						m_rowPointers; //In fact, m_rowPointers is a two-dimensional array
			png_structp						m_pngStructPtr;
			png_infop						m_pngInfoPtr;

			HBITMAP							m_hResHandle;//??????
			RESERROR						m_resError;
			vector<unsigned int>			m_arrVerticalLinePos;
			vector<unsigned int>			m_arrHorizontalLinePos;
			const COLORREF					m_purpleLineColor;
		};

	}//end of namespace Res
}//end of namespace Gear