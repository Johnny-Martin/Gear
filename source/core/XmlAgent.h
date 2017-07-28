/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.7.26
Description:		xml�����������xml�����ɶ����б�
*****************************************************/
#pragma once
#include "stdafx.h"
#include "tinyxml2.h"
#include "UIError.h"
#include <string>

using namespace tinyxml2;
using namespace std;;

namespace Gear {
	namespace Xml {

class XmlAgent {
public:
	XmlAgent&							GetInstance();
	virtual XMLERROR					CreateObject(const XMLElement* pElement);
	
	//��XML�����﷨��顢�����飬û����󣬷��ظ�����
	virtual XMLERROR					GetXmlRootElement(const string& sFilePath);
protected:
	virtual XMLERROR					CheckXmlNode(const XMLElement* pElement);
};



	}//namespace Xml
}//namespace Gear