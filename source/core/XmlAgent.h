/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.7.26
Description:		xml�����������xml�����ɶ����б�
*****************************************************/
#pragma once
#include "tinyxml2.h"
#include "UIError.h"

using namespace tinyxml2;

namespace Gear {
	namespace Xml {

class XmlAgent {
public:
	virtual XMLERROR BuildObjectTree(const XMLElement* pElement);
	virtual XMLERROR CheckXmlNode(const XMLElement* pElement);
};



	}//namespace Xml
}//namespace Gear