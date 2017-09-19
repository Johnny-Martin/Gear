#pragma once
#include "../stdafx.h"

using namespace std;
using namespace tinyxml2;

//带有属性的XML标签的抽象（不包括父子关系）
class XmlUIElement
{
public:
												XmlUIElement();
protected:
	void										InitAttrMap();
	void										InitAttrValuePatternMap();
	void										InitAttrValueParserMap();
public:
	virtual bool								Init(const XMLElement* pElement) = 0;
	shared_ptr<const string>					GetObjectID();
	shared_ptr<const string>					GetObjectName();
	bool										CheckAttrName(const string& sAttrName);
	bool										CheckAttrValue(const string& sAttrName, const string& sAttrValue);
	bool										SetAttrValue(const string& sAttrName, const string& sAttrValue);
	shared_ptr<const string>					GetAttrValue(const string& sAttrName);
	bool										AddAttr(const string& sAttrName, const string& sAttrDefaultValue = "");
protected:
	map<string, string>							m_attrMap;
	map<string, string>							m_attrValuePatternMap;
	map<string, function<bool(const string&)> >	m_attrValueParserMap;
};