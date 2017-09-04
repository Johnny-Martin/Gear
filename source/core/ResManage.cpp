#include "stdafx.h"
#include "ResManager.h"
#include "Util.h"

using namespace Gear::Res;

ResPicture::ResPicture() :
	m_purpleLineColor(RGB(127, 0, 127)),
	m_bPngFileLoaded(false),
	m_wstrFilePath(L""),
	m_pngWidth(0),
	m_pngHeight(0),
	m_colorType(0),
	m_bitDepth(0),
	m_colorChannels(0),
	m_rowPointers(nullptr),
	m_pngStructPtr(nullptr),
	m_pngInfoPtr(nullptr)
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
ResPicture::ResPicture(const wstring& wstrFilePath) :
	m_purpleLineColor(RGB(127, 0, 127)),
	m_bPngFileLoaded(false),
	m_wstrFilePath(L""),
	m_pngWidth(0),
	m_pngHeight(0),
	m_colorType(0),
	m_bitDepth(0),
	m_rowPointers(nullptr),
	m_pngStructPtr(nullptr),
	m_pngInfoPtr(nullptr)
{
	m_wstrFilePath = wstrFilePath;
	ReadPngFile(wstrFilePath);
}
bool ResPicture::ReadPngFile(const wstring& wstrFilePath)
{
	string strFilePath = WStringToString(wstrFilePath);
	if(!strFilePath.empty()){
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

bool ResPicture::IsVerticalLine(unsigned int horizontalPos, const COLORREF lineColor)
{
	for (unsigned int rowIndex = 0; rowIndex<m_pngHeight; ++rowIndex)
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
	for (unsigned int columnIndex = 0; columnIndex<m_pngWidth; ++columnIndex)
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

	for (unsigned int columnIndex = 0; columnIndex<m_pngWidth; ++columnIndex)
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

UIBitmap::UIBitmap():m_picObject(nullptr)
{
	InitAttrMap();
	InitEventMap();
	InitAttrValuePatternMap();
	InitAttrValueParserMap();
}
ID2D1Bitmap* UIBitmap::GetD2D1Bitmap(unsigned int width, unsigned int height)
{
	return nullptr;
}
Gdiplus::Bitmap* UIBitmap::GetGDIBitmap(unsigned int width, unsigned int height)
{
	return nullptr;
}
void UIBitmap::InitAttrMap()
{
	ADD_ATTR("fill", "1")
	ADD_ATTR("res",  "")
}

void UIBitmap::InitEventMap()
{
	//ADD_EVENT("OnCreate", nullptr)
}

bool UIBitmap::Init(const XMLElement* pElement)
{
	auto ret = UIObject::Init(pElement);
	if (ret && !m_attrMap["res"].empty()) {
		//加载res属性对应的png
		auto resID = m_attrMap["res"];
		m_picObject = ResManager::GetInstance().GetResObject(resID);
	}
	return true;
}

/*************************************************************************
*检查属性值是否合法
*	pos="leftexp, topexp, widthexp, heightexp"
*	leftexp、topexp    :支持0-9、#mid、#width、#height、()、+、-、*、/
*	widthexp、heightexp:支持0-9、#width、#height、()、+、-、*、/
**************************************************************************/
void UIBitmap::InitAttrValuePatternMap()
{
	ADD_ATTR_PATTERN("fill", R_CHECK_BOOL);
}
void UIBitmap::InitAttrValueParserMap()
{

}


ResManager::ResManager(LPWSTR szResPath)
{
	AddResPath(szResPath);
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

//设置资源文件夹，可以是相对路径(相对于exe),也可以是绝对路径.
//以后考虑支持zip
RESERROR ResManager::AddResPath(const wstring& cwstrPath)
{
	wstring wstrPath = cwstrPath;
	if (std::find(m_resPathVec.begin(), m_resPathVec.end(), wstrPath) != m_resPathVec.end()) {
		WARN("AddResPath warning: resource path already exists");
		return RES_SUCCESS;
	}

	//传的是相对路径，则要将相对路径做处理，以exe目录为基础，转成绝对路径
	if (wstrPath.find(L":") == wstring::npos) {
		TCHAR szFilePath[MAX_PATH + 1] = { 0 };
		GetModuleFileName(NULL, szFilePath, MAX_PATH);
		(_tcsrchr(szFilePath, _T('\\')))[1] = 0; //从文件名处截断，只获得路径字串
		
		::PathAppend(szFilePath, wstrPath.c_str());
		wstrPath = wstring(szFilePath);
	}
	//if (!::PathFileExists(wstrPath.c_str())) {
	//	ERR("AddResPath error: path file not exisit, wstrPath: {}", string(wstrPath.begin(), wstrPath.end()));
	//	return RES_ERROR_FILE_NOT_FOUND;
	//}
	
	/*文件夹路径的末尾一定要有\,方便以后拼路径使用 */
	if (wstrPath.find_last_of(L"\\") != wstrPath.length() - 1) {
		wstrPath += L"\\";
	}
	m_resPathVec.push_back(wstrPath);
	return RES_SUCCESS;
}

ResManager& ResManager::GetInstance()
{
	static ResManager resMgr{};
	return resMgr;
}

ResPicture*	ResManager::GetResObject(const string& strResID)
{
	auto pRes = m_resMap[strResID];
	if (pRes) { return pRes; }
	//map里不存在，就尝试从m_resPathVec里的目录里加载、解析
	
	if (LoadResource(strResID) && m_resMap[strResID] != nullptr) {
		return m_resMap[strResID];
	}

	ERR("GetResObject error: can not find ResObject, strResID: {}", strResID);
	return nullptr;
}

bool ResManager::LoadResource(const string& strResID)
{
	auto GetResType = [](const string& resID)->ResType {
		if (resID.find("image.") == 0) {
			return RES_IMAGE;
		} else if (resID.find("texture.") == 0) {
			return RES_TEXTURE;
		} else if (resID.find("imagelist.") == 0) {
			return RES_IMAGELIST;
		} else if (resID.find("texturelist.") == 0) {
			return RES_TEXTURELIST;
		}
		return RES_INVALIDE_TYPE;
	};

	auto GetResFileName = [](const string& resID)->string {
		string fileName;
		if (resID.find("image.") == 0 || resID.find("texture.") == 0) {
			fileName = resID + ".png";
		}
		else if (resID.find("imagelist.") == 0 || resID.find("texturelist.") == 0) {
			auto pos = resID.find_last_of(".");
			if (pos != string::npos) {
				fileName = resID.substr(0, pos) + ".png";
			}
		}
		return fileName;
	};

	auto fileName = GetResFileName(strResID);
	if (fileName.empty()) { 
		ERR("LoadResource error: Get resource file name failed, resID: {}", strResID);
		return false; 
	}

	///////////2017.8.26下午，敲到这里偶然转了一下头，一切都变了。

	wstring wstrFileName = StringToWString(fileName);
	for (auto it = m_resPathVec.begin(); it != m_resPathVec.end(); ++it) {
		wstring wstrCurPath = *it + wstrFileName;
		if (::PathFileExists(wstrCurPath.c_str())){
			return LoadResFromFile(wstrCurPath, strResID, GetResType(fileName));
		}
	}
	ERR("LoadResource error: Resource file not found in all resource folder");
	return false;
}

bool ResManager::LoadResFromFile(const wstring& wstrFilePath, const string& strResID, ResType resType)
{
	if (resType == RES_INVALIDE_TYPE) {
		ERR("LoadResFromFile error: RES_INVALIDE_TYPE");
		return false;
	}

	if (resType == RES_IMAGE) {
		ResPicture* pResPic = new ResImage(wstrFilePath);
		m_resMap.insert(pair<string, ResPicture*>(strResID, pResPic));
	} else if (resType == RES_TEXTURE) {
		ResPicture* pResPic = new ResTexture(wstrFilePath);
		m_resMap.insert(pair<string, ResPicture*>(strResID, pResPic));
	} else if (resType == RES_IMAGELIST) {
		PicListDivider divider(wstrFilePath);
		//vector<ResPicture*> vec = divider.DividePic();
		//for (auto it = vec.begin(); it != vec.end(); ++it) {
			//初始化资源

			//将资源存入m_resMap
		//}
	} else if (resType == RES_TEXTURELIST) {
		PicListDivider divider(wstrFilePath);
		//vector<ResPicture*> vec = divider.DividePic();
		///for (auto it = vec.begin(); it != vec.end(); ++it) {
			//初始化资源

			//将资源存入m_resMap
		//}
	}
	return true;
}

wstring ResManager::GetResFilePath(LPCSTR szResID)
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
ResImage::ResImage(const wstring& wstrFilePath)
{
	m_wstrFilePath = wstrFilePath;
	ReadPngFile(wstrFilePath);
}
ID2D1Bitmap* ResImage::GetD2D1Bitmap(unsigned int width, unsigned int height)
{

	return nullptr;
}
Gdiplus::Bitmap* ResImage::GetGDIBitmap(unsigned int width, unsigned int height)
{

	return nullptr;
}

ResTexture::ResTexture(const wstring& wstrFilePath)
{
	m_wstrFilePath = wstrFilePath;
	if (ReadPngFile(wstrFilePath)) {
		DetectHorizontalLine();
		DetectVerticalLine();
	}
}
ID2D1Bitmap* ResTexture::GetD2D1Bitmap(unsigned int width, unsigned int height)
{

	return nullptr;
}
Gdiplus::Bitmap* ResTexture::GetGDIBitmap(unsigned int width, unsigned int height)
{

	return nullptr;
}

unsigned int PicListDivider::GetPicCount()
{
	return m_arrVerticalLinePos.empty() ? 0 : m_arrVerticalLinePos.size() + 1;
}

//imagelist、texturelist都是水平排布的——用竖线分割
PicListDivider::PicListDivider(const wstring& wstrFilePath)
{
	m_wstrFilePath = wstrFilePath;
	if (ReadPngFile(wstrFilePath)) {
		DetectVerticalLine();
	}
}
ResPicture*	PicListDivider::GetPicByIndex()
{
	if(GetPicCount() == 0){ return nullptr; }

	//根据index创建新的image、texture

	return nullptr;
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
