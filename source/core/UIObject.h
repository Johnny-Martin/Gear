#pragma once
#ifndef UIOBJECT_H
#define UIOBJECT_H
#include "stdafx.h"
#include "RenderManager.h"
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
		m_eventMap.insert(pair<string, UIEvent*>(attrName, defaultValue));

#define ADD_ATTR_PATTERN(attrName, sPattern)	\
		m_attrValuePatternMap.insert(pair<string, string>(attrName, sPattern));

#define ADD_ATTR_PARSER(attrName, sParser)	\
		m_attrValueParserMap.insert(pair<string, function<bool(const string&)> >(attrName, sParser));

/*******************************************************************************
*���� 5 �������ڴ��Լ��pos���ʽ�ĺϷ���
*�Ƿ���4��
*�Ƿ��д�����ַ�
*pos���ʽ�У���ȡ��߶Ȳ�֧��#mid����
*******************************************************************************/
#define R_ATTR_POS_LEFTEXP    "([0-9heigtwdm\\+\\-\\*\\(\\)/#\\\s*]*)"
#define R_ATTR_POS_TOPEXP     R_ATTR_POS_LEFTEXP
#define R_ATTR_POS_WIDTHEXP   "([0-9heigtwd\\+\\-\\*\\(\\)/#\\\s*]*)"
#define R_ATTR_POS_HEIGHTEXP  R_ATTR_POS_WIDTHEXP
							  
#define R_CHECK_POS           "(" R_ATTR_POS_LEFTEXP "," R_ATTR_POS_TOPEXP "," R_ATTR_POS_WIDTHEXP "," R_ATTR_POS_HEIGHTEXP ")*"

/*******************************************************************************
*���� 7 ���꾫����ȷ���leftexp\topexp\widthexp\heightexp���ʽ�ĺϷ���
*�����Ƿ�ƴд�����Ƿ��пո�/�س�
*pos���ʽ�У���ȡ��߶Ȳ�֧��#mid����
*******************************************************************************/
#define R_MATH_EXP            "[0-9\\+\\-\\*\\(\\)/]*" 
#define R_ATTR_LEFTEXP_CMD    "((#mid)*(#height)*(#width)*)*"
#define R_ATTR_WIDTHEXP_CMD   "((#height)*(#width)*)*"

#define R_CHECK_LEFTEXP		  "(" R_MATH_EXP R_ATTR_LEFTEXP_CMD R_MATH_EXP ")*"
#define R_CHECK_TOPEXP		  R_CHECK_LEFTEXP
#define R_CHECK_WIDTHEXP	  "(" R_MATH_EXP R_ATTR_WIDTHEXP_CMD R_MATH_EXP ")*"
#define R_CHECK_HEIGHTEXP     R_CHECK_WIDTHEXP
#define R_CHECK_EVENT_FUNC    "([^=>]*)(=>)?([^=>]*)"

/*******************************************************************************
*���ID�������͡������������ֵ
*******************************************************************************/
#define R_CHECK_ID			  "[1-9A-Za-z_\\.]+"
#define R_CHECK_BOOL		  "[01]"
#define R_CHECK_INT			  "[0-9]+"

/*******************************************************************************
*��鴰�ڵ�shadow��border��resize���Ķ��������
*******************************************************************************/
#define R_CHECK_WINDOW_ATTR	  "(([0-9\\s*]*),([0-9\\s*]*),([0-9\\s*]*),([0-9\\s*]*))"
struct UIPos
{
	UIPos() :left(0), top(0), width(0), height(0) {};
	int			 left;
	int			 top;
	unsigned int width;
	unsigned int height;
};
class UIEvent
{
public:
	//UIEvent(const string& sLableAttr_Func_In_XML);
	bool										SetEventHandlerFilePath(const string& sPath);
	bool										SetEventHandlerFuncName(const string& sName);
	shared_ptr<const string>					GetEventHandlerFilePath();
	shared_ptr<const string>					GetEventHandlerFuncName();
	bool										Fire();
	bool										InvokeLuaHandler();
private:
	string m_filePath;							//������¼���lua���ļ�·��
	string m_funcName;							//������¼���lua�ĺ�����
};
/***************************************
����UIԪ�صĻ���
****************************************/
class UIObject:public RenderTarget
{
public:
	//UIBase��������Ҫ��һ���޲ι��죬�������ʼ��m_attrMap �� m_eventMap
	//(��дInitAttrMap��InitEventMap)�˴�Ӧ������ʱǿ�ƣ���δ�뵽�õķ���
	UIObject();
	//ʹ��XML�ڵ��ʼ��һ��UI����
	virtual bool								Init(const XMLElement* pElement);
	virtual bool								ParseSpecialCmd();
	shared_ptr<const string>					GetObjectID();
	shared_ptr<const string>					GetObjectName();
	bool										CheckAttrName(const string& sAttrName);
	bool										CheckAttrValue(const string& sAttrName, const string& sAttrValue);
	bool										CheckEventName(const string& sEventName);
	bool										AddAttr(const string& sAttrName, const string& sAttrDefaultValue = "");
	bool										SetAttrValue(const string& sAttrName, const string& sAttrValue);
	shared_ptr<const string>					GetAttrValue(const string& sAttrName);
	bool										AddEvent(const string& sEventName, UIEvent* pEventObj = nullptr);
	bool										SetEventHandler(const string& sEventName, const string& sFuncName, const string& sFilePath);
	bool										SetEventHandler(const XMLElement* pEventElement);
	shared_ptr<UIEvent*>						GetEventHandler(const string& sEventName);
	UIObject*									GetParent();
	bool										SetParent(UIObject* pParent);
	bool										AddChild(UIObject* pChild, const string& sChildID = "");
	UIObject*									GetChild(const string& sChildName);
	bool										RemoveChild(const string& sChildName);
	bool										CalcPosFromExp();
	const UIPos									GetPosObject();

#ifdef USE_D2D_RENDER_MODE
public:
	virtual HRESULT								OnDrawImpl(ID2D1RenderTarget* pRenderTarget, const RECT& rcInvalid) = 0;
protected:
	virtual HRESULT								CreateDeviceDependentResources(ID2D1RenderTarget* pRenderTarget) = 0;
	virtual HRESULT								DiscardDeviceDependentResources() = 0;
#else
public:
	virtual HRESULT								OnDrawImpl(HDC* pHdc, const RECT& rcInvalid) = 0;
#endif
protected:
	map<string, string>							m_attrMap;
	map<string, UIEvent*>						m_eventMap;//second��Ա�����event����
	UIObject*									m_parentObj;
	map<string, UIObject*>						m_childrenMap;
	map<string, string>							m_attrValuePatternMap;
	map<string, function<bool(const string&)> >	m_attrValueParserMap;
	UIPos										m_pos;

	void										InitAttrMap();
	void										InitEventMap();
	void										InitAttrValuePatternMap();
	void										InitAttrValueParserMap();
};

class Test :public UIObject
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

class LayoutObject: public UIObject
{
public:
												LayoutObject();
#ifdef USE_D2D_RENDER_MODE
public:
	virtual HRESULT								OnDrawImpl(ID2D1RenderTarget* pRenderTarget, const RECT& rcInvalid);
protected:
	virtual HRESULT								CreateDeviceDependentResources(ID2D1RenderTarget* pRenderTarget);
	virtual HRESULT								DiscardDeviceDependentResources();
#else
	virtual HRESULT								OnDrawImpl(HDC* pHdc, const RECT& rcInvalid);
#endif
protected:
	void										InitAttrMap();
	void										InitEventMap();
};


#endif