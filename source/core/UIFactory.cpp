
#include "stdafx.h"
#include "UIFactory.h"

unsigned int CObjectFactory::m_initialClassInfoArraySize = 0;

CObjectFactory::CObjectFactory() 
{
	const _ClassInfoMapType::value_type* initValueRet = CObjectFactory::_GetInitValue();

	unsigned int y = sizeof(_ClassInfoMapType::value_type);
	//'initValueRet' is a pointer type rather than an array type,so 
	//initValueLen = sizeof(initValueRet) is definitely wrong
	unsigned int initValueLen = m_initialClassInfoArraySize / y;

	//Initialize the m_classInfoMap with a memory address and offset,
	//Compiler do not allowed us initialize a member variable in class declaration,
	//So the REFLECTION_DECLARE_BEGIN micro construct a private member function and return
	//a local static array which can be initialized in compile-time in class declaration.
	m_classInfoMap = new _ClassInfoMapType(initValueRet, initValueRet + initValueLen);

}
void* CObjectFactory::CreateObject(string strClassName) 
{
	_ClassInfoMapType::const_iterator iter = m_classInfoMap->find(strClassName);

	if (m_classInfoMap->end() == iter)
		return nullptr;
	else
		return iter->second();
}
void* CObjectFactory::CreateObject(const XMLElement* pElement)
{
	auto className = pElement->Value();
	return CreateObject(className);
}
void CObjectFactory::RegistClass(string strClassName, CreateObjectCallBack callBackFun) 
{
	m_classInfoMap->insert(std::pair<string, CreateObjectCallBack>(strClassName, callBackFun));
}
CObjectFactory& CObjectFactory::GetInstance()
{
	static CObjectFactory m_factoryInstance{};
	return m_factoryInstance;
}