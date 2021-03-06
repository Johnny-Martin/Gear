#include "stdafx.h"
#include "UIResource.h"
#include "Log.h"
#include <sstream>


RPicture::RPicture():m_hResHandle(NULL)
					,m_szResID("")
					,m_szResTypeInfo("")
					,m_pngWidth(0)
					,m_pngHeight(0)
					,m_colorType(0)
					,m_bitDepth(0)
					,m_colorChannels(0)
					,m_rowPointers(NULL)
					,m_pngStructPtr(NULL)
					,m_pngInfoPtr(NULL)
					,m_resError(RES_SUCCESS)
{

}

RPicture::~RPicture()
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

	cout<<"ID: "<<m_szResID<<" free m_rowPointers done"<<endl;
}

void RPicture::SetResID(string resID)
{
	m_szResID = resID;
}
RESERROR RPicture::GetLastResError() const
{ 
	return m_resError;
}

png_bytep* RPicture::GetRowPointers() const
{
	return m_rowPointers;
}

png_infop RPicture::GetPngInfo() const
{
	return m_pngInfoPtr;
}

bool RPicture::IsVerticalLine(unsigned int horizontalPos, const COLORREF lineColor)
{
	for (unsigned int rowIndex=0; rowIndex<m_pngHeight; ++rowIndex)
	{
		png_byte* row = m_rowPointers[rowIndex];
		png_byte* ptr = &(row[horizontalPos*m_colorChannels]);
		
		COLORREF pixelColor = RGB(ptr[0], ptr[1], ptr[2]);
		if (lineColor != pixelColor)
			return false;
	}
	return true;
}
bool RPicture::IsHorizontalLine(unsigned int horizontalPos, const COLORREF lineColor)
{
	png_byte* row = m_rowPointers[horizontalPos];
	for (unsigned int columnIndex=0; columnIndex<m_pngWidth; ++columnIndex)
	{
		png_byte* ptr = &(row[columnIndex*m_colorChannels]);

		COLORREF pixelColor = RGB(ptr[0], ptr[1], ptr[2]);
		if (lineColor != pixelColor)
			return false;
	}
	return true;
}
//Only support 24 and 32 bits pixel depth png file
RESERROR RPicture::ReadPngFile(LPCWSTR wszFilePath)
{
	int multiByteLen = WideCharToMultiByte(CP_ACP, 0, wszFilePath, -1, NULL, 0, NULL, NULL);
	char* file_name = new char[multiByteLen + 1];
	WideCharToMultiByte(CP_ACP, 0, wszFilePath, -1, file_name, multiByteLen, NULL, NULL);

	/* open file and test for it being a png */
	FILE *fp = fopen(file_name, "rb");
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

	m_pngWidth   = png_get_image_width(m_pngStructPtr, m_pngInfoPtr);
	m_pngHeight  = png_get_image_height(m_pngStructPtr, m_pngInfoPtr);
	m_colorType  = png_get_color_type(m_pngStructPtr, m_pngInfoPtr);
	m_bitDepth   = png_get_bit_depth(m_pngStructPtr, m_pngInfoPtr);
	m_colorChannels = png_get_channels(m_pngStructPtr, m_pngInfoPtr);

	int number_of_passes = png_set_interlace_handling(m_pngStructPtr);
	png_read_update_info(m_pngStructPtr, m_pngInfoPtr);


	/* read file */
	if (setjmp(png_jmpbuf(m_pngStructPtr)))
		return RES_ERROR_PARSE_FILE_FALIED;

	m_rowPointers = (png_bytep*) malloc(sizeof(png_bytep) * m_pngHeight);
	//m_rowPointers = new png_bytep[(sizeof(png_bytep) * m_pngHeight)];
	
	//ID2D1HwndRenderTarget::CreateBitmap only support continuous png pixel data in memory
	//allocate a continuous memory for m_rowPointers so that class "Image"  can return 
	//m_rowPointers directly in GetPngPixelArray
	png_uint_32 rowSize = png_get_rowbytes(m_pngStructPtr,m_pngInfoPtr);
	png_byte* pngPixelData = (png_byte*) malloc(rowSize * m_pngHeight);
	for (unsigned int rowIndex=0; rowIndex<m_pngHeight; ++rowIndex)
	{
		png_byte *rowHead = (png_byte*)((int)pngPixelData + rowIndex * rowSize);
		m_rowPointers[rowIndex] = (png_byte*) rowHead;
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
//this function inspect the png data and png info that build function "CreatePicByData" 
//if we can create a png file in disk with "m_rowPointers" and "m_pngInfoPtr"
//the data must be definitely correct!
RESERROR RPicture::WritePngFile(LPCWSTR wszFilePath)
{
	int multiByteLen = WideCharToMultiByte(CP_ACP, 0, wszFilePath, -1, NULL, 0, NULL, NULL);
	char* file_name = new char[multiByteLen + 1];
	WideCharToMultiByte(CP_ACP, 0, wszFilePath, -1, file_name, multiByteLen, NULL, NULL);

	/* create file */
	FILE *fp = fopen(file_name, "wb");
	if (!fp)
		return RES_ERROR_FILE_NOT_FOUND;


	/* initialize stuff */
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr)
		return RES_ERROR_PARSE_FILE_FALIED;

	png_infop info_ptr = png_create_info_struct(png_ptr);//m_pngInfoPtr;
	if (!info_ptr)
		return RES_ERROR_PARSE_FILE_FALIED;

	if (setjmp(png_jmpbuf(png_ptr)))
		return RES_ERROR_PARSE_FILE_FALIED;

	png_init_io(png_ptr, fp);


	/* write header */
	if (setjmp(png_jmpbuf(png_ptr)))
		return RES_ERROR_PARSE_FILE_FALIED;

	png_set_IHDR(png_ptr, info_ptr, m_pngWidth, m_pngHeight,
		m_bitDepth, m_colorType, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	//write png file info to file
	png_write_info(png_ptr, info_ptr);


	/* write bytes */
	if (setjmp(png_jmpbuf(png_ptr)))
		return RES_ERROR_PARSE_FILE_FALIED;

	png_write_image(png_ptr, m_rowPointers);


	/* end write */
	if (setjmp(png_jmpbuf(png_ptr)))
		return RES_ERROR_PARSE_FILE_FALIED;

	png_write_end(png_ptr, NULL);

	fclose(fp);

	png_destroy_write_struct(&png_ptr, &info_ptr);
	return RES_SUCCESS;
}

RESERROR RPicture::CreatePicByData(LPCSTR szResID, png_bytep* rowPointers, unsigned int width, unsigned int height, png_byte bitDepth, png_byte colorType)
{
	m_pngStructPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!m_pngStructPtr)
	{
		//m_resError = RES_ERROR_PARSE_FILE_FALIED;
		return RES_ERROR_PARSE_FILE_FALIED;
	}

	m_pngInfoPtr = png_create_info_struct(m_pngStructPtr);
	if (!m_pngInfoPtr)
	{
		//m_resError = RES_ERROR_PARSE_FILE_FALIED;
		return RES_ERROR_PARSE_FILE_FALIED;
	}

	png_set_IHDR(m_pngStructPtr, m_pngInfoPtr, width, height,
		bitDepth, colorType, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	//png_write_info(m_pngStructPtr, m_pngInfoPtr);

	m_pngWidth    = width;
	m_pngHeight   = height;
	m_bitDepth    = bitDepth;
	m_colorType   = colorType;
	m_rowPointers = rowPointers;
	m_colorChannels = png_get_channels(m_pngStructPtr, m_pngInfoPtr);
	//m_pngInfoPtr->pixel_depth = m_pixelDepth;

	return RES_SUCCESS;
}

RImage::RImage(LPCWSTR wszResPath, LPCSTR szResID)
{
	//LoadResource(wszResPath);
	SetResID(szResID);
	ReadPngFile(wszResPath);
}

RImage::RImage(LPCSTR szResID, 
			   png_bytep* rowPointers, 
			   unsigned int width, 
			   unsigned int height, 
			   png_byte bitDepth, 
			   png_byte colorType)
{
	SetResID(szResID);
	m_resError = CreatePicByData(szResID, rowPointers, width, height, bitDepth, colorType);
}

RESERROR RImage::LoadResource(LPCWSTR wszResPath)
{
	return RES_SUCCESS;
}
RESERROR RImage::CreateD2D1Bitmap(ID2D1RenderTarget* pRenderTarget)
{
	D2D1_PIXEL_FORMAT pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);
	D2D1_BITMAP_PROPERTIES properties = {pixelFormat, 96.0, 96.0};
	ID2D1Bitmap *pBitmap = NULL;
	HRESULT hr = pRenderTarget->CreateBitmap(D2D1::SizeU(m_pngWidth, m_pngHeight), (void *)m_rowPointers[0], m_pngWidth*m_colorChannels, properties, &pBitmap);
	if (SUCCEEDED(hr))
	{
		m_arrD2D1Bitmap.push_back(pBitmap);
		return RES_SUCCESS;
	}
	return RES_ERROR_CREATE_D2D1BITMAP_FAILED;
}
RESERROR RImage::Draw(ID2D1RenderTarget* pRenderTarget, UINT left, UINT top, UINT right, UINT bottom)
{
	if (m_arrD2D1Bitmap.size() == 0)
	{
		CreateD2D1Bitmap(pRenderTarget);
	}
	D2D1_RECT_F dstRect = D2D1::RectF((FLOAT)left ,(FLOAT)top, (FLOAT)right, (FLOAT)bottom);
	pRenderTarget->DrawBitmap(m_arrD2D1Bitmap[0], dstRect);
	return RES_SUCCESS;
}

RESERROR RTexture::LoadResource(LPCWSTR wszResPath)
{
	return RES_SUCCESS;
}


//detect the dividing line(RGB: 255,0,255)
RESERROR RTexture::DetectVerticalLine()
{
	//before calling this function,make sure that
	//png file must has been loaded to memory successfully.
	png_byte* pixelDataPtr = NULL;
	
	for (unsigned int columnIndex=0; columnIndex<m_pngWidth; ++columnIndex)
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
RESERROR RTexture::DetectHorizontalLine()
{
	//before calling this function,make sure that
	//png file must has been loaded to memory successfully.
	png_byte* pixelDataPtr = NULL;
	
	for (unsigned int rowIndex=0; rowIndex<m_pngHeight; ++rowIndex)
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
RESERROR RTexture::ProcessTexture()
{
	if (THREE_V == m_textureType)
	{
		m_resError = DetectVerticalLine();
		if (RES_SUCCESS == m_resError)
		{

		}
	}else if (NINE == m_textureType)
	{
		DetectVerticalLine();
		DetectHorizontalLine();
	}else if (THREE_H == m_textureType)
	{
		DetectHorizontalLine();
	}else
		abort();
	return RES_SUCCESS;
}

void RTexture::SetTextureType(LPCSTR resID)
{
	string strResID = resID;
	if (string::npos != strResID.find("ThreeV"))
		m_textureType = THREE_V;
	else if (string::npos != strResID.find("Nine"))
		m_textureType = NINE;
	else if (string::npos != strResID.find("ThreeH"))
		m_textureType = THREE_H;
	else
		abort();
}
void RTexture::InitMemberVariable()
{

}
string num2str(UINT i)
{
	char str[25] = {0};
	str[0] = '0';
	str[1] = '0';
	str[2] = '0';
	if (i<10)
	{
		_itoa(i, &str[2],10);
	}else if (i<100)
	{
		_itoa(i, &str[1],10);
	}else
	{
		_itoa(i, str,10);
	}
	
	string ret(str);
	return ret;
}
void PrintPngPixelData(png_byte* pngData, UINT pixelWidth, UINT pixelHeight, UINT pixelColorDepth)
{
	std::ofstream outFile("E:\\data.txt", ios::app);

	UINT pngRowSize = pixelWidth*pixelColorDepth/8;
	UINT size = pngRowSize*pixelHeight;
	UCHAR Red = 0;
	UCHAR Green = 0;
	UCHAR Blue = 0;
	UCHAR Alpha = 0;
	for (UINT index=0; index<size; ++index)
	{
		if (index%pngRowSize == 0)
			outFile<<"["<<num2str(index)<<"]";

		Red   = pngData[index];
		Green = pngData[++index];
		Blue  = pngData[++index];
		outFile<<"("<<num2str(Red)<<","<<num2str(Green)<<","<<num2str(Blue);
		if (pixelColorDepth == 32)
		{
			Alpha = pngData[++index];
			outFile<<","<<num2str(Alpha);
		}
		outFile<<")";

		if (index%pngRowSize == (pngRowSize - 1))
			outFile<<endl;

	}
	outFile<<endl;
	outFile.close();
}
RTexture::RTexture():m_purpleLineColor(RGB(255,0,255))
{
	InitMemberVariable();
};
RTexture::RTexture(LPCWSTR wszResPath, LPCSTR szResID):m_purpleLineColor(RGB(255,0,255))
{
	InitMemberVariable();
	//LoadResource(wszResPath);
	SetResID(szResID);
	SetTextureType(szResID);
	m_resError = ReadPngFile(wszResPath);
	if (RES_SUCCESS == m_resError)
	{
		ProcessTexture();
	}
}
RTexture::RTexture(LPCSTR szResID, 
				   png_bytep* rowPointers, 
				   unsigned int width, 
				   unsigned int height, 
				   png_byte bitDepth, 
				   png_byte colorType
				   ):m_purpleLineColor(RGB(255,0,255))
{
	InitMemberVariable();
	SetResID(szResID);
	SetTextureType(szResID);
	m_resError = CreatePicByData(szResID, rowPointers, width, height, bitDepth, colorType);
	if (RES_SUCCESS == m_resError)
	{
		ProcessTexture();
	}
}
RESERROR RTexture::_CreateD2D1Bitmap_ThreeV(ID2D1RenderTarget* pRenderTarget)
{
	int lastBlockXEnd = -1;
	m_arrVerticalLinePos.push_back(m_pngWidth);
	for (int i=0; i<3; ++i)
	{
		if (i > 0)
			lastBlockXEnd = m_arrVerticalLinePos[i - 1];

		UINT curBlockWidth  = m_arrVerticalLinePos[i] - lastBlockXEnd - 1;
		UINT curBlockHeight = m_pngHeight;

		UINT pngRowSize = m_pngWidth*m_colorChannels;
		//curBlockData for ID2D1Bitmap must be 32bits pixel-width
		UINT curBlockRowSize = curBlockWidth*4;
		
		png_byte* curBlockData = (png_byte*) malloc(curBlockHeight*curBlockRowSize);
		memset(curBlockData, 0xff, curBlockHeight*curBlockRowSize);
		png_byte* pngData = m_rowPointers[0];

		//PrintPngPixelData(pngData, m_pngWidth, m_pngHeight, m_pixelDepth);

		if (4 == m_colorChannels)
		{
			for (UINT rowIndex=0; rowIndex<curBlockHeight; ++rowIndex)
				for (UINT columnIndex=0; columnIndex<curBlockRowSize; ++columnIndex)
				{
					UINT offset = (lastBlockXEnd + 1)*4 + rowIndex*pngRowSize + columnIndex;
					curBlockData[rowIndex*curBlockRowSize + columnIndex] = pngData[offset];
				}
		}else
		{
			for (UINT rowIndex=0; rowIndex<curBlockHeight; ++rowIndex)
				for (UINT columnPixelIndex=0; columnPixelIndex<curBlockWidth; ++columnPixelIndex)
				{
					UINT srcPngDataOffset = (lastBlockXEnd + 1)*3 + rowIndex*pngRowSize + columnPixelIndex*3;
					UINT dstBlockDataOffset = rowIndex*curBlockRowSize + columnPixelIndex*4;
					curBlockData[dstBlockDataOffset] = pngData[srcPngDataOffset];
					curBlockData[dstBlockDataOffset + 1] = pngData[srcPngDataOffset + 1];
					curBlockData[dstBlockDataOffset + 2] = pngData[srcPngDataOffset + 2];
				}
		}

		//PrintPngPixelData(curBlockData, curBlockWidth, curBlockHeight, 32);

		//NOTE: the png pixel data got from libpng is (R8G8B8A8), so the ID2D1Bitmap must use the DXGI_FORMAT_R8G8B8A8_UNORM
		//mode, if png pixel depth is 24, the Alpha channel will be filled with 255
		D2D1_PIXEL_FORMAT pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);
		D2D1_BITMAP_PROPERTIES properties = {pixelFormat, 96.0, 96.0};
		ID2D1Bitmap *pBitmap = NULL;
		HRESULT hr = pRenderTarget->CreateBitmap(D2D1::SizeU(curBlockWidth, curBlockHeight), (void *)curBlockData, curBlockRowSize , properties, &pBitmap);
		if (SUCCEEDED(hr))
		{
			m_arrD2D1Bitmap.push_back(pBitmap);
		}
		//delete curBlockData
		free(curBlockData);
	}

	m_arrVerticalLinePos.pop_back();
	return RES_SUCCESS;
}
RESERROR RTexture::_CreateD2D1Bitmap_ThreeH(ID2D1RenderTarget* pRenderTarget)
{
	int lastBlockYEnd = -1;
	m_arrHorizontalLinePos.push_back(m_pngHeight);
	for (int i=0; i<3; ++i)
	{
		if (i > 0)
			lastBlockYEnd = m_arrHorizontalLinePos[i - 1];

		UINT curBlockWidth  = m_pngWidth;
		UINT curBlockHeight = m_arrHorizontalLinePos[i] - lastBlockYEnd - 1;

		UINT pngRowSize = m_pngWidth*m_colorChannels;
		UINT curBlockRowSize = curBlockWidth*4;
		//curBlockData for ID2D1Bitmap must be 32bits pixel-width
		png_byte* curBlockData = (png_byte*) malloc(curBlockHeight*curBlockRowSize);
		memset(curBlockData, 0xff, curBlockHeight*curBlockRowSize);
		png_byte* pngData = m_rowPointers[0];

		//PrintPngPixelData(pngData, m_pngWidth, m_pngHeight, m_pixelDepth);

		if (4 == m_colorChannels)
		{
			for (UINT rowIndex=0; rowIndex<curBlockHeight; ++rowIndex)
				for (UINT columnIndex=0; columnIndex<curBlockRowSize; ++columnIndex)
				{
					UINT offset = (rowIndex + lastBlockYEnd + 1)*pngRowSize + columnIndex;
					curBlockData[rowIndex*curBlockRowSize + columnIndex] = pngData[offset];
				}
		}else
		{
			for (UINT rowIndex=0; rowIndex<curBlockHeight; ++rowIndex)
				for (UINT columnPixelIndex=0; columnPixelIndex<curBlockWidth; ++columnPixelIndex)
				{
					UINT srcPngDataOffset = (rowIndex + lastBlockYEnd + 1)*pngRowSize + columnPixelIndex*3;
					UINT dstBlockDataOffset = rowIndex*curBlockRowSize + columnPixelIndex*4;
					curBlockData[dstBlockDataOffset] = pngData[srcPngDataOffset];
					curBlockData[dstBlockDataOffset + 1] = pngData[srcPngDataOffset + 1];
					curBlockData[dstBlockDataOffset + 2] = pngData[srcPngDataOffset + 2];
				}
		}

		//PrintPngPixelData(curBlockData, curBlockWidth, curBlockHeight, 32);

		//NOTE: the png pixel data got from libpng is (R8G8B8A8), so the ID2D1Bitmap must use the DXGI_FORMAT_R8G8B8A8_UNORM
		//mode, if png pixel depth is 24, the Alpha channel will be filled with 255
		D2D1_PIXEL_FORMAT pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);
		D2D1_BITMAP_PROPERTIES properties = {pixelFormat, 96.0, 96.0};
		ID2D1Bitmap *pBitmap = NULL;
		HRESULT hr = pRenderTarget->CreateBitmap(D2D1::SizeU(curBlockWidth, curBlockHeight), (void *)curBlockData, curBlockRowSize , properties, &pBitmap);
		if (SUCCEEDED(hr))
		{
			m_arrD2D1Bitmap.push_back(pBitmap);
		}
		//delete curBlockData
		free(curBlockData);
	}

	m_arrHorizontalLinePos.pop_back();

	return RES_SUCCESS;
}

RESERROR RTexture::_CreateD2D1Bitmap_Nine(ID2D1RenderTarget* pRenderTarget)
{
	int lastBlockXEnd = -1;
	int lastBlockYEnd = -1;
	m_arrVerticalLinePos.push_back(m_pngWidth);
	m_arrHorizontalLinePos.push_back(m_pngHeight);
	for (int i=0; i<9; ++i)
	{
		if (i%3 == 0)
			lastBlockXEnd = -1;
		else
			lastBlockXEnd = m_arrVerticalLinePos[i%3 - 1];

		if (i/3 == 0)
			lastBlockYEnd = -1;
		else
			lastBlockYEnd = m_arrHorizontalLinePos[i/3 - 1];

		UINT curBlockWidth  = m_arrVerticalLinePos[i%3] - lastBlockXEnd - 1;
		UINT curBlockHeight = m_arrHorizontalLinePos[i/3] - lastBlockYEnd - 1;

		UINT pngRowSize = m_pngWidth*m_colorChannels;
		UINT curBlockRowSize = curBlockWidth*4;
		//curBlockData for ID2D1Bitmap must be 32bits pixel-width
		png_byte* curBlockData = (png_byte*) malloc(curBlockHeight*curBlockRowSize);
		memset(curBlockData, 0xff, curBlockHeight*curBlockRowSize);
		png_byte* pngData = m_rowPointers[0];

		//PrintPngPixelData(pngData, m_pngWidth, m_pngHeight, m_pixelDepth);

		if (4 == m_colorChannels)
		{
			for (UINT rowIndex=0; rowIndex<curBlockHeight; ++rowIndex)
				for (UINT columnIndex=0; columnIndex<curBlockRowSize; ++columnIndex)
				{
					UINT offset = (lastBlockXEnd + 1)*4 + (rowIndex + lastBlockYEnd + 1)*pngRowSize + columnIndex;
					curBlockData[rowIndex*curBlockRowSize + columnIndex] = pngData[offset];
					if (columnIndex%4 == 3)
					{
						curBlockData[rowIndex*curBlockRowSize + columnIndex] = 180;
					}
				}
		}else
		{
			for (UINT rowIndex=0; rowIndex<curBlockHeight; ++rowIndex)
				for (UINT columnPixelIndex=0; columnPixelIndex<curBlockWidth; ++columnPixelIndex)
				{
					UINT srcPngDataOffset = (lastBlockXEnd + 1)*3 + (rowIndex + lastBlockYEnd + 1)*pngRowSize + columnPixelIndex*3;
					UINT dstBlockDataOffset = rowIndex*curBlockRowSize + columnPixelIndex*4;
					curBlockData[dstBlockDataOffset] = pngData[srcPngDataOffset];
					curBlockData[dstBlockDataOffset + 1] = pngData[srcPngDataOffset + 1];
					curBlockData[dstBlockDataOffset + 2] = pngData[srcPngDataOffset + 2];
				}
		}

		//PrintPngPixelData(curBlockData, curBlockWidth, curBlockHeight, 32);

		//NOTE: the png pixel data got from libpng is (R8G8B8A8), so the ID2D1Bitmap must use the DXGI_FORMAT_R8G8B8A8_UNORM
		//mode, if png pixel depth is 24, the Alpha channel will be filled with 255
		D2D1_PIXEL_FORMAT pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);
		D2D1_BITMAP_PROPERTIES properties = {pixelFormat, 96.0, 96.0};
		ID2D1Bitmap *pBitmap = NULL;
		HRESULT hr = pRenderTarget->CreateBitmap(D2D1::SizeU(curBlockWidth, curBlockHeight), (void *)curBlockData, curBlockRowSize , properties, &pBitmap);
		if (SUCCEEDED(hr))
		{
			m_arrD2D1Bitmap.push_back(pBitmap);
		}
		//delete curBlockData
		free(curBlockData);
	}

	m_arrVerticalLinePos.pop_back();
	m_arrHorizontalLinePos.pop_back();

	return RES_SUCCESS;
}
RESERROR RTexture::CreateD2D1Bitmap(ID2D1RenderTarget* pRenderTarget)
{
	if (THREE_V == m_textureType)
	{
		return _CreateD2D1Bitmap_ThreeV(pRenderTarget);
	}else if (NINE == m_textureType)
	{
		return _CreateD2D1Bitmap_Nine(pRenderTarget);

	}else if (THREE_H == m_textureType)
	{
		return _CreateD2D1Bitmap_ThreeH(pRenderTarget);
	}else
		abort();
		
	return RES_SUCCESS;
}
RESERROR RTexture::_Draw_Nine(ID2D1RenderTarget* pRenderTarget, UINT left, UINT top, UINT right, UINT bottom)
{
	D2D1_RECT_F dstRect = D2D1::RectF((FLOAT)left ,(FLOAT)top, (FLOAT)right, (FLOAT)bottom);
	ID2D1Bitmap *pBitmap = NULL;

	pBitmap		   = m_arrD2D1Bitmap[0];
	dstRect.right  = (FLOAT)(left + m_arrVerticalLinePos[0]);
	dstRect.bottom = (FLOAT)(top + m_arrHorizontalLinePos[0]); 
	pRenderTarget->DrawBitmap(pBitmap, dstRect);

	pBitmap		   = m_arrD2D1Bitmap[1];
	dstRect.left   = dstRect.right;
	dstRect.right  = (FLOAT)(right - (m_pngWidth - m_arrVerticalLinePos[1]) + 1);
	pRenderTarget->DrawBitmap(pBitmap, dstRect);

	pBitmap		   = m_arrD2D1Bitmap[2];
	dstRect.left   = dstRect.right;
	dstRect.right  = (FLOAT)right;
	pRenderTarget->DrawBitmap(pBitmap, dstRect);

	pBitmap		   = m_arrD2D1Bitmap[3];
	dstRect.left   = (FLOAT)left;
	dstRect.top	   = dstRect.bottom;
	dstRect.right  = (FLOAT)(left + m_arrVerticalLinePos[0]);
	dstRect.bottom = (FLOAT)(bottom - (m_pngHeight - m_arrHorizontalLinePos[1]) + 1);
	pRenderTarget->DrawBitmap(pBitmap, dstRect);

	pBitmap		   = m_arrD2D1Bitmap[4];
	dstRect.left   = dstRect.right;
	dstRect.right  = (FLOAT)(right - (m_pngWidth - m_arrVerticalLinePos[1]) + 1);
	pRenderTarget->DrawBitmap(pBitmap, dstRect);

	pBitmap		   = m_arrD2D1Bitmap[5];
	dstRect.left   = dstRect.right;
	dstRect.right  = (FLOAT)right;
	pRenderTarget->DrawBitmap(pBitmap, dstRect);

	pBitmap		   = m_arrD2D1Bitmap[6];
	dstRect.left   = (FLOAT)left;
	dstRect.right  = (FLOAT)(left + m_arrVerticalLinePos[0]);
	dstRect.top    = dstRect.bottom;
	dstRect.bottom = (FLOAT)bottom;
	pRenderTarget->DrawBitmap(pBitmap, dstRect);

	pBitmap		   = m_arrD2D1Bitmap[7];
	dstRect.left   = dstRect.right;
	dstRect.right  = (FLOAT)(right - (m_pngWidth - m_arrVerticalLinePos[1]) + 1);
	pRenderTarget->DrawBitmap(pBitmap, dstRect);

	pBitmap = m_arrD2D1Bitmap[8];
	dstRect.left   = dstRect.right;
	dstRect.right  = (FLOAT)right;
	pRenderTarget->DrawBitmap(pBitmap, dstRect);

	return RES_SUCCESS;
}
RESERROR RTexture::_Draw_ThreeV(ID2D1RenderTarget* pRenderTarget, UINT left, UINT top, UINT right, UINT bottom)
{
	if (bottom - top > m_pngHeight)
	{
		bottom = top + m_pngHeight;
	}
	D2D1_RECT_F dstRect = D2D1::RectF((FLOAT)left ,(FLOAT)top, (FLOAT)right, (FLOAT)bottom);
	ID2D1Bitmap *pBitmap = NULL;

	pBitmap		   = m_arrD2D1Bitmap[0];
	dstRect.right  = (FLOAT)(left + m_arrVerticalLinePos[0]);
	pRenderTarget->DrawBitmap(pBitmap, dstRect);

	pBitmap		   = m_arrD2D1Bitmap[1];
	dstRect.left   = dstRect.right;
	dstRect.right  = (FLOAT)(right - (m_pngWidth - m_arrVerticalLinePos[1]) + 1);
	pRenderTarget->DrawBitmap(pBitmap, dstRect);

	pBitmap		   = m_arrD2D1Bitmap[2];
	dstRect.left   = dstRect.right;
	dstRect.right  = (FLOAT)right;
	pRenderTarget->DrawBitmap(pBitmap, dstRect);
	return RES_SUCCESS;
}
RESERROR RTexture::_Draw_ThreeH(ID2D1RenderTarget* pRenderTarget, UINT left, UINT top, UINT right, UINT bottom)
{
	if (right - left > m_pngWidth)
		right = left + m_pngWidth;
	
	D2D1_RECT_F dstRect = D2D1::RectF((FLOAT)left ,(FLOAT)top, (FLOAT)right, (FLOAT)bottom);
	ID2D1Bitmap *pBitmap = NULL;

	pBitmap		   = m_arrD2D1Bitmap[0];
	dstRect.bottom = (FLOAT)(top + m_arrHorizontalLinePos[0]); 
	pRenderTarget->DrawBitmap(pBitmap, dstRect);

	pBitmap		   = m_arrD2D1Bitmap[1];
	dstRect.top    = dstRect.bottom;
	dstRect.bottom = (FLOAT)(bottom -  (m_pngHeight - m_arrHorizontalLinePos[1]) + 1);
	pRenderTarget->DrawBitmap(pBitmap, dstRect);

	pBitmap		   = m_arrD2D1Bitmap[2];
	dstRect.top    = dstRect.bottom;
	dstRect.bottom = (FLOAT)bottom;
	pRenderTarget->DrawBitmap(pBitmap, dstRect);

	return RES_SUCCESS;
}
RESERROR RTexture::Draw(ID2D1RenderTarget* pRenderTarget, UINT left, UINT top, UINT right, UINT bottom)
{
	if (m_arrD2D1Bitmap.size() == 0)
	{
		CreateD2D1Bitmap(pRenderTarget);
	}

	if (THREE_V == m_textureType)
	{
		return _Draw_ThreeV(pRenderTarget, left, top, right, bottom);
	}else if (NINE == m_textureType)
	{
		return _Draw_Nine(pRenderTarget, left, top, right, bottom);
	}else if (THREE_H == m_textureType)
	{
		return _Draw_ThreeH(pRenderTarget, left, top, right, bottom);
	}

	return RES_SUCCESS;
}

