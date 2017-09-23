/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.7.26
Description:		xml�����������xml�����ɶ����б�
*****************************************************/
#pragma once
#include "../stdafx.h"
#include "tinyxml2.h"
#include "../utils/UIError.h"
#include "../base/UIObject.h"
#include <string>

using namespace tinyxml2;
using namespace std;;

namespace Gear {
	namespace Xml {

class XmlAgent {
public:
	static XmlAgent&					GetInstance();
	virtual XMLERROR					CreateObject(const XMLElement* pElement);
	
	//��XML�����﷨��顢�����飬û����󣬷��ظ�����
	virtual XMLERROR					GetXmlRootElement(const string& sFilePath);
protected:
	virtual XMLERROR					CheckXmlElement(const XMLElement* pElement);
	UIObject*							CreateUIObject(const XMLElement* pElement);

	//InitUIObject(UIBase* obj, XMLElement* element)
	//CreateUiObject
	//CheckElementAttrName
	//CheckElementEventName
	//

};



	}//namespace Xml
}//namespace Gear