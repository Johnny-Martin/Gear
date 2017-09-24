#include "stdafx.h"
#include "ResManager.h"

using namespace Gear::Res;

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
	for (auto it = m_colorMap.begin(); it != m_colorMap.end(); ++it) {
		delete it->second;
	}
	for (auto it = m_colorMap2.begin(); it != m_colorMap2.end(); ++it) {
		delete it->second;
	}
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
		TCHAR wszFilePath[MAX_PATH + 1] = { 0 };
		GetModuleFileName(NULL, wszFilePath, MAX_PATH);
		(_tcsrchr(wszFilePath, _T('\\')))[1] = 0; //从文件名处截断，只获得路径字串
		
		::PathAppend(wszFilePath, wstrPath.c_str());
		wstrPath = wstring(wszFilePath);
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
wstring ResManager::GetResFilePathByName(const wstring& cwstrName)
{
	for (auto it = m_resPathVec.begin(); it != m_resPathVec.end(); ++it) {
		TCHAR wszFilePath[MAX_PATH + 1] = { 0 };
		::PathAppend(wszFilePath, cwstrName.c_str());
		if (::PathFileExists(wszFilePath)) {
			return wstring(wszFilePath);
		}
	}
	return L"";
}
wstring ResManager::GetResFilePathByName(const string& cstrName)
{
	wstring wstrName = StringToWString(cstrName);;
	return GetResFilePathByName(wstrName);
}
ResManager& ResManager::GetInstance()
{
	static ResManager resMgr{};
	return resMgr;
}

ResPicture*	ResManager::GetPicObject(const string& strResID)
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
ResColor* ResManager::GetColorObject(const string& strColorValueOrID)
{
	if (strColorValueOrID.find("#") == 0){
		auto it = m_colorMap.find(strColorValueOrID);
		return it == m_colorMap.end() ? nullptr : it->second;
	} 
	
	auto it = m_colorMap2.find(strColorValueOrID);
	if (it != m_colorMap2.end()){
		return it->second;
	}
	//用给定的色值创建一个颜色对象
	ResColor* pColorObj = new ResColor(strColorValueOrID);
	m_colorMap2[strColorValueOrID] = pColorObj;
	return pColorObj;
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
	ERR("LoadResource error: Resource file not found in all resource folder, strResID: {}, fileName: {}", strResID, fileName);
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
		//ResPicture* pResPic = new ResTexture(wstrFilePath);
		//m_resMap.insert(pair<string, ResPicture*>(strResID, pResPic));
	} else if (resType == RES_IMAGELIST) {
		//PicListDivider divider(wstrFilePath);
		//vector<ResPicture*> vec = divider.DividePic();
		//for (auto it = vec.begin(); it != vec.end(); ++it) {
			//初始化资源

			//将资源存入m_resMap
		//}
	} else if (resType == RES_TEXTURELIST) {
		//PicListDivider divider(wstrFilePath);
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


