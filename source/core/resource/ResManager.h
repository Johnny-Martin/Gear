/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.8.25
Description:		����ͼƬ��Դ������
*****************************************************/
#pragma once
#include "../stdafx.h"

#include "png.h"
#include "../utils/UIError.h"
#include "../base/UIObject.h"
#include "../entry/RenderManager.h"
#include "ResPicture.h"
#include "ResColor.h"
#include "ResTexture.h"
#include "ResImage.h"

using namespace std;

namespace Gear {
	namespace Res{

class ResPicture;
enum ResType
{
	RES_INVALIDE_TYPE = 0,
	RES_IMAGE = 1,
	RES_TEXTURE = 2,
	RES_IMAGELIST = 3,
	RES_TEXTURELIST = 4,
};

class ResManager
{
public:
									ResManager() {};
									ResManager(LPWSTR szResPath);
									~ResManager();
	RESERROR						AddResPath(const wstring& cwstrPath);
	wstring							GetResFilePathByName(const wstring& cwstrName);
	wstring							GetResFilePathByName(const string& cstrName);
	static ResManager&				GetInstance();
	//��Ҫ�ӿ�
	ResPicture*						GetPicObject(const string& strResID);
	ResColor*						GetColorObject(const string& strColorValueOrID);
	bool							LoadResource(const string& strResID);
	bool							LoadResFromFile(const wstring& wstrFilePath, const string& strResID, ResType resType);
private:
	
	unsigned int					GetIndexFromPicListId(LPCSTR szPicListID);
	string							GetRealIdFromPicListId(LPCSTR szPicListID);
	wstring							GetResFilePath(LPCSTR szResID);
	wstring							m_wszResPath;
	//map<string, RPicture*>		m_resID2HandleMap;
	vector<wstring>					m_resPathVec;
	map<const string, ResPicture*>  m_picMap;
	map<const string, ResColor*>	m_colorMap;//������ID��������ɫ����XML����ģ�<color id="mainwnd.bak.color" value=...��
	map<const string, ResColor*>	m_colorMap2;//������ɫֵ��������ɫ����
};


class PicListDivider :public ResPicture
{
public:
									PicListDivider(const wstring& wstrFilePath);
#ifdef USE_D2D_RENDER_MODE
	virtual ID2D1Bitmap*			GetD2D1Bitmap(unsigned int width, unsigned int height) { return nullptr; }
#else
	virtual Gdiplus::Bitmap*		GetGDIBitmap(unsigned int width, unsigned int height)  { return nullptr; }
#endif
	unsigned int					GetPicCount();
	ResPicture*						GetPicByIndex();
};

//class UIImage

	}//end of namespace Res
}//end of namespace Gear