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
ResList::~ResList()
{
	//ResList析构时，无需析构，m_subPicVec里面的对象（由ResManager析构）
}
//非默认构造函数，没必要调用InitAttrMap()?
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
		m_vCount = 1;
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

	m_wstrFilePath = wstrPath;
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

	ATLASSERT(m_hCount > 0 && m_vCount > 0);
	m_subPicWidth  = m_pngWidth / m_hCount;
	m_subPicHeight = m_pngHeight / m_vCount;
	ATLASSERT(m_subPicWidth > 0 && m_subPicHeight > 0);

	//使用内存块创建ResImage 和 ResTexture
	for (int i=0; i<m_hCount*m_vCount; ++i){
		//分配空间
		png_bytep*  rowPointers = AllocPngDataMem(m_subPicWidth, m_subPicHeight, m_colorChannels);
		//拷贝数据
		int subPicRowPos = i / m_hCount;
		int subPicColPos = i % m_hCount;
		png_uint_32 fatherRowSize = png_get_rowbytes(m_pngStructPtr, m_pngInfoPtr);
		for (png_uint_32 row = 0; row < m_subPicHeight; ++row) {
			//int fatherRow  = subPicRowPos * m_subPicHeight + row;
			for (png_uint_32 col = 0; col < m_subPicWidth * m_colorChannels; ++col) {
				//int fatherCol = subPicColPos * m_subPicWidth * m_colorChannels + col;
				rowPointers[row][col] = m_rowPointers[subPicRowPos * m_subPicHeight + row][subPicColPos * m_subPicWidth * m_colorChannels + col];
			}
		}
		
		//使用内存块创建对象
		if (m_subPicType == RES_IMAGE) {
			ResPicture* picObjPtr = new ResImage(rowPointers, m_subPicWidth, m_subPicHeight, m_colorType, m_colorChannels, m_bitDepth);
			m_subPicVec.push_back(picObjPtr);
		} else if (m_subPicType == RES_TEXTURE){

		} else {
			ERR("ResList::LoadAllSubPictures error: illegal Resource type!");
			free(rowPointers[0]);
			free(rowPointers);
			rowPointers = nullptr;
			return 0;
		}
	}

	//ResList析构时，~ResPicture会销毁m_rowPointers[0]以及m_rowPointers
	return m_hCount*m_vCount;
}
ResPicture*	ResList::GetSubPicObjByIndex(unsigned int posIndex)
{
	unsigned int count = m_hCount*m_vCount;
	if (posIndex >= m_subPicVec.size()){
		ERR("GetSubPicObjByIndex error: illegal posIndex:{}, m_subPicVec.size: {}", posIndex, m_subPicVec.size());
		return nullptr;
	}
	return m_subPicVec[posIndex];
}
//ResList无需获取整个图像，故GetD2D1Bitmap与GetGDIBitmap无需实现
///////////////////////////////////////Direct2D渲染模式相关代码///////////////////////////////////
#ifdef USE_D2D_RENDER_MODE
ID2D1Bitmap* ResList::GetD2D1Bitmap(ID2D1RenderTarget* pRenderTarget, unsigned int width, unsigned int height, unsigned int& retWidth, unsigned int& retHeight)
{
	ATLASSERT(FALSE);
	return nullptr;
}
/////////////////////////////////////////GDI+渲染模式相关代码/////////////////////////////////////
#else
Gdiplus::Bitmap* ResList::GetGDIBitmap(unsigned int width, unsigned int height, unsigned int& retWidth, unsigned int& retHeight)
{
	ATLASSERT(FALSE);
	return nullptr;
}
#endif