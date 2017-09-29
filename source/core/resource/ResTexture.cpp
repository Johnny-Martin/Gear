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
	, m_purpleLineColor(RGB(127, 0, 127))
	, m_lastQueryHeight(0)
{
	InitAttrMap();
	InitAttrValuePatternMap();
	InitAttrValueParserMap();
}

ResTexture::ResTexture(const string& strImageDesc)
	: m_lastQueryWidth(0)
	, m_purpleLineColor(RGB(127, 0, 127))
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
	, m_purpleLineColor(RGB(127, 0, 127))
{
}
ResTexture::ResTexture(png_bytep* rowPointers, png_uint_32 width, png_uint_32 height, png_byte colorType, png_byte colorChannels, png_byte bitDepth)
	: ResPicture(rowPointers, width, height, colorType, colorChannels, bitDepth)
	, m_lastQueryWidth(0)
	, m_lastQueryHeight(0)
	, m_purpleLineColor(RGB(127, 0, 127))
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
///////////////////////////////////////Direct2D��Ⱦģʽ��ش���///////////////////////////////////
#ifdef USE_D2D_RENDER_MODE
ID2D1Bitmap* ResTexture::GetD2D1Bitmap(ID2D1RenderTarget* pRenderTarget, unsigned int width, unsigned int height, unsigned int& retWidth, unsigned int& retHeight)
{
	auto CreateNineInOne = [&]()->ID2D1Bitmap* {
		
		return nullptr;
	};
	auto CreateThreeH = [&]()->ID2D1Bitmap* {

		return nullptr;
	};
	auto CreateThreeV = [&]()->ID2D1Bitmap* {

		return nullptr;
	};

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

	if (m_arrVerticalLinePos.size() == 2 && m_arrHorizontalLinePos.size() == 2){
		//NineInOne���͵�texture
		m_d2d1BitmapPtr = CreateNineInOne();
	} else if (m_arrHorizontalLinePos.size() == 2) {
		//ThreeV���͵�texture��ֱ�ӷ��ص���ͼƬ
		m_d2d1BitmapPtr = CreateThreeV();
	}else if (m_arrVerticalLinePos.size() == 2){
		//ThreeH���͵�texture��ֱ�ӷ��ص���ͼƬ
		m_d2d1BitmapPtr = CreateThreeH();
	}

	if (m_d2d1BitmapPtr){
		retWidth  = width;
		retHeight = height;
		return m_d2d1BitmapPtr;
	}
	ERR("GetD2D1Bitmap error: get illegal divied line count! hline count:{}, vline count:{}", m_arrHorizontalLinePos.size(),  m_arrVerticalLinePos.size());
	return nullptr;
}

//��GetD2D1Bitmap�������ص���ͼƬ��ͬ��OnDrawImplEx��ʹ��9��(����3��)������bitmap����һ��bitmap�����졢ƴ�ӣ�
HRESULT ResTexture::OnDrawImplEx(ID2D1RenderTarget* pRenderTarget, const D2D1_RECT_F& rcWndPos, UIObject* obj /*= nullptr*/)
{
	auto DrawNineInOne = [&]()->HRESULT {

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
	
	HRESULT hr = S_OK;
	if (m_arrVerticalLinePos.size() == 2 && m_arrHorizontalLinePos.size() == 2) {
		//NineInOne���͵�texture
		hr = DrawNineInOne();
	} else if (m_arrHorizontalLinePos.size() == 2) {
		//ThreeV���͵�texture
		hr = DrawThreeV();
	} else if (m_arrVerticalLinePos.size() == 2) {
		//ThreeH���͵�texture
		hr = DrawThreeH();
	}

	return hr;
}
/////////////////////////////////////////GDI+��Ⱦģʽ��ش���/////////////////////////////////////
#else
Gdiplus::Bitmap* ResTexture::GetGDIBitmap(unsigned int width, unsigned int height)
{

	return nullptr;
}
#endif