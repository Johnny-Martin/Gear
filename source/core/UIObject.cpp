#include "stdafx.h"
#include "UIObject.h"
#include "UIFactory.h"
#include <sstream>
#include "Log.h"
#include <regex>

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

UIBase::UIBase():m_pos(), m_parentObj(nullptr)
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
	//先计算自己的pos，后续的子节点才有法计算
	CalcPosFromExp();
	//为方便XML文件的编写，避免一个标签的属性列表过长
	//、过多，事件是以子标签的形式写在一个lable下面
	//保存element的事件、生成并初始化子控件
	auto pChild = pElement->FirstChildElement();
	while (pChild){
		string childName = pChild->Value();
		if (childName == "event"){
			SetEventHandler(pChild);
		}else {
			auto pChildObj = CREATE(UIBase, childName);
			if (pChildObj) {
				//先加到父节点，再初始化
				AddChild(pChildObj);
				pChildObj->SetParent(this);
				pChildObj->Init(pChild);
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

	//以下属性不建议在xml中直接使用(引入空格、Tab等字符，可能出异常)
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
	//由于map的[]操作符在索引不存在的key的时候，会将key插入（此时的value调用默认构造生成，也就是""）
	//造成key的污染，此处不得不使用find遍历map
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

	//仅支持+-/*()六种操作符,sExp中除了数字之外就是六种操作符，不再有其他字符
	//不支持负数(可以用0-正数代替)，不支持小数(可以用分数表示)。
	auto CalcMathExpression = [](const string& sExp)->double {
		if (sExp.empty()) return 0.0;
		stack<double> outputStack;
		stack<char> operatorStack;

		auto IsNumber = [](const char& test) { return test >= '0' && test <= '9'; };

		auto PushToOutputStack = [&IsNumber, &outputStack](const string& originalExp, string::size_type begin) {
			string::size_type end = begin;
			while (end < originalExp.size() && IsNumber(originalExp[end])) { ++end; };
			--end;
			string strOperand = originalExp.substr(begin, end - begin + 1);
			_ASSERT(!strOperand.empty());
			int iOperand = atoi(strOperand.c_str());
			outputStack.push(iOperand);
			//返回当前操作数的末尾在串中的位置
			return end;
		};

		auto CalcOutputStack = [&outputStack](const char& op) {
			if (outputStack.size() < 2) {
				ERR("CalcOutputStack error: outputStack size: error.");
				return false;
			}
			if (op != '+' && op != '-' && op != '*' && op != '/') { return false; }
			
			double rightOp = outputStack.top();
			outputStack.pop();
			double leftOp  = outputStack.top();
			outputStack.pop();
			switch (op) {
			case '+':
				outputStack.push(leftOp + rightOp);
				break;
			case '-':
				outputStack.push(leftOp - rightOp);
				break;
			case '*':
				outputStack.push(leftOp * rightOp);
				break;
			case '/':
				//除0异常由外层捕获
				outputStack.push(leftOp / rightOp);
				break;
			default:
				ERR("CalcOutputStack error: unsupported operator: {}", op);
				return false;
			}
			return true;
		};

		for (string::size_type i = 0; i < sExp.size(); ++i) {
			//遇到操作数了，压入操作数栈
			if (IsNumber(sExp[i])) {
				i = PushToOutputStack(sExp, i);
				continue;
			}
			char curOperator = sExp[i];
			//如果是*/号，只要下一个字符是数字，就可以计算自己。否则入栈
			if (curOperator == '*' || curOperator == '/') {
				if (IsNumber(sExp[i + 1])) {
					//将下一个操作数找出、入栈，然后计算
					i = PushToOutputStack(sExp, i + 1);
					CalcOutputStack(curOperator);
					continue;
				}
				operatorStack.push(curOperator);
			}
			else if (curOperator == '+' || curOperator == '-') {
				//如果是+-号，只要栈顶不空、不是'(', 则先计算栈顶符号，再将自己入栈
				if (operatorStack.empty() || operatorStack.top() == '(') {
					operatorStack.push(curOperator);
					continue;
				}
				CalcOutputStack(operatorStack.top());
				operatorStack.pop();
				operatorStack.push(curOperator);
			}
			else if (curOperator == '(') {
				//左括号,直接入栈
				operatorStack.push(sExp[i]);
			}
			else if (curOperator == ')') {
				//边处理边计算，则符号栈中的()之间最多只有一个操作符
				if (CalcOutputStack(operatorStack.top())) {
					operatorStack.pop();
				}
				_ASSERT(operatorStack.top() == '(');
				operatorStack.pop();
			}
		}
		if (!operatorStack.empty()) {
			CalcOutputStack(operatorStack.top());
			operatorStack.pop();
		}
		return outputStack.top();
	};

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
		return D2I_Round(CalcMathExpression(strExp));
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
		int iValue = D2I_Round(CalcMathExpression(strExp));
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
	return true;
}
const UIPos UIBase::GetPosObject()
{
	return m_pos;
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
