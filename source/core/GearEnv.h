/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.9.22
Description:		环境初始化、根对象生命周期管理
*****************************************************/
#pragma once
#include "stdafx.h"
#include "base/UIObject.h"

class GearEnv
{
public:
	static HRESULT							Init();
	static HRESULT							UnInit();
	static bool								AddRootObject(UIObject* pObj);
private:
	static void								DestoryAllRootObject();
	static map<string, UIObject*>			m_rootObjMap;
};