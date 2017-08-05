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
	bool									CheckAttrName(const string& strName);
	bool									CheckEventName(const string& strName);
	bool									AddAttrName(const string& strName, const string& strDefaultValue = "");
	bool									SetAttrValue(const string& strName, const string& strValue);
	const string&							GetAttrValue(const string& strName);
	bool									AddEventName(const string& strName, const string& strDefaultValue = "");
	bool									SetEventHandler(const string& strName, const string& strValue);
	bool									SetEventHandler(const XMLElement* pEventElement);
	const string&							GetEventHandler(const string& strName);
protected:
	map<string, string>						m_attrMap;
	map<string, string>						m_eventMap;
	UIBase*									m_parentObj;
	vector<UIBase*>							m_childrenVec;
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