#include "stdafx.h"
#include "UIObject.h"
#include "../entry/UIFactory.h"
#include <sstream>

//不可识别的字符转义序列（正则表达式中的\s）
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

UIObject::UIObject():m_pos(), m_parentObj(nullptr), m_pVecChildrenPair(nullptr), m_bWndObj(false)
{
	InitAttrMap();
	InitEventMap();
	InitAttrValuePatternMap();
	InitAttrValueParserMap();
}
UIObject::~UIObject()
{
	if (m_pVecChildrenPair)
		delete m_pVecChildrenPair;
	
	for (auto it=m_childrenMap.begin(); it!=m_childrenMap.end(); ++it){
		delete it->second;
	}
}
bool UIObject::InitImpl(const XMLElement* pElement)
{
	//保存element的属性
	auto pAttr = pElement->FirstAttribute();
	while (pAttr){
		auto attrName = pAttr->Name();
		SetAttrValue(attrName, pAttr->Value());
		pAttr = pAttr->Next();
	}
	//先计算自己的pos，后续的子节点才有法计算
	CalcPosFromExp();
	//为方便XML文件的编写，避免一个标签的属性列表过长
	//、过多，事件是以子标签的形式写在一个lable下面
	//保存element的事件、生成并初始化子控件
	auto pChild = pElement->FirstChildElement();
	while (pChild){
		string childClassName = pChild->Value();
		if (childClassName == "event"){
			SetEventHandler(pChild);
		}else {
			const char* cszChildID = pChild->Attribute("id") ? pChild->Attribute("id") : pChild->Attribute("name");
			cszChildID = cszChildID ? cszChildID : "";

			//auto pChildObj = CREATE(UIObject*, childClassName);
			auto pChildObj = ::CreateUIObject<UIObject*>(childClassName);
			if (pChildObj) {
				//先加到父节点，再初始化
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

	SortChildrenByZorder();
	return true;
}
void UIObject::InitAttrMap()
{
	ADD_ATTR("pos",			"")

	ADD_ATTR("visible",		"1")
	ADD_ATTR("enable",		"1")
	ADD_ATTR("alpha",		"255")
	ADD_ATTR("zorder",		"")
	ADD_ATTR("antialias",	"1")
	ADD_ATTR("limit",		"1")

	//以下属性不建议在xml中直接使用(引入空格、Tab等字符，可能出异常)
	ADD_ATTR("leftexp",		"")
	ADD_ATTR("topexp",		"")
	ADD_ATTR("widthexp",	"")
	ADD_ATTR("heightexp",	"")
}
void UIObject::InitEventMap()
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
*检查属性值是否合法
*	pos="leftexp, topexp, widthexp, heightexp"
*	leftexp、topexp    :支持0-9、#mid、#width、#height、()、+、-、*、/
*	widthexp、heightexp:支持0-9、#width、#height、()、+、-、*、/
**************************************************************************/
void UIObject::InitAttrValuePatternMap()
{
	ADD_ATTR_PATTERN("pos",			R_CHECK_POS)
	ADD_ATTR_PATTERN("visible",		R_CHECK_BOOL)
	ADD_ATTR_PATTERN("enable",		R_CHECK_BOOL)
	ADD_ATTR_PATTERN("alpha",		R_CHECK_INT)
	ADD_ATTR_PATTERN("zorder",		R_CHECK_INT)
	ADD_ATTR_PATTERN("antialias", R_CHECK_BOOL)
	ADD_ATTR_PATTERN("limit",	R_CHECK_BOOL)

	ADD_ATTR_PATTERN("leftexp",		R_CHECK_LEFTEXP)
	ADD_ATTR_PATTERN("topexp",		R_CHECK_TOPEXP)
	ADD_ATTR_PATTERN("widthexp",	R_CHECK_WIDTHEXP)
	ADD_ATTR_PATTERN("heightexp",	R_CHECK_HEIGHTEXP)
}
/*************************************************************************
*解析属性值
**************************************************************************/
void UIObject::InitAttrValueParserMap()
{
	//正则表达式的\s会匹配 tab、空格、回车

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

			//先设置宽高，因为leftexp、topexp有可能含有#mid命令，需要用到宽高表达式
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
		string replaceStr	 = "((#width-(" + selfWidth + "))/2)";//不能带进空格
		auto   strRet		 = regex_replace(m_attrMap[sAttrName], midPattern, replaceStr);

		m_attrMap["leftexp"] = strRet;
		return true;
	};

	auto ParseTopExp = [&](const string& sAttrName = "topexp")->bool {
		regex  midPattern("#mid");
		string selfHeight = m_attrMap["heightexp"];
		string replaceStr = "((#height-(" + selfHeight + "))/2)";//不能带进空格
		auto   strRet = regex_replace(m_attrMap[sAttrName], midPattern, replaceStr);

		m_attrMap["topexp"] = strRet;
		return true;
	};
	
	auto UpdateZorder = [&](const string& sAttrName)->bool {
		UIObject* pParent = GetParent();
		if (pParent && pParent->GetInitState()){
			//父节点初始化完毕，才有必要在一个孩子改变了zorder之后重新排序
			pParent->SortChildrenByZorder();
		}
		return true;
	};

	ADD_ATTR_PARSER("pos",			ParsePos)
	ADD_ATTR_PARSER("leftexp",		ParseLeftExp)
	ADD_ATTR_PARSER("topexp",		ParseTopExp)
	ADD_ATTR_PARSER("zorder",		UpdateZorder)
}
bool UIObject::CheckEventName(const string& sEventName)
{
	return (m_eventMap.end() != m_eventMap.find(sEventName)) ? true : false;
}
bool UIObject::AddEvent(const string& sEventName, UIEvent* pEventObj /* nullptr */)
{
	ADD_EVENT(sEventName, pEventObj)
	return true;
}
bool UIObject::SetEventHandler(const string& sEventName, const string& sFuncName, const string& sFilePath)
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
bool UIObject::SetEventHandler(const XMLElement* pEventElement)
{
	//name 与 id 同等对待
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

	//去掉路径、函数名两端的空白符
	auto EraseEndsSpace = [](string& str) {
		//去掉头部空白符
		for (string::iterator it = str.begin(); it != str.end(); ) {
			if ((*it) == ' ' || (*it) == '\t' || (*it) == '\n' || (*it) == '\r') {
				str.erase(it);
				it = str.begin();
				continue;
			}
			break;
		}
		//去掉尾部空白符
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
shared_ptr<UIEvent*> UIObject::GetEventHandler(const string& sEventName)
{
	if (!CheckEventName(sEventName)) {
		WARN("GetAttrValue warning: attribute not found, key: {}", sEventName);
		return nullptr;
	}
	return make_shared<UIEvent*>(m_eventMap[sEventName]);
}
UIObject* UIObject::GetParent()
{
	return m_parentObj;
}
bool UIObject::SetParent(UIObject* pParent)
{
	if (m_parentObj != nullptr) {
		ERR("SetParent error: already has parent");
		return false;
	}
	m_parentObj = pParent;
	return false;
}
bool UIObject::AddChild(UIObject* pChild, const string& sChildID /* = ""*/)
{
	if (pChild == nullptr) {
		ERR("AddChild warning: pChild is nullptr");
		return true;
	}

	shared_ptr<const string> childName = make_shared<const string>(sChildID);
	if (!childName || childName->empty()) { childName = pChild->GetObjectName();}

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
UIObject*	UIObject::GetChild(const string& sChildName)
{
	if (m_childrenMap.end() != m_childrenMap.find(sChildName)) {
		WARN("GetChild warning: child not found, name: {}", sChildName);
		return nullptr;
	}
	ATLASSERT(m_childrenMap[sChildName]);

	return m_childrenMap[sChildName];
}
bool UIObject::RemoveChild(const string& sChildName, bool bDestoryChild /*= true*/, UIObject** ppChildObj /*= nullptr*/)
{
	if (ppChildObj) { *ppChildObj = nullptr; }
	map<string, UIObject*>::iterator it = m_childrenMap.find(sChildName);
	if (m_childrenMap.end() == it) {
		WARN("RemoveChild warning: child not found, name: {}", sChildName);
		return false;
	}
	if (bDestoryChild) {
		delete (it->second);
	} else {
		ATLASSERT(ppChildObj);
		*ppChildObj = it->second;
	}
	m_childrenMap.erase(it);
	return true;
}
bool UIObject::CalcPosFromExp()
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
		//根对象不能使用#width等涉及到父对象属性值的命令
		if (strExp.find("#") != string::npos) {
			if (!m_parentObj) {
				ERR("CalcLeftOrTopPos error: can not find parent object to get parent size");
				return (int)0;
			}
			//将leftexp变为纯数学表达式
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
		//根对象不能使用#width等涉及到父对象属性值的命令
		if (strExp.find("#") != string::npos) {
			if (!m_parentObj) {
				ERR("CalcLeftOrTopPos error: can not find parent object to get parent size");
				return (unsigned int)0;
			}
			//将leftexp变为纯数学表达式
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
			//宽度、高度不能小于0
			ERR("CalcWidthOrHeight error: The calculation result of expression [{}] is less than 0。Original expression: [{}]", strExp, m_attrMap[widthOrHightExp]);
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
const UIPos UIObject::GetPosObject()
{
	return m_pos;
}
inline bool  CmpByZorder::operator()(const PAIR& k1, const PAIR& k2)
{
	shared_ptr<const string> zorderA = k1.second->GetAttrValue("zorder");
	shared_ptr<const string> zorderB = k2.second->GetAttrValue("zorder");
	return atoi(zorderA->c_str()) < atoi(zorderB->c_str());
}
void UIObject::SortChildrenByZorder()
{
	if (m_pVecChildrenPair)
	{
		delete m_pVecChildrenPair;
		m_pVecChildrenPair = nullptr;
	}
	m_pVecChildrenPair = new vector<PAIR>(m_childrenMap.begin(), m_childrenMap.end());
	ATLASSERT(m_pVecChildrenPair != nullptr);
	std::sort(m_pVecChildrenPair->begin(), m_pVecChildrenPair->end(), CmpByZorder());
}
UIPos UIObject::GetWndCoordinatePos()
{
	if (m_bWndObj) {
		UIPos pos(m_pos);
		pos.left = 0;
		pos.top  = 0;
		return pos;
	}
	if (!m_parentObj) {
		WARN("GetWndCoordinatePos warning: m_parentObj is null");
		return UIPos();
	}
	UIPos pos    = m_parentObj->GetWndCoordinatePos();
	pos.left	+= m_pos.left;
	pos.top		+= m_pos.top;
	pos.width	 = m_pos.width;
	pos.height   = m_pos.height;
	return pos;
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
///////////////////////////////////////Direct2D渲染模式相关代码///////////////////////////////////
#ifdef USE_D2D_RENDER_MODE
HRESULT	LayoutObject::OnDrawImpl(ID2D1RenderTarget* pRenderTarget,const D2D1_RECT_F& rcWndPos)
{
	return S_OK;
}
HRESULT	LayoutObject::CreateDeviceDependentResources(ID2D1RenderTarget* pRenderTarget)
{

	return S_OK;
}
HRESULT	LayoutObject::DiscardDeviceDependentResources()
{
	return S_OK;
}
/////////////////////////////////////////GDI+渲染模式相关代码/////////////////////////////////////
#else
HRESULT	LayoutObject::OnDrawImpl(Graphics& graphics, const UIPos& rcWndPos)
{
	HRESULT hr = S_OK;

	return hr;
}

#endif