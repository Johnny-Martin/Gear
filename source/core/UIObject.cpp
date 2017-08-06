#include "stdafx.h"
#include "UIObject.h"
#include "UIFactory.h"
#include "Log.h"

UIBase::UIBase()
{
	InitAttrMap();
	InitEventMap();
}
bool UIBase::Init(const XMLElement* pElement)
{
	//保存element的属性
	auto pAttr = pElement->FirstAttribute();
	while (pAttr){
		auto attrName = pAttr->Name();
		SetAttrValue(attrName, pAttr->Value());
		pAttr = pAttr->Next();
	}

	//为方便XML文件的编写，避免一个标签的属性列表过长
	//、过多，事件是以子标签的形式写在一个lable下面
	//保存element的事件、生成并初始化子控件
	auto pChild = pElement->FirstChildElement();
	while (pChild){
		string childName = pChild->Value();
		if (childName == "event"){
			SetEventHandler(pChild);
		}else {
			auto pObj = CREATE(UIBase, childName);
			if (pObj) {
				pObj->Init(pChild);
			}
		}

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
const string UIBase::GetObjectID()
{
	if (m_attrMap.empty()) {
		ERR("GetObjectID error: m_attrMap has not been initicalized");
		return nullptr;
	}
	string name = m_attrMap["id"];
	if (name.empty())
		name = m_attrMap["name"];

	return name;
}
const string UIBase::GetObjectName()
{
	return GetObjectID();
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
	if (!CheckAttrName(strName)) {
		ERR("SetAttrValue error: Unsupported attr name: {}, value: {}.", strName, strValue);
		return false;
	}
#endif // DEBUG

	m_attrMap[strName] = strValue;

	//name 与 id 同等对待
	if(strName == "name")
		m_attrMap["id"] = strValue;
	else if(strName == "id")
		m_attrMap["name"] = strValue;

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
	if (!CheckEventName(strName)) {
		ERR("SetEventHandler error: Unsupported event name: {}, handler: {}.", strName, strValue);
		return false;
	}
		
#endif // DEBUG
	m_eventMap[strName] = strValue;
	return true;
}
bool UIBase::SetEventHandler(const XMLElement* pEventElement)
{
	//name 与 id 同等对待
	auto eventName = pEventElement->Attribute("name");
	if(eventName == nullptr)
		eventName = pEventElement->Attribute("id");

	if (eventName == nullptr) {
		ERR("SetEventHandler error: No event name specified");
		return false;
	}

	auto eventHandler = pEventElement->Attribute("func");
	if (eventHandler == nullptr) {
		ERR("SetEventHandler error: No event handler specified");
		return false;
	}
	return SetEventHandler(eventName, eventHandler);
}
const string& UIBase::GetEventHandler(const string& strName)
{
	map<string, string>::iterator itPos = m_eventMap.find(strName);
	if (itPos == m_eventMap.end())
		return "";

	return m_eventMap[strName];
}
UIBase* UIBase::GetParent()
{
	return m_parentObj;
}
bool UIBase::SetParent(UIBase* pParent)
{
	if (m_parentObj != nullptr) {
		ERR("SetParent error: already has parent");
		return false;
	}
	m_parentObj = pParent;
	return false;
}
bool UIBase::AddChild(UIBase* pChild)
{
	if (pChild == nullptr) {
		WARN("AddChild warning: pChild is nullptr");
		return true;
	}

	string childName = pChild->GetObjectName();
	if (childName.empty() || childName.length() == 0) {
		//匿名对象，不支持
		ERR("AddChild error: anonymous object is not supported");
		return false;
	}

	UIBase* pTemp = m_childrenMap[childName];
	if (pTemp != nullptr) {
		ERR("AddChild error: Homonymous object already exisit");
		return false;
	}

	return true;
}
LayoutObject::LayoutObject()
{
	InitAttrMap();
	InitEventMap();
}

void LayoutObject::InitAttrMap()
{

}
void LayoutObject::InitEventMap()
{

}
