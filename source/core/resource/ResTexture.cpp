#include "stdafx.h"
#include "ResTexture.h"
#include "ResManager.h"

/*//////////////////////////////////����texture��ʹ��/////////////////////////////////////////
*texture�����ָ�ʽ��ThreeH/ThreeV/NineInOne��������Ⱦ����:��ͼ����/��ͼ����
*
*   1,��ͼ���ԡ�������ResImage, GetD2D1Bitmap���ݲ�����Ҫ��ĳߴ磬����һ��bitmap���ڴ�飬��
*�ظ�������һ��Bitmap�������߸��𽫸�Bitmap���Ƴ�������ģʽ�ʺϡ���̬��С���Ŀؼ�����ν�ľ�̬��
*С����ָ�ؼ�������֮�����û����������в�������С�����߱����Ƶ�ʷǳ��ͣ���������󲿷ֵİ�
*ť���������Ļ��顢�˵��ı����ȡ�
*
*   2����ͼ���ԡ���texture���ڴ��ڱ���ʱ����������һ��Ͳ�̫������:���ڿ��ܱ��û�Ƶ���ر����С��
*����resizeʱ��Ƶ������GetD2D1Bitmap������һ���³ߴ��bitmap�����پɵ�bitmap���������Ƶ������
*����䡢�����뿽������ͼ���Ի���texture�����ڲ�ά��һ��tile(��Ƭ)�б�(NineInOne���ǾŸ�tiles)��
*GetD2D1Bitmapֱ�ӷ���null��������ת��ʹ��OnDrawImplEx���ƣ��÷�������ݴ������ľ��δ�С/λ�ã�
*�����ʵ���tile��ÿ��tile����һ��������Bitmap����ʡȥ��Ƶ���ڴ����Ĵ��롣
*
*UIBitmap��һ������:frequent��Ĭ��ֵΪ0(ʹ�õ�ͼ����)���ɿ������߸���Ԫ�ؾ���������趨��
*//////////////////////////////////////////////////////////////////////////////////////////////

using namespace Gear::Res;
ResTexture::ResTexture()
	: m_lastQueryWidth(0)
	, m_purpleLineColor(RGB(255, 0, 255))
	, m_lastQueryHeight(0)
{
	InitAttrMap();
	InitAttrValuePatternMap();
	InitAttrValueParserMap();
}

ResTexture::ResTexture(const string& strImageDesc)
	: m_lastQueryWidth(0)
	, m_purpleLineColor(RGB(255, 0, 255))
	, m_lastQueryHeight(0)
{
	m_wstrFilePath = ResManager::GetInstance().GetResFilePathByName(strImageDesc);
	if (m_wstrFilePath.empty()) {
		ERR("Create ResImage failed! cannot find image file");
		return;
	}
}
ResTexture::ResTexture(const wstring& wstrPath)
	: ResPicture(wstrPath)
	, m_lastQueryWidth(0)
	, m_lastQueryHeight(0)
	, m_purpleLineColor(RGB(255, 0, 255))
{
}
ResTexture::ResTexture(png_bytep* rowPointers, png_uint_32 width, png_uint_32 height, png_byte colorType, png_byte colorChannels, png_byte bitDepth)
	: ResPicture(rowPointers, width, height, colorType, colorChannels, bitDepth)
	, m_lastQueryWidth(0)
	, m_lastQueryHeight(0)
	, m_purpleLineColor(RGB(255, 0, 255))
{
}
void ResTexture::InitAttrMap()
{
	ADD_ATTR("file", "")
}
void ResTexture::InitAttrValuePatternMap()
{
}
void ResTexture::InitAttrValueParserMap()
{
}

bool ResTexture::IsVerticalLine(unsigned int horizontalPos, const COLORREF lineColor)
{
	for (unsigned int rowIndex = 0; rowIndex < m_pngHeight; ++rowIndex){
		png_byte* row = m_rowPointers[rowIndex];
		png_byte* ptr = &(row[horizontalPos*m_colorChannels]);

		COLORREF pixelColor = RGB(ptr[0], ptr[1], ptr[2]);
		if (lineColor != pixelColor){ return false; }
	}
	return true;
}

