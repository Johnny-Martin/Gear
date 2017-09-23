#pragma once
#include "../stdafx.h"

using namespace std;
using namespace tinyxml2;

//�������Ե�XML��ǩ�ĳ��󣨲��������ӹ�ϵ��
class XmlUIElement
{
public:
												XmlUIElement();
protected:
	void										InitAttrMap();
	void										InitAttrValuePatternMap();
	void										InitAttrValueParserMap();
public:
	virtual bool								Init(const XMLElement* pElement);
	virtual bool								InitImpl(const XMLElement* pElement);
	shared_ptr<const string>					GetObjectID();
	shared_ptr<const string>					GetObjectName();
	bool										CheckAttrName(const string& sAttrName);
	bool										CheckAttrValue(const string& sAttrName, const string& sAttrValue);
	bool										SetAttrValue(const string& sAttrName, const string& sAttrValue);
	shared_ptr<const string>					GetAttrValue(const string& sAttrName);
	bool										AddAttr(const string& sAttrName, const string& sAttrDefaultValue = "");
	bool										GetInitState();
protected:
	bool										m_bInit;//false:δ��ʼ�����ʼ��ʧ�ܣ�true:��ʼ���ɹ�
	map<string, string>							m_attrMap;
	map<string, string>							m_attrValuePatternMap;
	map<string, function<bool(const string&)> >	m_attrValueParserMap;
};