#include "stdafx.h"
#include "ResManager.h"
#include "../entry/WndManager.h"

using namespace Gear::Res;

#ifdef DEBUG
vector<string>	ResManager::m_vecLanguages{ "zh_cn", "zh_hk", "zh_tw", "en_us" };
#endif // DEBUG

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
	for (auto it=m_picMap.begin(); it!=m_picMap.end(); ++it){
		delete it->second;
	}
}

//������Դ�ļ��У����������·��(�����exe),Ҳ�����Ǿ���·��.
//�Ժ���֧��zip
RESERROR ResManager::AddResPath(const wstring& cwstrPath)
{
	wstring wstrPath = cwstrPath;
	if (std::find(m_resPathVec.begin(), m_resPathVec.end(), wstrPath) != m_resPathVec.end()) {
		WARN("AddResPath warning: resource path already exists");
		return RES_SUCCESS;
	}

	//���������·������Ҫ�����·����������exeĿ¼Ϊ������ת�ɾ���·��
	if (wstrPath.find(L":") == wstring::npos) {
		TCHAR wszFilePath[MAX_PATH + 1] = { 0 };
		GetModuleFileName(NULL, wszFilePath, MAX_PATH);
		(_tcsrchr(wszFilePath, _T('\\')))[1] = 0; //���ļ������ضϣ�ֻ���·���ִ�
		
		::PathAppend(wszFilePath, wstrPath.c_str());
		wstrPath = wstring(wszFilePath);
	}
	//if (!::PathFileExists(wstrPath.c_str())) {
	//	ERR("AddResPath error: path file not exisit, wstrPath: {}", string(wstrPath.begin(), wstrPath.end()));
	//	return RES_ERROR_FILE_NOT_FOUND;
	//}
	
	/*�ļ���·����ĩβһ��Ҫ��\,�����Ժ�ƴ·��ʹ�� */
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
	//����#��id,Ҫ��XML��ã�ResManager���ڳ�ʼ��ʱ��������<resource>��ǩ
	auto it = m_picMap.find(strResID);
	if (it != m_picMap.end()) { return it->second; }

	//map�ﲻ���ڣ��ͳ��Դ�m_resPathVec���Ŀ¼����ء�������strResIDͷ����Ҫ��һ��#�š�
	if (strResID.find("#") == 0) {
		if (LoadResourceByID(strResID) && m_picMap[strResID] != nullptr) {
			return m_picMap[strResID];
		}
	}
	//
	ERR("GetResObject error: can not find ResObject, strResID: {}", strResID);
	return nullptr;
}
ResColor* ResManager::GetColorObject(const string& strColorValueOrID)
{
	if (strColorValueOrID.find("#") == string::npos){
		auto it = m_colorMap.find(strColorValueOrID);
		return it == m_colorMap.end() ? nullptr : it->second;
	} 
	//��#�ŵ�id,��һ��ɫֵ������������ID
	auto it = m_colorMap2.find(strColorValueOrID);
	if (it != m_colorMap2.end()){
		return it->second;
	}
	//�ø�����ɫֵ����һ����ɫ����
	string value = strColorValueOrID.substr(1, strColorValueOrID.length() - 1);
	ResColor* pColorObj = new ResColor(value);
	m_colorMap2[strColorValueOrID] = pColorObj;
	return pColorObj;
}
ResFont* ResManager::GetFontObject(const string& strFontIDOrDesc)
{
	if (strFontIDOrDesc.find("#") == string::npos){
		//xml�ж�������壬��IDΪ����
		auto it = m_fontMap.find(strFontIDOrDesc);
		return it == m_fontMap.end() ? nullptr : it->second;
	}
	//ʹ��������������һ�������������"#����.12.normal"��
	auto it = m_fontMap2.find(strFontIDOrDesc);
	if (it != m_fontMap2.end()){
		return it->second;
	}
	string realID = strFontIDOrDesc.substr(1, strFontIDOrDesc.size() - 1);
	auto pFont = new ResFont(realID);
	m_fontMap2[strFontIDOrDesc] = pFont;
	return pFont;
}
ResType Gear::Res::GetResType(const string& resID)
{
	if (resID.find("image.") == 0 || resID.find("#image.") == 0) {
		return RES_IMAGE;
	} else if (resID.find("texture.") == 0 || resID.find("#texture.") == 0) {
		return RES_TEXTURE;
	} else if (resID.find("imagelist.") == 0 || resID.find("#imagelist.") == 0) {
		return RES_IMAGELIST;
	} else if (resID.find("texturelist.") == 0 || resID.find("#texturelist.") == 0) {
		return RES_TEXTURELIST;
	}
	return RES_INVALIDE_TYPE;
};

bool ResManager::LoadResourceByID(const string& strResID)
{
	auto GetResFileName = [](const string& resID)->string {
		string fileName;
		string tmpResID = resID.substr(1, resID.size() - 1);
		
		if (tmpResID.find("image.") == 0 || tmpResID.find("texture.") == 0) {
			fileName = tmpResID + ".png";
		}
		else if (tmpResID.find("imagelist.") == 0 || tmpResID.find("texturelist.") == 0) {
			auto pos = tmpResID.find_last_of(".");
			if (pos != string::npos) {
				fileName = tmpResID.substr(0, pos) + ".png";
			}
		}
		return fileName;
	};

	auto fileName = GetResFileName(strResID);
	if (fileName.empty()) { 
		ERR("LoadResource error: Get resource file name failed, resID: {}", strResID);
		return false; 
	}

	///////////2017.8.26���磬�õ�����żȻת��һ��ͷ��һ�ж����ˡ�

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
		m_picMap.insert(pair<string, ResPicture*>(strResID, pResPic));
	} else if (resType == RES_TEXTURE) {
		ResPicture* pResPic = new ResTexture(wstrFilePath);
		m_picMap.insert(pair<string, ResPicture*>(strResID, pResPic));
	} else if (resType == RES_IMAGELIST || resType == RES_TEXTURELIST) {
		//�˴���strResID��ָimagelist�е�һ��subImage��ID,��Ҫ�����imagelist��ID
		auto pos = strResID.find_last_of(".");
		ATLASSERT(pos != string::npos);
		string listID = strResID.substr(0, pos);
		
		ResList picList(listID, wstrFilePath);
		png_uint_32 picCount = picList.LoadAllSubPictures();
		for (png_uint_32 i=0; i<picCount; ++i){
			ResPicture* picObjPtr = picList.GetSubPicObjByIndex(i);
			string subPicID = listID + "." + NumToString(i);
			m_picMap.insert(pair<string, ResPicture*>(subPicID, picObjPtr));
		}	
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
string ResManager::GetLanguageName()
{
	return m_languageName;
}
void ResManager::SetLanguageName(const string& name)
{
	if (m_languageName != name) {
#ifdef DEBUG
		auto it = std::find(m_vecLanguages.begin(), m_vecLanguages.end(), name);
		ATLASSERT(it != m_vecLanguages.end());
#endif
		m_languageName = name;
		//֪ͨ���д��ڣ����»���һ�顣
		WndManager::GetInstance().RePaintAllWnd();
	}
}


