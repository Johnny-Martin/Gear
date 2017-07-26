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

class LayoutObject
{
public:
	LayoutObject(){ InitAttrMap(); }
	bool SetAttr(string key, string value);
	bool GetAttr(string key, string* value);
	bool CheckAttrName(string strName);
	bool CheckEventName(string strName);
	void Init(const XMLElement* pElement);
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