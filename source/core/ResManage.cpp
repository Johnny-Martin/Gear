#include "stdafx.h"
#include "ResManager.h"

using namespace Gear::Res;

UIPicture::UIPicture() :m_purpleLineColor(RGB(127, 0, 127))
{

}
UIPicture::UIPicture(const string& strFilePath) : m_purpleLineColor(RGB(127, 0, 127))
{
	m_strFilePath = strFilePath;
	ReadPngFile(strFilePath);
}

RESERROR UIPicture::ReadPngFile(const string& strFilePath)
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
	m_pixelDepth = m_pngInfoPtr->pixel_depth;

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
	for (unsigned int rowIndex = 0; rowIndex<m_pngHeight; ++rowIndex)
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

bool UIPicture::IsVerticalLine(unsigned int horizontalPos, const COLORREF lineColor)
{
	unsigned int bytesPerPixel = m_pixelDepth / 8;
	for (unsigned int rowIndex = 0; rowIndex<m_pngHeight; ++rowIndex)
	{
		png_byte* row = m_rowPointers[rowIndex];
		png_byte* ptr = &(row[horizontalPos*bytesPerPixel]);

		COLORREF pixelColor = RGB(ptr[0], ptr[1], ptr[2]);
		if (lineColor != pixelColor)
			return false;
	}
	return true;
}

bool UIPicture::IsHorizontalLine(unsigned int horizontalPos, const COLORREF lineColor)
{
	png_byte* row = m_rowPointers[horizontalPos];
	unsigned int bytesPerPixel = m_pixelDepth / 8;
	for (unsigned int columnIndex = 0; columnIndex<m_pngWidth; ++columnIndex)
	{
		png_byte* ptr = &(row[columnIndex*bytesPerPixel]);

		COLORREF pixelColor = RGB(ptr[0], ptr[1], ptr[2]);
		if (lineColor != pixelColor)
			return false;
	}
	return true;
}

