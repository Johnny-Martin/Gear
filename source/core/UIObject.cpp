#include "stdafx.h"
#include "UIObject.h"
#include "UIFactory.h"
#include <sstream>
#include "Log.h"
#include <regex>
#include "Util.h"

//����ʶ����ַ�ת�����У�������ʽ�е�\s��
#pragma warning(disable:4129)

bool UIEvent::SetEventHandlerFilePath(const string& sPath)
{
	m_filePath = sPath;
	return true;
}

bool UIEvent::SetEventHandlerFuncName(const string& sName)
{
	m_funcName = sName;
	return true;
}

shared_ptr<const string> UIEvent::GetEventHandlerFilePath()
{
	return make_shared<const string>(m_filePath);
}

shared_ptr<const string> UIEvent::GetEventHandlerFuncName()
{
	return make_shared<const string>(m_funcName);
}

UIBase::UIBase():m_pos(), m_parentObj(nullptr)
{
	InitAttrMap();
	InitEventMap();
	InitAttrValuePatternMap();
	InitAttrValueParserMap();
}
bool UIBase::Init(const XMLElement* pElement)
{
	//����element������
	auto pAttr = pElement->FirstAttribute();
	while (pAttr){
		auto attrName = pAttr->Name();
		SetAttrValue(attrName, pAttr->Value());
		pAttr = pAttr->Next();
	}
	//�ȼ����Լ���pos���������ӽڵ���з�����
	CalcPosFromExp();
	//Ϊ����XML�ļ��ı�д������һ����ǩ�������б����
	//�����࣬�¼������ӱ�ǩ����ʽд��һ��lable����
	//����element���¼������ɲ���ʼ���ӿؼ�
	auto pChild = pElement->FirstChildElement();
	while (pChild){
		string childClassName = pChild->Value();
		if (childClassName == "event"){
			SetEventHandler(pChild);
		}else {
			const char* cszChildID = pChild->Attribute("id") ? pChild->Attribute("id") : pChild->Attribute("name");
			cszChildID = cszChildID ? cszChildID : "";

			auto pChildObj = CREATE(UIBase, childClassName);
			if (pChildObj) {
				//�ȼӵ����ڵ㣬�ٳ�ʼ��
				AddChild(pChildObj, cszChildID);
				pChildObj->SetParent(this);
				pChildObj->Init(pChild);
			}else {
				ERR("Init error: create object failed, childClassName: {}", childClassName);
				return false;
			}
		}
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

	//�������Բ�������xml��ֱ��ʹ��(����ո�Tab���ַ������ܳ��쳣)
	ADD_ATTR("leftexp",		"")
	ADD_ATTR("topexp",		"")
	ADD_ATTR("widthexp",	"")
	ADD_ATTR("heightexp",	"")
}
void UIBase::InitEventMap()
{
	ADD_EVENT("OnCreate",			nullptr)
	ADD_EVENT("OnDestory",			nullptr)
	ADD_EVENT("OnVisibleChange",	nullptr)
	ADD_EVENT("OnEnableChange",		nullptr)
	ADD_EVENT("OnMove",				nullptr)
	ADD_EVENT("OnSize",				nullptr)
	ADD_EVENT("OnMouseEnter",		nullptr)
	ADD_EVENT("OnMouseLeave",		nullptr)
	ADD_EVENT("OnMouseMove",		nullptr)
}
/*************************************************************************
*�������ֵ�Ƿ�Ϸ�
*	pos="leftexp, topexp, widthexp, heightexp"
*	leftexp��topexp    :֧��0-9��#mid��#width��#height��()��+��-��*��/
*	widthexp��heightexp:֧��0-9��#width��#height��()��+��-��*��/
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
*��������ֵ
**************************************************************************/
void UIBase::InitAttrValueParserMap()
{
	//������ʽ��\s��ƥ�� tab���ո񡢻س�
	auto EraseSpace = [](string& str) {
		for (string::iterator it = str.end(); it != str.begin();) {
			--it;
			if ((*it) == ' ' || (*it) == '\t' || (*it) == '\n' || (*it) == '\r') {
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

			//ȥ�����ʽ�еĿհ׷�
			EraseSpace(leftexp);
			EraseSpace(topexp);
			EraseSpace(widthexp);
			EraseSpace(heightexp);

			//�����ÿ�ߣ���Ϊleftexp��topexp�п��ܺ���#mid�����Ҫ�õ���߱��ʽ
			SetAttrValue("widthexp",  widthexp);
			SetAttrValue("heightexp", heightexp);
			SetAttrValue("leftexp",	  leftexp);
			SetAttrValue("topexp",	  topexp);
			
			return true;
		}catch (...) {
			ERR("ParsePos error: catch exception");
			return false;
		}
	};

	auto ParseLeftExp = [&](const string& sAttrName = "leftexp")->bool {
		regex  midPattern("#mid");
		string selfWidth	 = m_attrMap["widthexp"];
		string replaceStr	 = "((#width-(" + selfWidth + "))/2)";//���ܴ����ո�
		auto   strRet		 = regex_replace(m_attrMap[sAttrName], midPattern, replaceStr);

		m_attrMap["leftexp"] = strRet;
		return true;
	};

	auto ParseTopExp = [&](const string& sAttrName = "topexp")->bool {
		regex  midPattern("#mid");
		string selfHeight = m_attrMap["heightexp"];
		string replaceStr = "((#height-(" + selfHeight + "))/2)";//���ܴ����ո�
		auto   strRet = regex_replace(m_attrMap[sAttrName], midPattern, replaceStr);

		m_attrMap["topexp"] = strRet;
		return true;
	};
	
	ADD_ATTR_PARSER("pos",			ParsePos);
	ADD_ATTR_PARSER("leftexp",		ParseLeftExp);
	ADD_ATTR_PARSER("topexp",		ParseTopExp);
}
shared_ptr<const string> UIBase::GetObjectID()
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

	//name �� id ͬ�ȶԴ�
	if(sAttrName == "name")
		m_attrMap["id"] = sAttrValue;
	else if(sAttrName == "id")
		m_attrMap["name"] = sAttrValue;

	auto it = m_attrValueParserMap.find(sAttrName);
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
	//����map��[]�����������������ڵ�key��ʱ�򣬻Ὣkey���루��ʱ��value����Ĭ�Ϲ������ɣ�Ҳ����""��
	//���key����Ⱦ���˴����ò�ʹ��find����map
	if (!CheckAttrName(sAttrName)) {
		WARN("GetAttrValue warning: attribute not found, key: {}", sAttrName);
		return nullptr;
	}
	return make_shared<const string>(m_attrMap[sAttrName]);
}
bool UIBase::AddEvent(const string& sEventName, UIEvent* pEventObj /* nullptr */)
{
	ADD_EVENT(sEventName, pEventObj)
	return true;
}
bool UIBase::SetEventHandler(const string& sEventName, const string& sFuncName, const string& sFilePath)
{
	
#ifdef DEBUG
	if (!CheckEventName(sEventName)) {
		ERR("SetEventHandler error: Unsupported event name: {}, handler: {}.", sEventName, sFuncName);
		return false;
	}
	auto CheckFuncName = [&sEventName, &sFuncName]()->bool {
		regex funcNamePattern("[0-9A-Za-z_]+");
		if (!regex_match(sFuncName, funcNamePattern)) {
			ERR("SetEventHandler CheckFuncName error: ilegal function name, event name: {}, function name: {}", sEventName, sFuncName);
			return false;
		}
		return true;
	};

	if (!CheckFuncName()) return false;

	auto it = m_eventMap.find(sEventName);
	if (it != m_eventMap.end() && it->second) {
		WARN("SetEventHandler  warning: event handler already exisit, will be recoverd! event name: {}, old function name: {}, new function name: {}", sEventName, *(it->second->GetEventHandlerFuncName()), sFuncName);
	}
#endif // DEBUG
	
	UIEvent* pEventObj = m_eventMap[sEventName] ? m_eventMap[sEventName] : (new UIEvent());
	if (!pEventObj) {
		ERR("SetEventHandler error: get event object failed, name: {}, function: {}", sEventName, sFuncName);
		return false;
	}
	pEventObj->SetEventHandlerFilePath(sFilePath);
	pEventObj->SetEventHandlerFuncName(sFuncName);
	
	m_eventMap[sEventName] = pEventObj;
	return true;
}
bool UIBase::SetEventHandler(const XMLElement* pEventElement)
{
	//name �� id ͬ�ȶԴ�
	auto eventName = pEventElement->Attribute("name");
	if(eventName == nullptr)
		eventName = pEventElement->Attribute("id");

	if (eventName == nullptr) {
		ERR("SetEventHandler error: No event name specified");
		return false;
	}

	auto szFuncName = pEventElement->Attribute("func");
	auto szFilePath = pEventElement->Attribute("file");

	string sFuncName = szFuncName ? szFuncName : eventName;
	string sFilePath = szFilePath ? szFilePath : "";

	//ȥ��·�������������˵Ŀհ׷�
	auto EraseEndsSpace = [](string& str) {
		//ȥ��ͷ���հ׷�
		for (string::iterator it = str.begin(); it != str.end(); ) {
			if ((*it) == ' ' || (*it) == '\t' || (*it) == '\n' || (*it) == '\r') {
				str.erase(it);
				it = str.begin();
				continue;
			}
			break;
		}
		//ȥ��β���հ׷�
		for (string::iterator it = str.end(); it != str.begin();) {
			--it;
			if ((*it) == ' ' || (*it) == '\t' || (*it) == '\n' || (*it) == '\r') {
				str.erase(it);
				it = str.end();
				continue;
			}
			break;
		}
	};

	EraseEndsSpace(sFuncName);
	EraseEndsSpace(sFilePath);

	return SetEventHandler(eventName, sFuncName, sFilePath);
}
shared_ptr<UIEvent*> UIBase::GetEventHandler(const string& sEventName)
{
	if (!CheckEventName(sEventName)) {
		WARN("GetAttrValue warning: attribute not found, key: {}", sEventName);
		return nullptr;
	}
	return make_shared<UIEvent*>(m_eventMap[sEventName]);
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
bool UIBase::AddChild(UIBase* pChild, const string& sChildID /* = ""*/)
{
	if (pChild == nullptr) {
		ERR("AddChild warning: pChild is nullptr");
		return true;
	}

	shared_ptr<const string> childName = make_shared<const string>(sChildID);
	if (!childName || childName->empty()) { childName = pChild->GetObjectName();}

	if (!childName || childName->empty()) {
		//�������󣬲�֧��
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
bool UIBase::CalcPosFromExp()
{
	auto I2Str = [](auto param)->string{
		stringstream strStream;
		string ret;
		strStream << param;
		strStream >> ret;
		return ret;
	};

	auto D2I_Round = [](const double& input) { return (int)(input >= 0 ? (input + 0.5) : (input - 0.5)); };

	auto CalcLeftOrTopPos = [&](const string& leftOrTopExp)->int {
		string strExp = m_attrMap[leftOrTopExp];
		if (strExp.empty()) {
			WARN("CalcLeftOrTopPos warning: {} expression not found, use default: 0", leftOrTopExp);
			return (int)0;
		}
		//��������ʹ��#width���漰������������ֵ������
		if (strExp.find("#") != string::npos) {
			if (!m_parentObj) {
				ERR("CalcLeftOrTopPos error: can not find parent object to get parent size");
				return (int)0;
			}
			//��leftexp��Ϊ����ѧ���ʽ
			strExp = regex_replace(strExp, regex("#width"),  I2Str(m_parentObj->GetPosObject().width));
			strExp = regex_replace(strExp, regex("#height"), I2Str(m_parentObj->GetPosObject().height));
		}
		try {
			return D2I_Round(CalcMathExpression(strExp));
		}catch (...) {
			ERR("CalcLeftOrTopPos error: exception occured. attribute name: {}, expression: {}", leftOrTopExp, strExp);
			return 0;
		}
	};

	auto CalcWidthOrHeight = [&](const string& widthOrHightExp)->unsigned int {
		string strExp = m_attrMap[widthOrHightExp];
		if (strExp.empty()) {
			WARN("CalcWidthOrHeight warning: {} expression not found, use default: 0", widthOrHightExp);
			return (unsigned int)0;
		}
		//��������ʹ��#width���漰������������ֵ������
		if (strExp.find("#") != string::npos) {
			if (!m_parentObj) {
				ERR("CalcLeftOrTopPos error: can not find parent object to get parent size");
				return (unsigned int)0;
			}
			//��leftexp��Ϊ����ѧ���ʽ
			strExp = regex_replace(strExp, regex("#width"),  I2Str(m_parentObj->GetPosObject().width));
			strExp = regex_replace(strExp, regex("#height"), I2Str(m_parentObj->GetPosObject().height));
		}

		int iValue = 0;
		try {
			iValue = D2I_Round(CalcMathExpression(strExp));
		}catch (...) {
			ERR("CalcWidthOrHeight error, exception occured. attribute name: {}, expression: {}", widthOrHightExp, strExp);
			return (unsigned int)0;
		}
		
		if (iValue < 0) {
			//��ȡ��߶Ȳ���С��0
			ERR("CalcWidthOrHeight error: The calculation result of expression [{}] is less than 0��Original expression: [{}]", strExp, m_attrMap[widthOrHightExp]);
			return (unsigned int)0;
		}
		return (unsigned int)iValue;
	};

	m_pos.left	 = CalcLeftOrTopPos("leftexp");
	m_pos.top	 = CalcLeftOrTopPos("topexp");
	m_pos.width  = CalcWidthOrHeight("widthexp");
	m_pos.height = CalcWidthOrHeight("heightexp");

	INFO("CalcPosFromExp info: left: {}, top: {}, width: {}, height: {}", m_pos.left, m_pos.top, m_pos.width, m_pos.height);
	return true;
}
const UIPos UIBase::GetPosObject()
{
	return m_pos;
}

/********************************************
������#���������UIBase���ṩposһ��������
������Ľ���(#mid #left #top #width #height)
�������Ľ����������������leftexp topexp...
*********************************************/
bool UIBase::ParseSpecialCmd()
{
	//����pos����

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
