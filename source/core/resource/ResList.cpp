#include "stdafx.h"
#include "ResList.h"
#include "ResManager.h"

using namespace Gear::Res;

ResList::ResList()
	:m_hCount(1)
	,m_vCount(1)
	,m_subPicWidth(0)
	,m_subPicHeight(0)
	,m_subPicType(RES_IMAGE)
{
	InitAttrMap();
	InitAttrValuePatternMap();
	InitAttrValueParserMap();
}
ResList::~ResList()
{

}
ResList::ResList(const string& strListDesc, const wstring& wstrPath)
	:m_hCount(1)
	, m_vCount(1)
	, m_subPicWidth(0)
	, m_subPicHeight(0)
	, m_subPicType(RES_IMAGE)
{
	//从strListDesc（也就是ID）中解析出list的一些必要属性(ResList对象需要知道子图的类型、数量)
	//"#imagelist.1x4.button.bkg.2
	vector<string> vec = Split(strListDesc, ".");
	if (vec.size() < 2) {
		ERR("ResList constructor error: illegal resource name:{}", strListDesc);
		return;
	}
	string size = vec[1];
	vector<string> sizeVec = Split(vec[1], "x");
	if (sizeVec.size() != 1 && sizeVec.size() != 2) {
		ERR("ResList constructor error: illegal resource size description:{}", strListDesc);
		return;
	}
	if (sizeVec.size() == 1){
		m_hCount = atoi(sizeVec[0].c_str());
	}else {
		m_vCount = atoi(sizeVec[0].c_str());
		m_hCount = atoi(sizeVec[1].c_str());
	}

	if (strListDesc.find("imagelist.") == 0 || strListDesc.find("#imagelist.") == 0){
		m_subPicType = RES_IMAGE;
	} else if (strListDesc.find("texturelist.") == 0 || strListDesc.find("#texturelist.") == 0) {
		m_subPicType = RES_TEXTURE;
	} else {
		ERR("ResList error: can not parse resource type from strListDesc:{}", strListDesc);
		return;
	}

	m_wstrFilePath = wstrPath;// ResManager::GetInstance().GetResFilePathByName(strListDesc);
	if (m_wstrFilePath.empty()) {
		ERR("Create ResImage failed! cannot find image file");
		return;
	}
}
ResList::ResList(const wstring& wstrPath)
	:m_hCount(1)
	, m_vCount(1)
	, m_subPicWidth(0)
	, m_subPicHeight(0)
	, m_subPicType(RES_IMAGE)
{
	m_wstrFilePath = wstrPath;
}
png_uint_32 ResList::LoadAllSubPictures()
{
	if (m_pngWidth == 0) {
		auto ret = ReadPngFile(m_wstrFilePath);
		if (!ret) { return 0; }
	}

	ATLASSERT(m_hCount >= 0 && m_vCount > 0);
	m_subPicWidth  = m_pngWidth / m_hCount;
	m_subPicHeight = m_pngHeight / m_vCount;

	//使用内存块创建ResImage 和 ResTexture
	if (m_subPicType == RES_IMAGE) {
		for (int i=0; i<m_hCount*m_vCount; ++i){
			//分配空间

			//拷贝数据

			//使用内存块创建Image对象
			//ResPicture* picObjPtr = new ResImage();
		}

		//销毁本体内存空间??
	}
	
	return 0;
}
ResPicture*	ResList::GetSubPicObjByIndex(unsigned int posIndex)
{
	unsigned int count = m_hCount*m_vCount;
	if (posIndex >= count){
		ERR("GetSubPicObjByIndex error: illegal posIndex:{}, m_hCount*m_vCount: {}", posIndex, m_hCount*m_vCount);
		return nullptr;
	}
	//第一次使用的时候再加载
	if (m_subPicVec.size() == 0){
		
	}

	return nullptr;
}
void ResList::InitAttrMap()
{
	ADD_ATTR("file", "")
	ADD_ATTR("type", "image")
	ADD_ATTR("hcount", "1")
	ADD_ATTR("vcount", "1")
	//ADD_ATTR("split", "0")//List中的各个图片单元之间的分割用的是127，0，127
}
void ResList::InitAttrValuePatternMap()
{
	ADD_ATTR_PATTERN("hcount", R_CHECK_INT)
	ADD_ATTR_PATTERN("vcount", R_CHECK_INT)
	//ADD_ATTR_PATTERN("split", R_CHECK_BOOL)
}
void ResList::InitAttrValueParserMap()
{

}
///////////////////////////////////////Direct2D渲染模式相关代码///////////////////////////////////
#ifdef USE_D2D_RENDER_MODE
ID2D1Bitmap* ResList::GetD2D1Bitmap(ID2D1RenderTarget* pRenderTarget, unsigned int width, unsigned int height, unsigned int& retWidth, unsigned int& retHeight)
{
	return nullptr;
}
/////////////////////////////////////////GDI+渲染模式相关代码/////////////////////////////////////
#else
Gdiplus::Bitmap* ResList::GetGDIBitmap(unsigned int width, unsigned int height, unsigned int& retWidth, unsigned int& retHeight)
{
	return nullptr;
}
#endif