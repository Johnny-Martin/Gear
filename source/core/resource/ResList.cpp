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
ResList::~ResList()
{
	//ResList����ʱ������������m_subPicVec����Ķ�����ResManager������
}
//��Ĭ�Ϲ��캯����û��Ҫ����InitAttrMap()?
ResList::ResList(const string& strListDesc, const wstring& wstrPath)
	:m_hCount(1)
	, m_vCount(1)
	, m_subPicWidth(0)
	, m_subPicHeight(0)
	, m_subPicType(RES_IMAGE)
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

	//ʹ���ڴ�鴴��ResImage �� ResTexture
	for (int i=0; i<m_hCount*m_vCount; ++i){
		//����ռ�
		png_bytep*  rowPointers = AllocPngDataMem(m_subPicWidth, m_subPicHeight, m_colorChannels);
		//��������
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
		
		//ʹ���ڴ�鴴������
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

	//ResList����ʱ��~ResPicture������m_rowPointers[0]�Լ�m_rowPointers
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
//ResList�����ȡ����ͼ�񣬹�GetD2D1Bitmap��GetGDIBitmap����ʵ��
///////////////////////////////////////Direct2D��Ⱦģʽ��ش���///////////////////////////////////
#ifdef USE_D2D_RENDER_MODE
ID2D1Bitmap* ResList::GetD2D1Bitmap(ID2D1RenderTarget* pRenderTarget, unsigned int width, unsigned int height, unsigned int& retWidth, unsigned int& retHeight)
{
	ATLASSERT(FALSE);
	return nullptr;
}
/////////////////////////////////////////GDI+��Ⱦģʽ��ش���/////////////////////////////////////
#else
Gdiplus::Bitmap* ResList::GetGDIBitmap(unsigned int width, unsigned int height, unsigned int& retWidth, unsigned int& retHeight)
{
	ATLASSERT(FALSE);
	return nullptr;
}
#endif