#include "stdafx.h"
#include "XmlAgent.h"
#include "tinyxml2.h"
#include "UIFactory.h"
#include "UIObject.h"

using namespace std;
using namespace Gear::Xml;

XmlAgent& XmlAgent::GetInstance()
{
	static XmlAgent singleton{};
	return singleton;
}
XMLERROR XmlAgent::CreateObject(const XMLElement* pElement)
{
	return XML_SUCCESS;
}
UIBase*	XmlAgent::CreateUIObject(const XMLElement* pElement)
{
	string sClassName = pElement->Value();
	CObjectFactory& factory = CObjectFactory::GetInstance();
	return (UIBase*)factory.CreateObjectByClassName(sClassName);
}
XMLERROR XmlAgent::CheckXmlElement(const XMLElement* pElement)
{
#ifdef DEBUG
	if (pElement == nullptr) return XML_SUCCESS;
	UIBase* pObj = CreateUIObject(pElement);
	if (pObj == nullptr)
		return XML_ERROR_ILLEGAL_LABLE;

	const XMLAttribute* pAttr = pElement->FirstAttribute();
	while (pAttr)
	{
		string attrName = pAttr->Name();
		if (!pObj->CheckAttrName(attrName))
		{
			return XML_ERROR_ILLEGAL_ATTRIBUTE;
		}
		pAttr = pAttr->Next();
	}
#endif // DEBUG
	return XML_SUCCESS;
}
XMLERROR XmlAgent::GetXmlRootElement(const std::string& sFilePath)
{
	LOG->trace("GetXmlRootElement wsFilePath:  ");
	LOG->trace(sFilePath.c_str());
	tinyxml2::XMLDocument doc;
	doc.LoadFile(sFilePath.c_str());

	if (doc.Error()) {
		LOG->trace("Load XML file Error: ");
		return XML_TINYXML2_ERROR;
	}
	//强制使用无bom格式
	if (doc.HasBOM())
		return XML_ERROR_ILLEGAL_FILE_ENCODE;

	tinyxml2::XMLElement* element = doc.RootElement();
	while (element) {
		XMLERROR checkRet = CheckXmlElement(element);
		if (checkRet != XML_SUCCESS) return checkRet;
		element = element->NextSiblingElement();
	}

	return XML_SUCCESS;
}