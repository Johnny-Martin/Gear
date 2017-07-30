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
	bool									CheckAttrName(const string& strName);
	bool									CheckEventName(const string& strName);
	bool									AddAttrName(const string& strName, const string& strDefaultValue = "");
	bool									SetAttrValue(const string& strName, const string& strValue);
	const string&							GetAttrValue(const string& strName);
	bool									AddEventName(const string& strName, const string& strDefaultValue = "");
	bool									SetEventHandler(const string& strName, const string& strValue);
	const string&							GetEventHandler(const string& strName);
protected:
	map<string, string>						m_attrMap;
	map<string, string>						m_eventMap;
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

class LayoutObject
{
public:
											LayoutObject(){ InitAttrMap(); }
	bool									SetAttr(string key, string value);
	bool									GetAttr(string key, string* value);
	bool									CheckAttrName(string strName);
	bool									CheckEventName(string strName);
private:
	//static set<string> InitAttrNameSet();
	static set<string> InitEventNameSet();
	static set<string> m_eventNameSet;

	bool InitAttrMap();
	//bool InitEventMap();
	map<string, string> m_attrMap;
	map<string, string> m_eventMap;
	vector<shared_ptr<LayoutObject>> m_children;
};

#endif