bool ResTexture::IsHorizontalLine(unsigned int horizontalPos, const COLORREF lineColor)
{
	png_byte* row = m_rowPointers[horizontalPos];
	for (unsigned int columnIndex = 0; columnIndex < m_pngWidth; ++columnIndex){
		png_byte* ptr = &(row[columnIndex*m_colorChannels]);

		COLORREF pixelColor = RGB(ptr[0], ptr[1], ptr[2]);
		if (lineColor != pixelColor){ return false; }
	}
	return true;
}

RESERROR ResTexture::DetectVerticalLine()
{
	png_byte* pixelDataPtr = NULL;

	for (unsigned int columnIndex = 0; columnIndex < m_pngWidth; ++columnIndex){
		pixelDataPtr = &(m_rowPointers[0][columnIndex*m_colorChannels]);
		COLORREF pixelColor = RGB(pixelDataPtr[0], pixelDataPtr[1], pixelDataPtr[2]);
		if (m_purpleLineColor == pixelColor){
			if (IsVerticalLine(columnIndex, m_purpleLineColor)){
				m_arrVerticalLinePos.push_back(columnIndex);
			}
		}
	}
	return RES_SUCCESS;
}

RESERROR ResTexture::DetectHorizontalLine()
{
	png_byte* pixelDataPtr = NULL;

	for (unsigned int rowIndex = 0; rowIndex < m_pngHeight; ++rowIndex){
		pixelDataPtr = &(m_rowPointers[rowIndex][0]);
		COLORREF pixelColor = RGB(pixelDataPtr[0], pixelDataPtr[1], pixelDataPtr[2]);
		if (m_purpleLineColor == pixelColor){
			if (IsHorizontalLine(rowIndex, m_purpleLineColor)){
				m_arrHorizontalLinePos.push_back(rowIndex);
			}
		}
	}
	return RES_SUCCESS;
}
png_bytep* ResTexture::__CreateSingleBitmapBlock_NineInOn(unsigned int width, unsigned int height, unsigned int& retWidth, unsigned int& retHeight)
{
	ATLASSERT(width > m_pngWidth && height > m_pngHeight);
	png_bytep* rowPointers = AllocPngDataMem(width, height, m_colorChannels);

	//�ĸ����������죬ֱ�ӿ���
	for (png_uint_32 i = 0; i < m_arrHorizontalLinePos[0]; ++i)
		for (png_uint_32 j = 0; j < m_arrVerticalLinePos[0] * m_colorChannels; ++j) {
			rowPointers[i][j] = m_rowPointers[i][j];
		}

	png_uint_32 topRightCornerWidth =(m_pngWidth - m_arrVerticalLinePos[1] - 1);
	png_uint_32 topRightCornerHeight = m_arrHorizontalLinePos[0];
	png_uint_32 offsetXSrc = (m_arrVerticalLinePos[1] + 1)*m_colorChannels;
	png_uint_32 offsetXDst = (width - topRightCornerWidth)*m_colorChannels;
	for (png_uint_32 i = 0; i < topRightCornerHeight; ++i)
		for (png_uint_32 j = 0; j < (m_pngWidth - m_arrVerticalLinePos[1] - 1) * m_colorChannels; ++j) {
			rowPointers[i][offsetXDst + j] = m_rowPointers[i][offsetXSrc + j];
		}

	png_uint_32 bottomLeftCornerWidth = m_arrVerticalLinePos[0];
	png_uint_32 bottomLeftCornerHeight = m_pngHeight - m_arrHorizontalLinePos[1] - 1;
	png_uint_32 offsetYSrc = m_pngHeight - bottomLeftCornerHeight;
	png_uint_32 offsetYDst = height - bottomLeftCornerHeight;
	for (png_uint_32 i = 0; i < bottomLeftCornerHeight; ++i)
		for (png_uint_32 j = 0; j < bottomLeftCornerWidth * m_colorChannels; ++j) {
			rowPointers[offsetYDst + i][j] = m_rowPointers[offsetYSrc + i][j];
		}

	png_uint_32 bottomRightCornerWidth = m_pngWidth - m_arrVerticalLinePos[1] - 1;
	png_uint_32 bottomRightCornerHeight = m_pngHeight - m_arrHorizontalLinePos[1] - 1;
	offsetXSrc = (m_pngWidth - bottomRightCornerWidth)*m_colorChannels;
	offsetXDst = (width - bottomRightCornerWidth)*m_colorChannels;
	offsetYSrc = m_pngHeight - bottomRightCornerHeight;
	offsetYDst = height - bottomRightCornerHeight;
	for (png_uint_32 i = 0; i < bottomRightCornerHeight; ++i)
		for (png_uint_32 j = 0; j < bottomRightCornerWidth * m_colorChannels; ++j) {
			rowPointers[offsetYDst + i][offsetXDst + j] = m_rowPointers[offsetYSrc + i][offsetXSrc + j];
		}

	//�ĸ��ߣ�����
	png_uint_32 leftEdgeWidth = m_arrVerticalLinePos[0];
	png_uint_32 leftEdgeHeight = height - m_arrHorizontalLinePos[0] - bottomLeftCornerHeight;
	for (png_uint_32 i = 0; i < leftEdgeHeight; ++i)
		for (size_t j = 0; j < leftEdgeWidth * m_colorChannels; j++) {
			rowPointers[i + m_arrHorizontalLinePos[0]][j] = m_rowPointers[m_arrHorizontalLinePos[0] + 1][j];
		}

	png_uint_32 topEdgeWidth = width - m_arrVerticalLinePos[0] - topRightCornerWidth;
	png_uint_32 topEdgeHeight = m_arrHorizontalLinePos[0];
	offsetXDst = m_arrVerticalLinePos[0] * m_colorChannels;
	for (png_uint_32 i = 0; i < topEdgeHeight; ++i)
		for (png_uint_32 j = 0; j < topEdgeWidth * m_colorChannels; j++) {
			rowPointers[i][j + offsetXDst] = m_rowPointers[i][offsetXDst + m_colorChannels + j%m_colorChannels];
		}

	png_uint_32 rightEdgeWidth = topRightCornerWidth;
	png_uint_32 rightEdgeHeight = height - topRightCornerHeight - bottomRightCornerHeight;
	offsetXDst = (width - rightEdgeWidth)*m_colorChannels;
	offsetXSrc = (m_arrVerticalLinePos[1] + 1)*m_colorChannels;
	for (png_uint_32 i = 0; i < rightEdgeHeight; ++i)
		for (png_uint_32 j = 0; j < rightEdgeWidth*m_colorChannels; ++j) {
			rowPointers[i + topRightCornerHeight][j + offsetXDst] = m_rowPointers[topRightCornerHeight + 1][j + offsetXSrc];
		}

	png_uint_32 bottomEdgeWidth = width - bottomLeftCornerWidth - bottomRightCornerWidth;
	png_uint_32 bottomEdgeHeight = bottomLeftCornerHeight;
	offsetYDst = height - bottomEdgeHeight;
	offsetYSrc = m_arrHorizontalLinePos[1] + 1;
	offsetXDst = m_arrVerticalLinePos[0] * m_colorChannels;
	offsetXSrc = (m_arrVerticalLinePos[0] + 1)*m_colorChannels;
	for (png_uint_32 i = 0; i < bottomEdgeHeight; ++i)
		for (png_uint_32 j = 0; j < bottomEdgeWidth *m_colorChannels; ++j) {
			rowPointers[i + offsetYDst][j + offsetXDst] = m_rowPointers[offsetYSrc + i][j%m_colorChannels + offsetXSrc];
		}

	png_uint_32 centerWidth = topEdgeWidth;
	png_uint_32 centerHeight = leftEdgeHeight;
	offsetXDst = leftEdgeWidth*m_colorChannels;
	for (png_uint_32 i = 0; i < centerHeight; ++i)
		for (png_uint_32 j = 0; j < centerWidth *m_colorChannels; ++j) {
			rowPointers[m_arrHorizontalLinePos[0] + i][offsetXDst + j] = m_rowPointers[m_arrHorizontalLinePos[0] + 1][(m_arrVerticalLinePos[0] + 1)*m_colorChannels + j%m_colorChannels];
		}

	retWidth  = width;
	retHeight = height;
	return rowPointers;
}