RPicList::RPicList():m_purpleLineColor(RGB(127,0,127))
{

}
RPicList::RPicList(LPCWSTR wszResPath, LPCSTR resID):m_purpleLineColor(RGB(127,0,127))
{
	//LoadResource(wszResPath);
	SetResID(resID);
	SetPicListType(resID);
	m_resError = ReadPngFile(wszResPath);
	if (RES_SUCCESS != m_resError)
		return;

	m_resError = DetectVerticalLine();
	if (RES_SUCCESS != m_resError)
		return;

	m_resError = CreatePicFromMem();
}
RPicList::~RPicList()
{
	for (unsigned int i=0; i<m_picListVector.size(); ++i)
		delete m_picListVector[i];
}
RPicture* RPicList::GetPicByIndex(unsigned int index)
{
	if (m_picListVector.size() <= index)
		return NULL;
	return m_picListVector[index];
}
//RPicList do not need a draw function
RESERROR RPicList::Draw(ID2D1RenderTarget* pRenderTarget, UINT left, UINT top, UINT right, UINT bottom)
{ 
	return RES_SUCCESS;
}
void RPicList::SetPicListType(LPCSTR resID)
{
	string strResID = resID;
	if (0 == strResID.find("texturelist"))
		m_picListType = TEXTURELIST;
	else if (0 == strResID.find("imagelist"))
		m_picListType = IMAGELIST;
	else
		abort();
}
RESERROR RPicList::LoadResource(LPCWSTR wszResPath)
{
	//++++++++++++++++++++++++++++++++++++++++++
	wstring wszFileName;
	wszFileName = ::PathFindFileName(wszResPath);
	std::size_t iBeginPos = wszFileName.find(L"texturelist");
	if (0 == iBeginPos)
	{
	}
	std::size_t iBeginPosEx = wszFileName.find(L"imagelist");
	
	return RES_SUCCESS;
}

