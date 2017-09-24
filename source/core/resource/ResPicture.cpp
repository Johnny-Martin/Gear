#include "stdafx.h"
#include "ResPicture.h"

using namespace Gear::Res;

ResPicture::ResPicture() :
	m_purpleLineColor(RGB(127, 0, 127)),
	m_wstrFilePath(L""),
	m_pngWidth(0),
	m_pngHeight(0),
	m_colorType(0),
	m_bitDepth(0),
	m_colorChannels(0),
	m_rowPointers(nullptr),
	m_pngStructPtr(nullptr),
	m_pngInfoPtr(nullptr)
#ifdef USE_D2D_RENDER_MODE
	,m_d2d1BitmapPtr(nullptr)
#endif
{

}

ResPicture::~ResPicture()
{
	//m_rowPointers is allocated by malloc() in function png_create_read_struct(),
	//thus release with free()
	if (m_rowPointers)
	{
		//for (unsigned int rowIndex=0; rowIndex<m_pngHeight; ++rowIndex)
		//free(m_rowPointers[rowIndex]);

		free(m_rowPointers[0]);
	}

	if (m_pngStructPtr && m_pngInfoPtr)
		png_destroy_read_struct(&m_pngStructPtr, &m_pngInfoPtr, NULL);
}
ResPicture::ResPicture(const wstring& wstrFilePath) 
	:m_purpleLineColor(RGB(127, 0, 127))
	,m_wstrFilePath(L"")
	,m_pngWidth(0)
	,m_pngHeight(0)
	,m_colorType(0)
	,m_bitDepth(0)
	,m_rowPointers(nullptr)
	,m_pngStructPtr(nullptr)
	,m_pngInfoPtr(nullptr)
#ifdef USE_D2D_RENDER_MODE
	, m_d2d1BitmapPtr(nullptr)
#endif
{
	m_wstrFilePath = wstrFilePath;
	//ReadPngFile(wstrFilePath);
}
bool ResPicture::ReadPngFile(const wstring& wstrFilePath)
{
	string strFilePath = WStringToString(wstrFilePath);
	if (!strFilePath.empty()) {
		auto ret = ReadPngFile(strFilePath);
		if (ret == RES_SUCCESS) { return true; }

		ERR("ReadPngFile error: Load png file error, code: {}", ret);
		return false;
	}
	ERR("ReadPngFile error: convert file path error");
	return false;
}

RESERROR ResPicture::ReadPngFile(const string& strFilePath)
{
	//int multiByteLen = WideCharToMultiByte(CP_ACP, 0, wszFilePath, -1, NULL, 0, NULL, NULL);
	//char* file_name = new char[multiByteLen + 1];
	//WideCharToMultiByte(CP_ACP, 0, wszFilePath, -1, file_name, multiByteLen, NULL, NULL);

	/* open file and test for it being a png */
	FILE *fp = fopen(strFilePath.c_str(), "rb");
	if (!fp)
		return RES_ERROR_FILE_NOT_FOUND;

	unsigned char header[8];    // 8 is the maximum size that can be checked
	fread(header, 1, 8, fp);
	if (png_sig_cmp(header, 0, 8))
		return RES_ERROR_ILLEGAL_FILE_TYPE;


	/* initialize stuff */
	m_pngStructPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!m_pngStructPtr)
		return RES_ERROR_PARSE_FILE_FALIED;

	m_pngInfoPtr = png_create_info_struct(m_pngStructPtr);
	if (!m_pngInfoPtr)
		return RES_ERROR_PARSE_FILE_FALIED;

	if (setjmp(png_jmpbuf(m_pngStructPtr)))
		return RES_ERROR_PARSE_FILE_FALIED;

	png_init_io(m_pngStructPtr, fp);
	png_set_sig_bytes(m_pngStructPtr, 8);

	png_read_info(m_pngStructPtr, m_pngInfoPtr);

	if (png_get_color_type(m_pngStructPtr, m_pngInfoPtr) != PNG_COLOR_TYPE_RGB
		&& png_get_color_type(m_pngStructPtr, m_pngInfoPtr) != PNG_COLOR_TYPE_RGBA)
		return RES_ERROR_ILLEGAL_PNG_FILE;

	m_pngWidth = png_get_image_width(m_pngStructPtr, m_pngInfoPtr);
	m_pngHeight = png_get_image_height(m_pngStructPtr, m_pngInfoPtr);
	m_colorType = png_get_color_type(m_pngStructPtr, m_pngInfoPtr);
	m_bitDepth = png_get_bit_depth(m_pngStructPtr, m_pngInfoPtr);
	m_colorChannels = png_get_channels(m_pngStructPtr, m_pngInfoPtr);

	int number_of_passes = png_set_interlace_handling(m_pngStructPtr);
	png_read_update_info(m_pngStructPtr, m_pngInfoPtr);


	/* read file */
	if (setjmp(png_jmpbuf(m_pngStructPtr)))
		return RES_ERROR_PARSE_FILE_FALIED;

	m_rowPointers = (png_bytep*)malloc(sizeof(png_bytep) * m_pngHeight);
	//m_rowPointers = new png_bytep[(sizeof(png_bytep) * m_pngHeight)];

	//ID2D1HwndRenderTarget::CreateBitmap only support continuous png pixel data in memory
	//allocate a continuous memory for m_rowPointers so that class "Image"  can return 
	//m_rowPointers directly in GetPngPixelArray
	png_uint_32 rowSize = png_get_rowbytes(m_pngStructPtr, m_pngInfoPtr);
	png_byte* pngPixelData = (png_byte*)malloc(rowSize * m_pngHeight);
	for (unsigned int rowIndex = 0; rowIndex < m_pngHeight; ++rowIndex)
	{
		png_byte *rowHead = (png_byte*)((int)pngPixelData + rowIndex * rowSize);
		m_rowPointers[rowIndex] = (png_byte*)rowHead;
	}

	//Old memory allocation
	//for (unsigned int rowIndex=0; rowIndex<m_pngHeight; ++rowIndex)
	//{
	//	png_uint_32 size = png_get_rowbytes(m_pngStructPtr,m_pngInfoPtr);
	//	m_rowPointers[rowIndex] = (png_byte*) malloc(size);
	//}
	png_read_image(m_pngStructPtr, m_rowPointers);

	fclose(fp);
	return RES_SUCCESS;
}

