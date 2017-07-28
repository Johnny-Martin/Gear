#include "stdafx.h"
#include "XmlAgent.h"
#include "tinyxml2.h"

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
XMLERROR XmlAgent::CheckXmlNode(const XMLElement* pElement)
{
	return XML_SUCCESS;
}
XMLERROR XmlAgent::GetXmlRootElement(const std::string& sFilePath)
{
	LOG->trace("GetXmlRootElement wsFilePath:  ");
	tinyxml2::XMLDocument doc;
	doc.LoadFile(sFilePath.c_str());

	if (doc.Error()) {
		LOG->error("Load XML file Error: ", doc.ErrorID());
		return XML_TINYXML2_ERROR;
	}

	//强制XML文件使用UTF8无BOM编码
	if (doc.HasBOM())
		return XML_ERROR_ILLEGAL_FILE_ENCODE;


	return XML_SUCCESS;
}