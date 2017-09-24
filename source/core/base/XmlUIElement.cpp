#include "stdafx.h"
#include "XmlUIElement.h"

//����ʶ����ַ�ת�����У�������ʽ�е�\s��
#pragma warning(disable:4129)

XmlUIElement::XmlUIElement():m_bInit(false)
{
	InitAttrMap();
	InitAttrValuePatternMap();
	InitAttrValueParserMap();
}
bool XmlUIElement::Init(const XMLElement* pElement)
{
	m_bInit = InitImpl(pElement);
	return m_bInit;
}
bool XmlUIElement::InitImpl(const XMLElement* pElement)
{
	//����element������
	auto pAttr = pElement->FirstAttribute();
	while (pAttr) {
		auto attrName = pAttr->Name();
		SetAttrValue(attrName, pAttr->Value());
		pAttr = pAttr->Next();
	}

	return true;
}
void XmlUIElement::InitAttrMap()
{
	ADD_ATTR("id", "")
	ADD_ATTR("name", "")
}

/*************************************************************************
*�������ֵ�Ƿ�Ϸ�
*	pos="leftexp, topexp, widthexp, heightexp"
*	leftexp��topexp    :֧��0-9��#mid��#width��#height��()��+��-��*��/
*	widthexp��heightexp:֧��0-9��#width��#height��()��+��-��*��/
**************************************************************************/
void XmlUIElement::InitAttrValuePatternMap()
{
	ADD_ATTR_PATTERN("id", R_CHECK_ID);
	ADD_ATTR_PATTERN("name", R_CHECK_ID);
}
/*************************************************************************
*��������ֵ
**************************************************************************/
void XmlUIElement::InitAttrValueParserMap()
{
	auto OnSetID = [&](const string& sAttrName)->bool {
		//name �� id ͬ�ȶԴ�
		m_attrMap["id"]		= m_attrMap[sAttrName];
		m_attrMap["name"]	= m_attrMap[sAttrName];
		return true;
	};

	ADD_ATTR_PARSER("id", OnSetID)
	ADD_ATTR_PARSER("name", OnSetID)
}
shared_ptr<const string> XmlUIElement::GetObjectID()
{
	if (m_attrMap.empty()) {
		ERR("GetObjectID error: m_attrMap has not been initicalized");
		return nullptr;
	}
	//id �� name������key��һ�����ڵģ����뵣��map��[]�������������İ����key
	if (!m_attrMap["id"].empty())
		return make_shared<const string>(m_attrMap["id"]);

	if (!m_attrMap["name"].empty())
		return make_shared<const string>(m_attrMap["name"]);

	return nullptr;
}
shared_ptr<const string> XmlUIElement::GetObjectName()
{
	return GetObjectID();
}
bool XmlUIElement::CheckAttrName(const string& sAttrName)
{
	return (m_attrMap.end() != m_attrMap.find(sAttrName)) ? true : false;
}
bool XmlUIElement::CheckAttrValue(const string& sAttrName, const string& sAttrValue)
{
	map<string, string>::iterator it = m_attrValuePatternMap.find(sAttrName);
	if (it == m_attrValuePatternMap.end()) {
		INFO("CheckAttrValue Info: attribute value pattern not found.(do not need check), name: {}", sAttrName);
		return true;
	}
	regex pattern(it->second.c_str());
	if (regex_match(sAttrValue, pattern)) {
		return true;
	} else {
		ERR("CheckAttrValue error: ilegal attribute value, name: {}, value: {}", sAttrName, sAttrValue);
		return false;
	}
}
bool XmlUIElement::AddAttr(const string& sAttrName, const string& sAttrDefaultValue /* = "" */)
{
	ADD_ATTR(sAttrName, sAttrDefaultValue)
		return true;
}
bool XmlUIElement::SetAttrValue(const string& sAttrName, const string& sAttrValue)
{
	//��ǩ���е�attr��ֵ�����ᱻ�޳��ո��
#ifdef DEBUG
	if (!CheckAttrName(sAttrName)) {
		ERR("SetAttrValue error: Unsupported attribute, name: {}, value: {}.", sAttrName, sAttrValue);
		return false;
	}
	if (!CheckAttrValue(sAttrName, sAttrValue)) {
		ERR("SetAttrValue error: ilegal attribute value, name: {}, value: {}.", sAttrName, sAttrValue);
		return false;
	}
#endif // DEBUG

	m_attrMap[sAttrName] = sAttrValue;

	auto it = m_attrValueParserMap.find(sAttrName);
	if (it != m_attrValueParserMap.end()) {
		if (!it->second(sAttrName)) {
			ERR("SetAttrValue error: parse attribute value error, name: {}, value: {}", sAttrName, sAttrValue);
			return false;
		}
	}
	return true;
}
shared_ptr<const string> XmlUIElement::GetAttrValue(const string& sAttrName)
{
	//����map��[]�����������������ڵ�key��ʱ�򣬻Ὣkey���루��ʱ��value����Ĭ�Ϲ������ɣ�Ҳ����""��
	//���key����Ⱦ���˴����ò�ʹ��find����map
	if (!CheckAttrName(sAttrName)) {
		WARN("GetAttrValue warning: attribute not found, key: {}", sAttrName);
		return nullptr;
	}
	return make_shared<const string>(m_attrMap[sAttrName]);
}
bool XmlUIElement::GetInitState()
{
	return m_bInit;
}