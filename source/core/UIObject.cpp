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
				//先初始化，再加到父节点
				pObj->Init(pChild);
				AddChild(pObj);
			}
		}
		m_childrenMap;
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
shared_ptr<const string> UIBase::GetObjectID()
{
	if (m_attrMap.empty()) {
		ERR("GetObjectID error: m_attrMap has not been initicalized");
		return nullptr;
	}
	//id 与 name这两个key是一定存在的，无须担心map的[]操作符意外插入陌生的key
	if (!m_attrMap["id"].empty())
		return make_shared<const string>(m_attrMap["id"]);

	if (!m_attrMap["name"].empty())
		return make_shared<const string>(m_attrMap["name"]);

	return nullptr;
}
shared_ptr<const string> UIBase::GetObjectName()
{
	return GetObjectID();
}
bool UIBase::CheckAttrName(const string& sAttrName)
{
	return (m_attrMap.end() != m_attrMap.find(sAttrName)) ? true : false;
}
bool UIBase::CheckEventName(const string& sEventName)
{
	return (m_eventMap.end() != m_eventMap.find(sEventName)) ? true : false;
}
bool UIBase::AddAttr(const string& sAttrName, const string& sAttrDefaultValue /* = "" */)
{
	ADD_ATTR(sAttrName, sAttrDefaultValue)
	return true;
}
bool UIBase::SetAttrValue(const string& sAttrName, const string& sAttrValue)
{
#ifdef DEBUG
	if (!CheckAttrName(sAttrName)) {
		ERR("SetAttrValue error: Unsupported attr name: {}, value: {}.", sAttrName, sAttrValue);
		return false;
	}
#endif // DEBUG

	m_attrMap[sAttrName] = sAttrValue;

	//name 与 id 同等对待
	if(sAttrName == "name")
		m_attrMap["id"] = sAttrValue;
	else if(sAttrName == "id")
		m_attrMap["name"] = sAttrValue;

	return true;
}
shared_ptr<const string> UIBase::GetAttrValue(const string& sAttrName)
{
	//由于map的[]操作符在索引不存在的key的时候，会将key插入（此时的value调用默认构造生成，也就是""）
	//造成key的污染，此处不得不使用find遍历map
	if (!CheckAttrName(sAttrName)) {
		WARN("GetAttrValue warning: attribute not found, key: {}", sAttrName);
		return nullptr;
	}
	return make_shared<const string>(m_attrMap[sAttrName]);
}
bool UIBase::AddEvent(const string& sEventName, const string& sEventDefaultValue /* = "" */)
{
	ADD_EVENT(sEventName, sEventDefaultValue)
	return true;
}
bool UIBase::SetEventHandler(const string& sEventName, const string& sEventValue)
{
#ifdef DEBUG
	if (!CheckEventName(sEventName)) {
		ERR("SetEventHandler error: Unsupported event name: {}, handler: {}.", sEventName, sEventValue);
		return false;
	}
		
#endif // DEBUG
	m_eventMap[sEventName] = sEventValue;
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
shared_ptr<const string> UIBase::GetEventHandler(const string& sEventName)
{
	if (!CheckEventName(sEventName)) {
		WARN("GetAttrValue warning: attribute not found, key: {}", sEventName);
		return nullptr;
	}
	return make_shared<const string>(m_eventMap[sEventName]);
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
		ERR("AddChild warning: pChild is nullptr");
		return true;
	}

	shared_ptr<const string> childName = pChild->GetObjectName();
	if (!childName || childName->empty()) {
		//匿名对象，不支持
		ERR("AddChild error: anonymous object is not supported");
		return false;
	}

	if (m_childrenMap.end() != m_childrenMap.find(*childName)) {
		ERR("AddChild error: Homonymous object already exisit");
		return false;
	}
	
	m_childrenMap[*childName] = pChild;
	return true;
}
UIBase*	UIBase::GetChild(const string& sChildName)
{
	if (m_childrenMap.end() != m_childrenMap.find(sChildName)) {
		WARN("GetChild warning: child not found, name: {}", sChildName);
		return nullptr;
	}
	ATLASSERT(m_childrenMap[sChildName]);

	return m_childrenMap[sChildName];
}
bool UIBase::RemoveChild(const string& sChildName)
{
	map<string, UIBase*>::iterator it = m_childrenMap.find(sChildName);
	if (m_childrenMap.end() == it) {
		WARN("RemoveChild warning: child not found, name: {}", sChildName);
		return false;
	}
	m_childrenMap.erase(it);
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
