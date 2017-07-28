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
											UIBase();
	//使用XML节点初始化一个UI对象
	virtual bool							Init(const XMLElement* pElement);
	bool									CheckAttrName(string strName);
	bool									CheckEventName(string strName);
protected:
	map<string, string>						m_attrMap;
	map<string, string>						m_eventMap;
private:
	void									InitAttrMap();
	void									InitEventMap();
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