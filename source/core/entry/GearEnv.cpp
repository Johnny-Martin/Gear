#include "stdafx.h"
#include "GearEnv.h"

map<string, UIObject*> GearEnv::m_rootObjMap{};

HRESULT GearEnv::Init()
{
	RenderManager::Init();
	return S_OK;
}
HRESULT GearEnv::UnInit()
{
	RenderManager::UnInit();
	DestoryAllRootObject();
	return S_OK;
}
bool GearEnv::AddRootObject(UIObject* pObj)
{
	auto spObjName = pObj->GetAttrValue("id");
	if (!spObjName  || spObjName->empty()){
		ERR("AddRootObj error: can not get object id");
		return false;
	}
	auto it = m_rootObjMap.find(*spObjName);
	if (it != m_rootObjMap.end()){
		ERR("AddRootObj error: repetitive object id");
		return false;
	}
	m_rootObjMap[*spObjName] = pObj;
	return true;
}

void GearEnv::DestoryAllRootObject()
{
	auto it = m_rootObjMap.begin();
	while (it != m_rootObjMap.end())
	{
		delete it->second;
		m_rootObjMap.erase(it);
		it = m_rootObjMap.begin();
	}
}