//detect the dividing line(RGB: 255,0,255)
RESERROR UIPicture::DetectVerticalLine()
{
	//before calling this function,make sure that
	//png file must has been loaded to memory successfully.
	png_byte* pixelDataPtr = NULL;
	unsigned int bytesPerPixel = m_pixelDepth / 8;

	for (unsigned int columnIndex = 0; columnIndex<m_pngWidth; ++columnIndex)
	{
		pixelDataPtr = &(m_rowPointers[0][columnIndex*bytesPerPixel]);
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
RESERROR UIPicture::DetectHorizontalLine()
{
	//before calling this function,make sure that
	//png file must has been loaded to memory successfully.
	png_byte* pixelDataPtr = NULL;
	unsigned int bytesPerPixel = m_pixelDepth / 8;
	for (unsigned int rowIndex = 0; rowIndex<m_pngHeight; ++rowIndex)
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

UIBitmap::UIBitmap()
{
	InitAttrMap();
	InitEventMap();
	InitAttrValuePatternMap();
	InitAttrValueParserMap();
}
void UIBitmap::InitAttrMap()
{
	ADD_ATTR("fit", "1")
	ADD_ATTR("file", "")
}
void UIBitmap::InitEventMap()
{
	//ADD_EVENT("OnCreate", nullptr)
}
bool UIBitmap::Init(const XMLElement* pElement)
{
	auto ret = UIObject::Init(pElement);
	if (ret) {
		//加载file属性中的路径对应的png

	}
	return true;
}
ID2D1Bitmap* UIBitmap::GetD2D1Bitmap(unsigned int width, unsigned int height)
{
	return nullptr;
}

Gdiplus::Bitmap* UIBitmap::GetGDIBitmap(unsigned int width, unsigned int height)
{
	return nullptr;
}
/*************************************************************************
*检查属性值是否合法
*	pos="leftexp, topexp, widthexp, heightexp"
*	leftexp、topexp    :支持0-9、#mid、#width、#height、()、+、-、*、/
*	widthexp、heightexp:支持0-9、#width、#height、()、+、-、*、/
**************************************************************************/
void UIBitmap::InitAttrValuePatternMap()
{
	ADD_ATTR_PATTERN("fit", R_CHECK_BOOL);
}
void UIBitmap::InitAttrValueParserMap()
{
	auto ParseFilePath = [&](const string& sAttrName = "file") {
		//还缺插件管理的设施：管理插件包、文件路径
		//暂时file属性使用全路径
		string strPath = m_attrMap[sAttrName];
		if (!::PathFileExistsA(strPath.c_str())) {
			ERR("ParseFilePath error: file not exisit, path: {}", strPath);
			return false;
		}
		return true;
	};

	ADD_ATTR_PARSER("file", ParseFilePath);
}


ResManager::ResManager(LPWSTR szResPath)
{
	SetResPath(szResPath);
}
ResManager::~ResManager()
{
	/*
	map<string, RPicture*>::iterator iter = m_resID2HandleMap.begin();
	for (; iter != m_resID2HandleMap.end(); ++iter)
	{
		delete iter->second;
	}
	//*/
}

RESERROR ResManager::SetResPath(LPWSTR wszResPath)
{
	m_wszResPath = wszResPath;
	if (::PathFileExists(wszResPath))
		return RES_SUCCESS;

	return RES_ERROR_FILE_NOT_FOUND;
}
wstring ResManager::GetPicPathByID(LPCSTR szResID)
{
	LPWSTR wszPath = new WCHAR[MAX_PATH];

	unsigned int i;
	for (i = 0; i<m_wszResPath.length(); ++i)
		wszPath[i] = m_wszResPath[i];
	wszPath[i] = '\0';

	int unicodeLen = MultiByteToWideChar(CP_ACP, 0, szResID, -1, NULL, 0);
	LPWSTR wszMore = new WCHAR[unicodeLen + 1];
	MultiByteToWideChar(CP_ACP, 0, szResID, -1, wszMore, unicodeLen);

	PathAppend(wszPath, wszMore);

	wstring wstrRet = wszPath;
	delete[] wszPath;
	delete[] wszMore;

	wstrRet += L".png";
	return wstrRet;
}

unsigned int ResManager::GetIndexFromPicListId(LPCSTR szPicListID)
{
	unsigned int picIndex = 0;
	//get the index section
	std::size_t iPicListIDLen = strlen(szPicListID);

	unsigned int iLastDotPos = iPicListIDLen;
	for (; iLastDotPos>0; --iLastDotPos)
	{
		if ('.' == szPicListID[iLastDotPos])
			break;
	}

	for (unsigned int i = iLastDotPos + 1; i<iPicListIDLen; ++i)
	{
		if (szPicListID[i] > '9' || szPicListID[i] < '0')
		{
			return 0;
		}
		else
		{
			picIndex = picIndex * 10 + szPicListID[i] - '0';
		}
	}

	return picIndex;
}
string ResManager::GetRealIdFromPicListId(LPCSTR szPicListID)
{
	//get the index section
	unsigned int iLastDotPos = strlen(szPicListID);
	for (; iLastDotPos>0; --iLastDotPos)
	{
		if ('.' == szPicListID[iLastDotPos])
			break;
	}

	string strRealResId;
	for (unsigned int i = 0; i<iLastDotPos; ++i)
		strRealResId.append(sizeof(char), szPicListID[i]);

	return strRealResId;
}
/*RESERROR ResManager::GetResPicHandle(LPCSTR szResID, RPicture** hRes)
{
	
	map<string, RPicture*>::iterator iter = m_resID2HandleMap.find(szResID);
	if (m_resID2HandleMap.end() != iter)
	{
		*hRes = iter->second;
		return RES_SUCCESS;
	}

	wstring resFilePath;
	string strResID = szResID;
	//list's strResID must has .index at the end
	std::size_t iBeginPos = strResID.find("texturelist");
	std::size_t iBeginPosEx = strResID.find("imagelist");
	if (0 == iBeginPos || 0 == iBeginPosEx)
	{
		unsigned int resIndex = GetIndexFromPicListId(szResID);
		if (resIndex <= 0)
			return RES_ERROR_ILLEGAL_ID;

		string strRealResId = GetRealIdFromPicListId(szResID);
		iter = m_resID2HandleMap.find(strRealResId);
		//piclist has been created
		if (m_resID2HandleMap.end() != iter)
		{
			RPicList* picListObjPointer = dynamic_cast<RPicList*>(iter->second);
			*hRes = picListObjPointer->GetPicByIndex(resIndex - 1);
			return (NULL == (*hRes)) ? RES_ERROR_ILLEGAL_ID : RES_SUCCESS;
		}

		resFilePath = GetPicPathByID(strRealResId.c_str());
		if (!::PathFileExists(resFilePath.c_str()))
			return RES_ERROR_FILE_NOT_FOUND;

		RPicList* picListObj = new RPicList(resFilePath.c_str(), strRealResId.c_str());

		//insert each of texture into map
		//++++++++++++++++++++++++++++++++
		m_resID2HandleMap.insert(pair<string, RPicList*>(szResID, picListObj));

		*hRes = picListObj->GetPicByIndex(resIndex - 1);
		RPicture* pObj = *hRes;

		return (NULL == (*hRes)) ? RES_ERROR_ILLEGAL_ID : RES_SUCCESS;
	}

	iBeginPos = strResID.find("texture");
	iBeginPosEx = strResID.find("image");
	if (0 == iBeginPos || 0 == iBeginPosEx)
	{
		resFilePath = GetPicPathByID(szResID);
		if (!::PathFileExists(resFilePath.c_str()))
			return RES_ERROR_FILE_NOT_FOUND;

		RPicture* picObj = NULL;
		if (0 == iBeginPos)
		{
			picObj = new RTexture(resFilePath.c_str(), szResID);
		}
		else
		{
			picObj = new RImage(resFilePath.c_str(), szResID);
		}
		m_resID2HandleMap.insert(pair<string, RPicture*>(szResID, picObj));

		*hRes = picObj;
		return RES_SUCCESS;
	}
	
	return RES_ERROR_UNKNOWN;
}//*/