png_bytep* ResTexture::__CreateSingleBitmapBlock_ThreeH(unsigned int width, unsigned int height, unsigned int& retWidth, unsigned int& retHeight)
{
	png_bytep* rowPointers = AllocPngDataMem(width, m_pngHeight, m_colorChannels);

	//����ֱ�ӿ���
	png_uint_32 leftPartWidth = m_arrVerticalLinePos[0];
	png_uint_32 leftPartHeight = m_pngHeight;
	for (png_uint_32 i = 0; i < leftPartHeight; ++i)
		for (png_uint_32 j = 0; j < leftPartWidth*m_colorChannels; ++j) {
			rowPointers[i][j] = m_rowPointers[i][j];
		}
	png_uint_32 rightPartWidth = m_pngWidth - m_arrVerticalLinePos[1] - 1;
	png_uint_32 rightPartHeight = m_pngHeight;
	png_uint_32 offsetXSrc = (m_arrVerticalLinePos[1] + 1)*m_colorChannels;
	png_uint_32 offsetXDst = (width - rightPartWidth)*m_colorChannels;
	for (png_uint_32 i = 0; i < rightPartHeight; ++i)
		for (png_uint_32 j = 0; j < rightPartWidth*m_colorChannels; ++j) {
			rowPointers[i][j + offsetXDst] = m_rowPointers[i][j + offsetXSrc];
		}

	png_uint_32 centerWidth = width - leftPartWidth - rightPartWidth;
	offsetXDst = (m_arrVerticalLinePos[0])*m_colorChannels;
	for (png_uint_32 i = 0; i < m_pngHeight; ++i)
		for (png_uint_32 j = 0; j < centerWidth*m_colorChannels; ++j) {
			rowPointers[i][j + offsetXDst] = m_rowPointers[i][j%m_colorChannels + offsetXDst + m_colorChannels];
		}

	retWidth  = width;
	retHeight = m_pngHeight;
	return rowPointers;
}

