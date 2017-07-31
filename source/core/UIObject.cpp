#include "stdafx.h"
#include "UIObject.h"
#include "UIFactory.h"

UIBase::UIBase()
{
	InitAttrMap();
	InitEventMap();
}
bool UIBase::Init(const XMLElement* pElement)
{
	//保存element的属性
	auto pAttr = pElement->FirstAttribute();
	while (pAttr)
	{
		auto attrName = pAttr->Name();
		if (CheckAttrName(attrName))
		{
			SetAttrValue(attrName, pAttr->Value());
		}
		pAttr = pAttr->Next();
	}

	//为方便XML文件的编写，避免一个标签的属性列表过长/过多，
	//事件是以子标签的形式写在一个lable下面
	//保存element的事件，生成并初始化子控件
	auto pChild = pElement->FirstChildElement();
	while (pChild)
	{
		auto childName = pChild->Value();
		if (childName == "event" || SetEventHandler(pChild))
			continue;

		auto pObj = CREATE(UIBase, childName);
		if (pObj && pObj->Init(pChild))
			continue;

		//出错了，事件名错误，或者是pChild内的错误

		pChild = pChild->NextSiblingElement();
	}
	return true;
}
void UIBase::InitAttrMap()
{
	
	ADD_ATTR("left",		"0")
	ADD_ATTR("top",			"0")
	ADD_ATTR("right",		"0")
	ADD_ATTR("bottom",		"0")
	ADD_ATTR("visible",		"1")
	ADD_ATTR("enable",		"1")

	ADD_ATTR("id",			"")
	ADD_ATTR("name",		"")
	ADD_ATTR("pos",			"")
	ADD_ATTR("leftexp",		"")
	ADD_ATTR("topexp",		"")
	ADD_ATTR("rightexp",	"")
	ADD_ATTR("bottomexp",	"")
	ADD_ATTR("rightexp",	"")

}
void UIBase::InitEventMap()
{
	ADD_EVENT("OnCreate",			"")
	ADD_EVENT("OnDestory",			"")
	ADD_EVENT("OnVisibleChange",	"")
	ADD_EVENT("OnEnableChange",		"")
	ADD_EVENT("OnMove",				"")
	ADD_EVENT("OnSize",				"")
	ADD_EVENT("OnMouseEnter",		"")
	ADD_EVENT("OnMouseLeave",		"")
	ADD_EVENT("OnMouseMove",		"")
}

bool UIBase::CheckAttrName(const string& strName)
{
	return (m_attrMap.end() != m_attrMap.find(strName)) ? true : false;
}
bool UIBase::CheckEventName(const string& strName)
{
	return (m_eventMap.end() != m_eventMap.find(strName)) ? true : false;
}
bool UIBase::AddAttrName(const string& strName, const string& strDefaultValue /* = "" */)
{
	ADD_ATTR(strName, strDefaultValue)
	return true;
}
bool UIBase::SetAttrValue(const string& strName, const string& strValue)
{
#ifdef DEBUG
	if (!CheckAttrName(strName))
		return false;
#endif // DEBUG
	m_attrMap[strName] = strValue;
	return true;
}
const string& UIBase::GetAttrValue(const string& strName)
{
	return m_attrMap[strName];
}
bool UIBase::AddEventName(const string& strName, const string& strDefaultValue /* = "" */)
{
	ADD_EVENT(strName, strDefaultValue)
	return true;
}
bool UIBase::SetEventHandler(const string& strName, const string& strValue)
{
#ifdef DEBUG
	if (!CheckEventName(strName))
		return false;
#endif // DEBUG
	m_eventMap[strName] = strValue;
	return true;
}
bool UIBase::SetEventHandler(const XMLElement* pEventElement)
{
	auto eventName = pEventElement->Attribute("name");
	if (eventName && CheckEventName(eventName))
	{
		auto eventHandler = pEventElement->Attribute("func");
		return SetEventHandler(eventName, eventHandler);
	}

	return false;
}
const string& UIBase::GetEventHandler(const string& strName)
{
	return m_eventMap[strName];
}

set<string> LayoutObject::m_eventNameSet = LayoutObject::InitEventNameSet();

bool LayoutObject::InitAttrMap()
{
	m_attrMap.insert(pair<string, string>("topmost", "0"));
	m_attrMap.insert(pair<string, string>("layered", "1"));
	m_attrMap.insert(pair<string, string>("appwnd", "1"));
	m_attrMap.insert(pair<string, string>("blur", "0"));
	m_attrMap.insert(pair<string, string>("minenable", "1"));
	m_attrMap.insert(pair<string, string>("maxenable", "1"));
	m_attrMap.insert(pair<string, string>("rootobjectid", ""));
	return true;
}
set<string> LayoutObject::InitEventNameSet()
{
	set<string> eventNameSet;
	eventNameSet.insert("OnCreate");
	eventNameSet.insert("OnShowWnd");
	eventNameSet.insert("OnShowWnd");
	eventNameSet.insert("OnDestory");
	eventNameSet.insert("OnStateChange");
	eventNameSet.insert("OnMove");
	eventNameSet.insert("OnSize");
	eventNameSet.insert("OnVisibleChange");
	eventNameSet.insert("OnEnableChange");
	eventNameSet.insert("OnCreate");

	return eventNameSet;
}
bool LayoutObject::SetAttr(string key, string value)
{
	if (CheckAttrName(key))
	{
		m_attrMap.insert(pair<string, string>(key, value));
		return true;
	}
	return false;
}
bool LayoutObject::GetAttr(string key, string* value)
{
	map<string, string>::iterator iter = m_attrMap.find(key);
	if (m_attrMap.end() != iter)
	{
		*value = iter->second;
		return true;
	}
	return false;
}
bool LayoutObject::CheckAttrName(string strName)
{ 
	return (m_attrMap.end() != m_attrMap.find(strName)) ? true:false;
}
bool LayoutObject::CheckEventName(string strName)
{ 
	return (m_eventMap.end() != m_eventMap.find(strName)) ? true:false;
}