bool ResPicture::IsVerticalLine(unsigned int horizontalPos, const COLORREF lineColor)
{
	for (unsigned int rowIndex = 0; rowIndex < m_pngHeight; ++rowIndex)
	{
		png_byte* row = m_rowPointers[rowIndex];
		png_byte* ptr = &(row[horizontalPos*m_colorChannels]);

		COLORREF pixelColor = RGB(ptr[0], ptr[1], ptr[2]);
		if (lineColor != pixelColor)
			return false;
	}
	return true;
}

bool ResPicture::IsHorizontalLine(unsigned int horizontalPos, const COLORREF lineColor)
{
	png_byte* row = m_rowPointers[horizontalPos];
	for (unsigned int columnIndex = 0; columnIndex < m_pngWidth; ++columnIndex)
	{
		png_byte* ptr = &(row[columnIndex*m_colorChannels]);

		COLORREF pixelColor = RGB(ptr[0], ptr[1], ptr[2]);
		if (lineColor != pixelColor)
			return false;
	}
	return true;
}

//detect the dividing line(RGB: 255,0,255)
RESERROR ResPicture::DetectVerticalLine()
{
	//before calling this function,make sure that
	//png file must has been loaded to memory successfully.
	png_byte* pixelDataPtr = NULL;

	for (unsigned int columnIndex = 0; columnIndex < m_pngWidth; ++columnIndex)
	{
		pixelDataPtr = &(m_rowPointers[0][columnIndex*m_colorChannels]);
		COLORREF pixelColor = RGB(pixelDataPtr[0], pixelDataPtr[1], pixelDataPtr[2]);
		if (m_purpleLineColor == pixelColor)
		{
			if (IsVerticalLine(columnIndex, m_purpleLineColor))
			{
				m_arrVerticalLinePos.push_back(columnIndex);
			}
		}

	}
	return RES_SUCCESS;
}

RESERROR ResPicture::DetectHorizontalLine()
{
	//before calling this function,make sure that
	//png file must has been loaded to memory successfully.
	png_byte* pixelDataPtr = NULL;

	for (unsigned int rowIndex = 0; rowIndex < m_pngHeight; ++rowIndex)
	{
		pixelDataPtr = &(m_rowPointers[rowIndex][0]);
		COLORREF pixelColor = RGB(pixelDataPtr[0], pixelDataPtr[1], pixelDataPtr[2]);
		if (m_purpleLineColor == pixelColor)
		{
			if (IsHorizontalLine(rowIndex, m_purpleLineColor))
			{
				m_arrHorizontalLinePos.push_back(rowIndex);
			}
		}

	}
	return RES_SUCCESS;
}
///////////////////////////////////////Direct2D渲染模式相关代码///////////////////////////////////
#ifdef USE_D2D_RENDER_MODE
HRESULT ResPicture::OnDrawImpl(ID2D1RenderTarget* pRenderTarget, const D2D1_RECT_F& rcWndPos, const RECT* rcInvalidPtr/* = nullptr*/)
{
	//--//设置额外的属性
	pRenderTarget->DrawBitmap(m_d2d1BitmapPtr, rcWndPos);
	return S_OK;
}	    
HRESULT ResPicture::CreateDeviceDependentResources(ID2D1RenderTarget* pRenderTarget)
{
	ID2D1Bitmap* pBitmap = GetD2D1Bitmap(pRenderTarget, m_pngWidth, m_pngHeight);
	return pBitmap ? S_OK : S_FALSE;
}	    
HRESULT ResPicture::DiscardDeviceDependentResources()
{
	SafeRelease(&m_d2d1BitmapPtr);
	return S_OK;
}
/////////////////////////////////////////GDI+渲染模式相关代码/////////////////////////////////////
#else
HRESULT ResPicture::OnDrawImpl(Graphics& graphics, const UIPos& rcWndPos)
{
	return S_OK;
}
#endif