png_bytep* ResTexture::__CreateSingleBitmapBlock_ThreeV(unsigned int width, unsigned int height, unsigned int& retWidth, unsigned int& retHeight)
{
	png_bytep* rowPointers = AllocPngDataMem(m_pngWidth, height, m_colorChannels);

	png_uint_32 topPartWidth = m_pngWidth;
	png_uint_32 topPartHeight = m_arrHorizontalLinePos[0];
	for (png_uint_32 i = 0; i < topPartHeight; ++i)
		for (png_uint_32 j = 0; j < topPartWidth*m_colorChannels; ++j) {
			rowPointers[i][j] = m_rowPointers[i][j];
		}

	png_uint_32 bottomPartWidth = m_pngWidth;
	png_uint_32 bottomPartHeight = m_pngHeight - m_arrHorizontalLinePos[1] - 1;
	png_uint_32 offsetYSrc = (m_arrHorizontalLinePos[1] + 1);
	png_uint_32 offsetYDst = height - bottomPartHeight;
	for (png_uint_32 i = 0; i < bottomPartHeight; ++i)
		for (png_uint_32 j = 0; j < bottomPartWidth*m_colorChannels; ++j) {
			rowPointers[i + offsetYDst][j] = m_rowPointers[i + offsetYSrc][j];
		}

	png_uint_32 centerWidth = m_pngWidth;
	png_uint_32 centerHeight = height - topPartHeight - bottomPartHeight;
	offsetYSrc = (m_arrHorizontalLinePos[0] + 1);
	offsetYDst = topPartHeight;
	for (png_uint_32 i = 0; i < centerHeight; ++i)
		for (png_uint_32 j = 0; j < centerWidth*m_colorChannels; ++j) {
			rowPointers[i + offsetYDst][j] = m_rowPointers[offsetYSrc][j];
		}

	retWidth  = m_pngWidth;
	retHeight = height;
	return rowPointers;
}