//detect the dividing line(RGB: 127,0,127)
//a pictrue list's dividing line must be vertical
RESERROR RPicList::DetectVerticalLine()
{
	//before calling this function,make sure that
	//png file *must* has been loaded to memory successfully.
	png_byte* pixelDataPtr = NULL;

	for (unsigned int columnIndex=0; columnIndex<m_pngWidth; ++columnIndex)
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
RESERROR RPicList::CreatePicFromMem()
{
	if (m_arrVerticalLinePos.size() <= 0)
		return RES_ERROR_UNKNOWN;

	png_bytep* pngRowPtr = NULL;
	vector<unsigned int>::iterator iter = m_arrVerticalLinePos.begin();
	
	//a virtual dividing line before 0 column
	int lastDivideLinePos = -1;
	//a virtual dividing line after last column
	m_arrVerticalLinePos.push_back(m_pngWidth);
	
	for (unsigned int verticalLineIndex = 0; verticalLineIndex < m_arrVerticalLinePos.size(); ++verticalLineIndex)
	{
		unsigned int curDivideLinePos = m_arrVerticalLinePos[verticalLineIndex];
		unsigned int newWidth = curDivideLinePos - lastDivideLinePos - 1;
		pngRowPtr = (png_bytep*) malloc(m_pngHeight * sizeof(png_bytep));
		
		//The same with RPicture::ReadPngFile
		//ID2D1HwndRenderTarget::CreateBitmap only support continuous png pixel data in memory
		//allocate a continuous memory for m_rowPointers so that class "Image"  can return 
		//m_rowPointers directly in GetPngPixelArray
		png_byte* pngPixelData = (png_byte*) malloc(m_pngHeight*newWidth*m_colorChannels);

		
		for (unsigned int rowIndex=0; rowIndex < m_pngHeight; ++rowIndex)
		{
			png_byte *rowHead = (png_byte*)((int)pngPixelData + rowIndex * newWidth * m_colorChannels);
			pngRowPtr[rowIndex] = (png_byte*) rowHead;
			//pngRowPtr[rowIndex] = (png_byte*) malloc(newWidth*bytesPerPixel);
			
			//copy png pixel data row by row, it takes *four* bytes per pixel
			for (unsigned int columnIndex=0; columnIndex < newWidth; ++columnIndex)
			{
				unsigned int srcPngPixelPos = lastDivideLinePos + columnIndex + 1;
				png_byte* dstPtr = &(pngRowPtr[rowIndex][columnIndex * m_colorChannels]);
				png_byte* srcPtr = &(m_rowPointers[rowIndex][srcPngPixelPos * m_colorChannels]);

				for (unsigned int i=0; i < m_colorChannels; ++i)
					dstPtr[i] = srcPtr[i];
			}

		}
		lastDivideLinePos = curDivideLinePos;

		char szIndex[10]={0};
		_itoa(verticalLineIndex+1, szIndex, 10);
		string textureId = m_szResID + '.';
		textureId += szIndex;
		RPicture* pPicObj = NULL;
		if (TEXTURELIST == m_picListType)
			pPicObj = new RTexture(textureId.c_str(), pngRowPtr, newWidth, m_pngHeight, m_bitDepth, m_colorType);
		else
			pPicObj = new RImage(textureId.c_str(), pngRowPtr, newWidth, m_pngHeight, m_bitDepth, m_colorType);
		
		m_picListVector.push_back(pPicObj);
	}

	m_arrVerticalLinePos.pop_back();
	return RES_SUCCESS;
}