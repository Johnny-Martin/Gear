#include "stdafx.h"
#include "UIObject.h"
#include "UIFactory.h"
#include "Log.h"
#include <regex>

//不可识别的字符转义序列（正则表达式中的\s）
#pragma warning(disable:4129)

bool UIEvent::SetEventHandlerFilePath(const string& sPath)
{
	m_filePath = sPath;
	return true;
}

bool UIEvent::SetEventHandlerName(const string& sName)
{
	m_funcName = sName;
	return true;
}

shared_ptr<const string> UIEvent::GetEventHandlerFilePath()
{
	return make_shared<const string>(m_filePath);
}

shared_ptr<const string> UIEvent::GetEventHandlerName()
{
	return make_shared<const string>(m_funcName);
}

UIBase::UIBase()
{
	InitAttrMap();
	InitEventMap();
	InitAttrValuePatternMap();
	InitAttrValueParserMap();
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
	ADD_ATTR("id",			"")
	ADD_ATTR("name",		"")
	ADD_ATTR("pos",			"")

	ADD_ATTR("visible",		"1")
	ADD_ATTR("enable",		"1")
	ADD_ATTR("alpha",		"255")
	ADD_ATTR("zorder",		"")

	//以下属性不建议在xml中直接使用(引入空格、Tab等字符，可能出异常)
	ADD_ATTR("leftexp",		"")
	ADD_ATTR("topexp",		"")
	ADD_ATTR("widthexp",	"")
	ADD_ATTR("heightexp",	"")
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
/*************************************************************************
*检查属性值是否合法
*	pos="leftexp, topexp, widthexp, heightexp"
*	leftexp、topexp    :支持0-9、#mid、#width、#height、()、+、-、*、/
*	widthexp、heightexp:支持0-9、#width、#height、()、+、-、*、/
**************************************************************************/
void UIBase::InitAttrValuePatternMap()
{
	ADD_ATTR_PATTERN("id",			R_CHECK_ID);
	ADD_ATTR_PATTERN("name",		R_CHECK_ID);
	ADD_ATTR_PATTERN("pos",			R_CHECK_POS);
	ADD_ATTR_PATTERN("visible",		R_CHECK_BOOL);
	ADD_ATTR_PATTERN("enable",		R_CHECK_BOOL);
	ADD_ATTR_PATTERN("alpha",		R_CHECK_INT);
	ADD_ATTR_PATTERN("zorder",		R_CHECK_INT);

	ADD_ATTR_PATTERN("leftexp",		R_CHECK_LEFTEXP);
	ADD_ATTR_PATTERN("topexp",		R_CHECK_TOPEXP);
	ADD_ATTR_PATTERN("widthexp",	R_CHECK_WIDTHEXP);
	ADD_ATTR_PATTERN("heightexp",	R_CHECK_HEIGHTEXP);
}
/*************************************************************************
*解析属性值
**************************************************************************/
void UIBase::InitAttrValueParserMap()
{
	//正则表达式的\s会匹配 tab、空格、回车
	auto EraseSpace = [](string& str) {
		for (string::iterator it = str.end(); it != str.begin();) {
			--it;
			if ((*it) == ' ' || (*it) == '\t') {
				str.erase(it);
			}
		}
	};

	auto ParsePos = [&](const string& sAttrName="pos")->bool{
		try {
			auto sPattern = m_attrValuePatternMap[sAttrName];
			auto posExp = m_attrMap[sAttrName];

			regex pattern(sPattern.c_str());
			string leftexp   = regex_replace(posExp, pattern, string("$2"));
			string topexp    = regex_replace(posExp, pattern, string("$3"));
			string widthexp  = regex_replace(posExp, pattern, string("$4"));
			string heightexp = regex_replace(posExp, pattern, string("$5"));

			//去除表达式中的空白符
			EraseSpace(leftexp);
			EraseSpace(topexp);
			EraseSpace(widthexp);
			EraseSpace(heightexp);

			SetAttrValue("leftexp",	  leftexp);
			SetAttrValue("topexp",	  topexp);
			SetAttrValue("widthexp",  widthexp);
			SetAttrValue("heightexp", heightexp);
			return true;
		}catch (...) {
			ERR("ParsePos error: catch exception");
			return false;
		}
	};

	auto ParseLeftExp = [](const string& sAttrName = "leftexp")->bool {

		return true;
	};

	auto ParseWidthExp = [](const string& sAttrName = "leftexp")->bool {

		return true;
	};

	ADD_ATTR_PARSER("pos",			ParsePos);
	ADD_ATTR_PARSER("leftexp",		ParseLeftExp);
	ADD_ATTR_PARSER("topexp",		ParseLeftExp);
	ADD_ATTR_PARSER("widthexp",		ParseWidthExp);
	ADD_ATTR_PARSER("heightexp",	ParseWidthExp);
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
bool UIBase::CheckAttrValue(const string& sAttrName, const string& sAttrValue)
{
	map<string, string>::iterator it = m_attrValuePatternMap.find(sAttrName);
	if (it == m_attrValuePatternMap.end()) {
		INFO("CheckAttrValue Info: attribute value pattern not found.(do not need check), name: {}", sAttrName);
		return true;
	}
	regex pattern(it->second.c_str());
	if (regex_match(sAttrValue, pattern)) {
		return true;
	}else {
		ERR("CheckAttrValue error: ilegal attribute value, name: {}, value: {}", sAttrName, sAttrValue);
		return false;
	}
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
	//标签所有的attr的值，都会被剔除空格符
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

	//name 与 id 同等对待
	if(sAttrName == "name")
		m_attrMap["id"] = sAttrValue;
	else if(sAttrName == "id")
		m_attrMap["name"] = sAttrValue;

	map<string, function<bool(const string&)> >::iterator it = m_attrValueParserMap.find(sAttrName);
	if (it != m_attrValueParserMap.end()) {
		if (!it->second(sAttrName)) {
			ERR("SetAttrValue error: parse attribute value error, name: {}, value: {}", sAttrName, sAttrValue);
			return false;
		}
	}
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
		ERR("SetEventHandler info: No event handler specified, use event name: {}", eventName);
		//不指定func字段，则到同名lua下找eventName同名function
		return SetEventHandler(eventName, eventName);
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

/********************************************
解析带#的特殊命令，UIBase仅提供pos一个属性内
种命令的解析(#mid #left #top #width #height)
五个命令的解析，并将结果存入leftexp topexp...
*********************************************/
bool UIBase::ParseSpecialCmd()
{
	//解析pos属性

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
