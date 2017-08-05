#include "stdafx.h"
#include "XmlAgent.h"
#include "tinyxml2.h"
#include "UIFactory.h"
#include "UIObject.h"
#include "Log.h"

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
	return (UIBase*)factory.CreateObject(sClassName);
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
	INFO("GetXmlRootElement wsFilePath:  {}", sFilePath);
	tinyxml2::XMLDocument doc;
	doc.LoadFile(sFilePath.c_str());

	if (doc.Error()) {
		INFO("Load XML file Error: ");
		return XML_TINYXML2_ERROR;
	}
	//强制使用无bom格式
	if (doc.HasBOM())
		return XML_ERROR_ILLEGAL_FILE_ENCODE;

	tinyxml2::XMLElement* pElement = doc.RootElement();
	while (pElement) {
		//XMLERROR checkRet = CheckXmlElement(element);
		//if (checkRet != XML_SUCCESS) return checkRet;
		auto pObj = CREATE(UIBase, pElement);
		if (!pObj || !pObj->Init(pElement))
			INFO("create/initicalize UI ojject error: pObj：{}", int(pObj));
		
		pElement = pElement->NextSiblingElement();
	}

	return XML_SUCCESS;
}