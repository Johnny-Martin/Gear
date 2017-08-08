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

/***************************************
所有UI元素的基类
****************************************/
class UIBase
{
public:
	//UIBase的子类需要有一个无参构造，在里面初始化m_attrMap 和 m_eventMap
	//(重写InitAttrMap、InitEventMap)此处应作编译时强制，但未想到好的方案
											UIBase();
	//使用XML节点初始化一个UI对象
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
	void									InitAttrMap();
	void									InitEventMap();
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