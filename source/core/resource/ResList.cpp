#include "stdafx.h"
#include "ResList.h"
#include "ResManager.h"

using namespace Gear::Res;

ResList::ResList()
	:m_hCount(1)
	,m_vCount(1)
	,m_subPicWidth(0)
	,m_subPicHeight(0)
{
	InitAttrMap();
	InitAttrValuePatternMap();
	InitAttrValueParserMap();
}
ResList::~ResList()
{

}
ResList::ResList(const string& strListDesc, const wstring& wstrPath)
{
	//��strListDesc��Ҳ����ID���н�����list��һЩ��Ҫ����(ResList������Ҫ֪����ͼ�����͡�����)
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

	m_wstrFilePath = wstrPath;// ResManager::GetInstance().GetResFilePathByName(strListDesc);
	if (m_wstrFilePath.empty()) {
		ERR("Create ResImage failed! cannot find image file");
		return;
	}
}
ResList::ResList(const wstring& wstrPath)
{
	m_wstrFilePath = wstrPath;
}
png_uint_32 ResList::LoadSubPictures()
{
	if (m_pngWidth == 0) {
		auto ret = ReadPngFile(m_wstrFilePath);
		if (!ret) { return 0; }
	}

	ATLASSERT(m_hCount >= 0 && m_vCount > 0);
	m_subPicWidth  = m_pngWidth / m_hCount;
	m_subPicHeight = m_pngHeight / m_vCount;

	//ʹ���ڴ�鴴��ResImage �� ResTexture
	//--//ResImage(BYTE* memChunk, width, height, colorType, m_bitDepth, colorChannels)
	return 0;
}
ResPicture*	ResList::GetSubPicObjByIndex(unsigned int posIndex)
{
	unsigned int count = m_hCount*m_vCount;
	if (posIndex >= count){
		ERR("GetSubPicObjByIndex error: illegal posIndex:{}, m_hCount*m_vCount: {}", posIndex, m_hCount*m_vCount);
		return nullptr;
	}
	//��һ��ʹ�õ�ʱ���ټ���
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
	//ADD_ATTR("split", "0")//List�еĸ���ͼƬ��Ԫ֮��ķָ��õ���127��0��127
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
///////////////////////////////////////Direct2D��Ⱦģʽ��ش���///////////////////////////////////
#ifdef USE_D2D_RENDER_MODE
ID2D1Bitmap* ResList::GetD2D1Bitmap(ID2D1RenderTarget* pRenderTarget, unsigned int width, unsigned int height, unsigned int& retWidth, unsigned int& retHeight)
{
	return nullptr;
}
/////////////////////////////////////////GDI+��Ⱦģʽ��ش���/////////////////////////////////////
#else
Gdiplus::Bitmap* ResList::GetGDIBitmap(unsigned int width, unsigned int height, unsigned int& retWidth, unsigned int& retHeight)
{
	return nullptr;
}
#endif