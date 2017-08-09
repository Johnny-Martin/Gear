#ifndef UIOBJECT_H
#define UIOBJECT_H
#include "stdafx.h"
#include "tinyxml2.h"
#include <memory>
using namespace std;
using namespace tinyxml2;

namespace Gear {
	namespace UI {

	}//namespace UI
}//namespace Gear

#define ADD_ATTR(attrName, defaultValue)	\
		m_attrMap.insert(pair<string, string>(attrName, defaultValue));

#define ADD_EVENT(attrName, defaultValue)	\
		m_eventMap.insert(pair<string, string>(attrName, defaultValue));

#define ADD_ATTR_PATTERN(attrName, sPattern)	\
		m_attrCmdPatternMap.insert(pair<string, string>(attrName, sPattern));

/*******************************************************************************
*���� 5 �������ڴ��Լ��pos���ʽ�ĺϷ���
*�Ƿ���4��
*�Ƿ��д�����ַ�
*pos���ʽ�У���ȡ��߶Ȳ�֧��#mid����
*******************************************************************************/
#define R_ATTR_POS_LEFTEXP    "([0-9heigtwdm\\+\\-\\*\\(\\)/#]*)"
#define R_ATTR_POS_TOPEXP     R_ATTR_POS_LEFTEXP
#define R_ATTR_POS_WIDTHEXP   "([0-9heigtwd\\+\\-\\*\\(\\)/#]*)"
#define R_ATTR_POS_HEIGHTEXP  R_ATTR_POS_WIDTHEXP
							  
#define R_CHECK_POS           "(" R_ATTR_POS_LEFTEXP "," R_ATTR_POS_TOPEXP "," R_ATTR_POS_WIDTHEXP "," R_ATTR_POS_HEIGHTEXP ")*"

/*******************************************************************************
*���� 7 ���꾫����ȷ���leftexp\topexp\widthexp\heightexp���ʽ�ĺϷ���
*�����Ƿ�ƴд����
*pos���ʽ�У���ȡ��߶Ȳ�֧��#mid����
*******************************************************************************/
#define R_MATH_EXP            "[0-9\\+\\-\\*\\(\\)/]*" 
#define R_ATTR_LEFTEXP_CMD    "((#mid)*(#height)*(#width)*)*"
#define R_ATTR_WIDTHEXP_CMD   "((#height)*(#width)*)*"

#define R_CHECK_LEFTEXP		  "(" R_MATH_EXP R_ATTR_LEFTEXP_CMD R_MATH_EXP ")*"
#define R_CHECK_TOPEXP		  R_CHECK_LEFTEXP
#define R_CHECK_WIDTHEXP	  "(" R_MATH_EXP R_ATTR_WIDTHEXP_CMD R_MATH_EXP ")*"
#define R_CHECK_HEIGHTEXP     R_CHECK_WIDTHEXP
/***************************************
����UIԪ�صĻ���
****************************************/
class UIBase
{
public:
	//UIBase��������Ҫ��һ���޲ι��죬�������ʼ��m_attrMap �� m_eventMap
	//(��дInitAttrMap��InitEventMap)�˴�Ӧ������ʱǿ�ƣ���δ�뵽�õķ���
											UIBase();
	//ʹ��XML�ڵ��ʼ��һ��UI����
	virtual bool							Init(const XMLElement* pElement);
	virtual bool							ParseSpecialCmd();
	shared_ptr<const string>				GetObjectID();
	shared_ptr<const string>				GetObjectName();
	bool									CheckAttrName(const string& sAttrName);
	bool									CheckEventName(const string& sEventName);
	bool									AddAttr(const string& sAttrName, const string& sAttrDefaultValue = "");
	bool									SetAttrValue(const string& sAttrName, const string& sAttrValue);
	shared_ptr<const string>				GetAttrValue(const string& sAttrName);
	bool									AddEvent(const string& sEventName, const string& sEventDefaultValue = "");
	bool									SetEventHandler(const string& sEventName, const string& sEventValue);
	bool									SetEventHandler(const XMLElement* pEventElement);
	shared_ptr<const string>				GetEventHandler(const string& sEventName);
	UIBase*									GetParent();
	bool									SetParent(UIBase* pParent);
	bool									AddChild(UIBase* pChild);
	UIBase*									GetChild(const string& sChildName);
	bool									RemoveChild(const string& sChildName);
protected:
	map<string, string>						m_attrMap;
	map<string, string>						m_eventMap;
	UIBase*									m_parentObj;
	map<string, UIBase*>					m_childrenMap;
	map<string, string>						m_attrCmdPatternMap;
	void									InitAttrMap();
	void									InitEventMap();
	void									InitAttrCmdParserMap();
};

class Test :public UIBase
{
public:
	Test() {
		InitAttrMap();
		return;
	};
	bool Init(const XMLElement* pElement){ return true; };
	void InitAttrMap() {
		ADD_ATTR("TestAttr", "test")
	};
};

class LayoutObject: public UIBase
{
public:
											LayoutObject();

protected:
	void									InitAttrMap();
	void									InitEventMap();
};

#endif