png_bytep* ResTexture::CreateAndCopySubBlock(unsigned int subWidth, unsigned int subHeight, unsigned int offsetXInPix, unsigned int offsetYInPix)
{
	png_bytep* rowPointers = AllocPngDataMem(subWidth, subHeight, m_colorChannels);
	offsetXInPix = offsetXInPix*m_colorChannels;
	for (png_uint_32 i = 0; i < subHeight; ++i)
		for (png_uint_32 j = 0; j < subWidth*m_colorChannels; ++j) {
			rowPointers[i][j] = m_rowPointers[i + offsetYInPix][j + offsetXInPix];
		}

	return rowPointers;
}
///////////////////////////////////////Direct2D��Ⱦģʽ��ش���///////////////////////////////////
#ifdef USE_D2D_RENDER_MODE
ID2D1Bitmap* ResTexture::GetD2D1Bitmap(ID2D1RenderTarget* pRenderTarget, unsigned int width, unsigned int height, unsigned int& retWidth, unsigned int& retHeight)
{
	if (m_d2d1BitmapPtr && width == m_lastQueryWidth && height == m_lastQueryHeight) { 
		return m_d2d1BitmapPtr; 
	}
	//ɾ���ɳߴ��ͼƬ,�������һ��
	SafeRelease(&m_d2d1BitmapPtr);
	
	//��δ����png
	if (m_pngWidth == 0) {
		auto ret = ReadPngFile(m_wstrFilePath);
		if (!ret) { return nullptr; }

		//���ָ��ߣ���μӿ��⣿ texture.ThreeH?��
		DetectHorizontalLine();
		DetectVerticalLine();

		ATLASSERT(m_arrVerticalLinePos.size() == 2 || m_arrHorizontalLinePos.size() == 2);
	}

	HRESULT hr = S_OK;
	D2D1_PIXEL_FORMAT pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);
	D2D1_BITMAP_PROPERTIES properties = { pixelFormat, 96.0, 96.0 };
	D2D1_SIZE_U size;
	png_bytep* rowPointers = nullptr;
	if (m_arrVerticalLinePos.size() == 2 && m_arrHorizontalLinePos.size() == 2){
		rowPointers = __CreateSingleBitmapBlock_NineInOn(width, height, retWidth, retHeight);
	} else if (m_arrHorizontalLinePos.size() == 2) {
		rowPointers = __CreateSingleBitmapBlock_ThreeV(width, height, retWidth, retHeight);
	}else if (m_arrVerticalLinePos.size() == 2){
		rowPointers = __CreateSingleBitmapBlock_ThreeH(width, height, retWidth, retHeight);
	}

	size.width  = retWidth;
	size.height = retHeight;
	hr = pRenderTarget->CreateBitmap(size, (void*)rowPointers[0], width * m_colorChannels, properties, &m_d2d1BitmapPtr);
	
	if (FAILED(hr) || m_d2d1BitmapPtr == nullptr){
		ERR("GetD2D1Bitmap error: get null m_d2d1BitmapPtr");
		SafeDelete(&m_d2d1BitmapPtr);
	}
	return m_d2d1BitmapPtr;
}

//��GetD2D1Bitmap�������ص���ͼƬ��ͬ��OnDrawImplEx��ʹ��9��(����3��)������bitmap����һ��bitmap�����졢ƴ�ӣ�
HRESULT ResTexture::OnDrawImplEx(ID2D1RenderTarget* pRenderTarget, const D2D1_RECT_F& rcWndPos, UIObject* obj /*= nullptr*/)
{
	auto DrawNineInOne = [&]()->HRESULT {
		D2D1_RECT_F dstRect = rcWndPos;
		ID2D1Bitmap *pBitmap = NULL;

		pBitmap = m_arrD2D1Bitmap[0];
		dstRect.right = (FLOAT)(rcWndPos.left + m_arrVerticalLinePos[0]);
		dstRect.bottom = (FLOAT)(rcWndPos.top + m_arrHorizontalLinePos[0]);
		pRenderTarget->DrawBitmap(pBitmap, dstRect);

		pBitmap = m_arrD2D1Bitmap[1];
		dstRect.left = dstRect.right;
		dstRect.right = (FLOAT)(rcWndPos.right - (m_pngWidth - m_arrVerticalLinePos[1]) + 1);
		pRenderTarget->DrawBitmap(pBitmap, dstRect);

		pBitmap = m_arrD2D1Bitmap[2];
		dstRect.left = dstRect.right;
		dstRect.right = (FLOAT)rcWndPos.right;
		pRenderTarget->DrawBitmap(pBitmap, dstRect);

		pBitmap = m_arrD2D1Bitmap[3];
		dstRect.left = (FLOAT)rcWndPos.left;
		dstRect.top = dstRect.bottom;
		dstRect.right = (FLOAT)(rcWndPos.left + m_arrVerticalLinePos[0]);
		dstRect.bottom = (FLOAT)(rcWndPos.bottom - (m_pngHeight - m_arrHorizontalLinePos[1]) + 1);
		pRenderTarget->DrawBitmap(pBitmap, dstRect);

		pBitmap = m_arrD2D1Bitmap[4];
		dstRect.left = dstRect.right;
		dstRect.right = (FLOAT)(rcWndPos.right - (m_pngWidth - m_arrVerticalLinePos[1]) + 1);
		pRenderTarget->DrawBitmap(pBitmap, dstRect);

		pBitmap = m_arrD2D1Bitmap[5];
		dstRect.left = dstRect.right;
		dstRect.right = (FLOAT)rcWndPos.right;
		pRenderTarget->DrawBitmap(pBitmap, dstRect);

		pBitmap = m_arrD2D1Bitmap[6];
		dstRect.left = (FLOAT)rcWndPos.left;
		dstRect.right = (FLOAT)(rcWndPos.left + m_arrVerticalLinePos[0]);
		dstRect.top = dstRect.bottom;
		dstRect.bottom = (FLOAT)rcWndPos.bottom;
		pRenderTarget->DrawBitmap(pBitmap, dstRect);

		pBitmap = m_arrD2D1Bitmap[7];
		dstRect.left = dstRect.right;
		dstRect.right = (FLOAT)(rcWndPos.right - (m_pngWidth - m_arrVerticalLinePos[1]) + 1);
		pRenderTarget->DrawBitmap(pBitmap, dstRect);

		pBitmap = m_arrD2D1Bitmap[8];
		dstRect.left = dstRect.right;
		dstRect.right = (FLOAT)rcWndPos.right;
		pRenderTarget->DrawBitmap(pBitmap, dstRect);
		return S_OK;
	};
	auto DrawThreeH = [&]()->HRESULT {

		return S_OK;
	};
	auto DrawThreeV = [&]()->HRESULT {

		return S_OK;
	};

	//ɾ���ɵ�ͼƬ,ʹ����ģʽ����
	SafeRelease(&m_d2d1BitmapPtr);

	if (m_pngWidth == 0) {
		auto ret = ReadPngFile(m_wstrFilePath);
		if (!ret) { return S_FALSE; }

		//���ָ��ߣ���μӿ��⣿ texture.ThreeH?��
		DetectHorizontalLine();
		DetectVerticalLine();

		ATLASSERT(m_arrVerticalLinePos.size() == 2 || m_arrHorizontalLinePos.size() == 2);
	}
	
	//��δ������ͼ
	if (m_arrD2D1Bitmap.size() <= 0){
		__CreateMultiSubBitmapBlock_AllStyle();
		ATLASSERT(m_arrSubBitmapBlock.size() > 0 && m_arrSubBitmapBlock.size() == m_arrSubBitmapBlockSize.size());
		for (size_t i=0; i<m_arrSubBitmapBlock.size(); ++i){
			png_bytep* rowPointers = m_arrSubBitmapBlock[i];
			D2D1_PIXEL_FORMAT pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);
			D2D1_BITMAP_PROPERTIES properties = { pixelFormat, 96.0, 96.0 };
			D2D1_SIZE_U size;
			size.width = m_arrSubBitmapBlockSize[i].first;
			size.height = m_arrSubBitmapBlockSize[i].second;
			ID2D1Bitmap* d2d1Bitmap = nullptr;
			HRESULT hr = pRenderTarget->CreateBitmap(size, (void*)rowPointers[0], size.width * m_colorChannels, properties, &d2d1Bitmap);
			ATLASSERT(SUCCEEDED(hr));
			m_arrD2D1Bitmap.push_back(d2d1Bitmap);
			//������Ϻ�����m_arrSubBitmapBlock�е��ڴ��
			free(rowPointers[0]);
			free(rowPointers);
			m_arrSubBitmapBlock[i] = nullptr;
		}
		m_arrSubBitmapBlock.clear();
		m_arrSubBitmapBlockSize.clear();
	}

	HRESULT hr = S_OK;
	if (m_arrVerticalLinePos.size() == 2 && m_arrHorizontalLinePos.size() == 2) {
		hr = DrawNineInOne();
	} else if (m_arrHorizontalLinePos.size() == 2) {
		hr = DrawThreeV();
	} else if (m_arrVerticalLinePos.size() == 2) {
		hr = DrawThreeH();
	}
	return hr;
}
//ֻ�����ڴ�飬��ͬʱ��D2D��GDIʹ��
size_t ResTexture::__CreateMultiSubBitmapBlock_AllStyle()
{
	auto CreateNineInOneSubBitmapBlock = [&]()->void {
		m_arrVerticalLinePos.push_back(m_pngWidth);
		m_arrHorizontalLinePos.push_back(m_pngHeight);
		int lastBlockXEnd = -1, lastBlockYEnd = -1;
		for (auto i=0; i<9; ++i){
			lastBlockXEnd = (i % 3 == 0) ? -1: m_arrVerticalLinePos[i % 3 - 1];
			lastBlockYEnd = (i / 3 == 0)?-1: m_arrHorizontalLinePos[i / 3 - 1];

			unsigned int curBlockWidth  = m_arrVerticalLinePos[i % 3] - lastBlockXEnd - 1;
			unsigned int curBlockHeight = m_arrHorizontalLinePos[i / 3] - lastBlockYEnd - 1;
			unsigned int offsetXInPix   = lastBlockXEnd + 1;
			unsigned int offsetYInPix   = lastBlockYEnd + 1;
			png_bytep* rowPointers = CreateAndCopySubBlock(curBlockWidth, curBlockHeight, offsetXInPix, offsetYInPix);
			m_arrSubBitmapBlockSize.push_back(pair<unsigned int, unsigned int>(curBlockWidth, curBlockHeight));
			m_arrSubBitmapBlock.push_back(rowPointers);
		}
		m_arrVerticalLinePos.pop_back();
		m_arrHorizontalLinePos.pop_back();
	};
	auto CreateThreeHSubBitmapBlock = [&]()->void {
		int lastBlockXEnd = -1;
		m_arrVerticalLinePos.push_back(m_pngWidth);
		for (int i = 0; i < 3; ++i) {
			lastBlockXEnd = (i % 3 == 0) ? -1 : m_arrVerticalLinePos[i % 3 - 1];
			unsigned int curBlockWidth  = m_arrVerticalLinePos[i] - lastBlockXEnd - 1;
			unsigned int curBlockHeight = m_pngHeight;
			unsigned int offsetXInPix   = lastBlockXEnd + 1;
			png_bytep* rowPointers = CreateAndCopySubBlock(curBlockWidth, curBlockHeight, offsetXInPix, 0);
			m_arrSubBitmapBlockSize.push_back(pair<unsigned int, unsigned int>(curBlockWidth, curBlockHeight));
			m_arrSubBitmapBlock.push_back(rowPointers);
		}
	};
	auto CreateThreeVSubBitmapBlock = [&]()->void {
		int lastBlockYEnd = -1;
		m_arrHorizontalLinePos.push_back(m_pngHeight);
		for (auto i = 0; i < 3; ++i) {
			lastBlockYEnd = (i / 3 == 0) ? -1 : m_arrHorizontalLinePos[i / 3 - 1];

			unsigned int curBlockWidth  = m_pngWidth;
			unsigned int curBlockHeight = m_arrHorizontalLinePos[i] - lastBlockYEnd - 1;
			unsigned int offsetYInPix   = lastBlockYEnd + 1;
			png_bytep* rowPointers = CreateAndCopySubBlock(curBlockWidth, curBlockHeight, 0, offsetYInPix);
			m_arrSubBitmapBlockSize.push_back(pair<unsigned int, unsigned int>(curBlockWidth, curBlockHeight));
			m_arrSubBitmapBlock.push_back(rowPointers);
		}
		m_arrHorizontalLinePos.pop_back();
	};

	if (m_arrVerticalLinePos.size() == 2 && m_arrHorizontalLinePos.size() == 2) {
		//NineInOne���͵�texture,�����Ÿ���ͼ
		CreateNineInOneSubBitmapBlock();
	} else if (m_arrHorizontalLinePos.size() == 2) {
		//ThreeV���͵�texture
		CreateThreeVSubBitmapBlock();
	} else if (m_arrVerticalLinePos.size() == 2) {
		//ThreeH���͵�texture
		CreateThreeHSubBitmapBlock();
	} else {
		return S_FALSE;
	}
	return S_OK;
}
/////////////////////////////////////////GDI+��Ⱦģʽ��ش���/////////////////////////////////////
#else
Gdiplus::Bitmap* ResTexture::GetGDIBitmap(unsigned int width, unsigned int height, unsigned int& retWidth, unsigned int& retHeight)
{
	if (m_gdiplusBitmapPtr && width == m_lastQueryWidth && height == m_lastQueryHeight) {
		return m_gdiplusBitmapPtr;
	}
	//ɾ���ɳߴ��ͼƬ,�������һ��
	SafeDelete(&m_gdiplusBitmapPtr);

	//��δ����png
	if (m_pngWidth == 0) {
		auto ret = ReadPngFile(m_wstrFilePath);
		if (!ret) { return nullptr; }

		//���ָ��ߣ���μӿ��⣿ texture.ThreeH?��
		DetectHorizontalLine();
		DetectVerticalLine();

		ATLASSERT(m_arrVerticalLinePos.size() == 2 || m_arrHorizontalLinePos.size() == 2);
	}

	HRESULT hr = S_OK;
	png_bytep* rowPointers = nullptr;
	if (m_arrVerticalLinePos.size() == 2 && m_arrHorizontalLinePos.size() == 2) {
		rowPointers = __CreateSingleBitmapBlock_NineInOn(width, height, retWidth, retHeight);
	} else if (m_arrHorizontalLinePos.size() == 2) {
		rowPointers = __CreateSingleBitmapBlock_ThreeV(width, height, retWidth, retHeight);
	} else if (m_arrVerticalLinePos.size() == 2) {
		rowPointers = __CreateSingleBitmapBlock_ThreeH(width, height, retWidth, retHeight);
	}

	PixelFormat pixFormat = PixelFormat32bppARGB;
	INT stride = retWidth * 4;
	m_gdiplusBitmapPtr = new Gdiplus::Bitmap(retWidth, retHeight, stride, pixFormat, (BYTE*)rowPointers[0]);
	Gdiplus::Status status = m_gdiplusBitmapPtr->GetLastStatus();
	if (status != Ok) {
		ERR("GetGDIBitmap error: create Gdiplus::Bitmap failed!!!!");
		SafeDelete(&m_gdiplusBitmapPtr);
	}
	
	return m_gdiplusBitmapPtr;
}
HRESULT	ResTexture::OnDrawImplEx(Graphics& graphics, const UIPos& rcWndPos, UIObject* obj /*= nullptr*/)
{
	return S_OK;
}
#endif