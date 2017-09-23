/*****************************************************
Author:				Johnny-Martin
Email:				970803555@qq.com
Date:				2017.7.28
Description:		实现一个简单的C++反射
*****************************************************/

#pragma once
#include "stdafx.h"
#include "UIFrameWnd.h"
//#include "resource/UIResource.h"
#include "UIWindow.h"
#include "metaobject/UIRectangle.h"
#include "resource/ResManager.h"
#include "resource/ResColor.h"

using namespace std;
using namespace Gear::Res;

typedef void* (*CreateObjectCallBack)(void);
typedef std::map<string, CreateObjectCallBack> _ClassInfoMapType;

template <class T>
void* GreateObject()
{
	return (void*) new T();
}

#define REFLECTION_DECLARE_BEGIN()											\
	private:																\
	const static _ClassInfoMapType::value_type* WINAPI _GetInitValue() {	\
		static const _ClassInfoMapType::value_type initValue[] = {

#define REGISTER_CLASS(name, className)										\
	_ClassInfoMapType::value_type(name, GreateObject<className>),

#define REGISTER_CLASS2(className)											\
	_ClassInfoMapType::value_type(#className, GreateObject<className>),

#define REFLECTION_DECLARE_END()											\
	};m_initialClassInfoArraySize = sizeof(initValue);						\
	return initValue; }


//单例工厂
class CObjectFactory{
private:
												CObjectFactory();
public:
	void*										CreateObject(const string& strClassName);
	void*										CreateObject(const XMLElement* pElement);
	void										RegistClass(string strClassName, CreateObjectCallBack callBackFun);
	static CObjectFactory&						GetInstance();
private:
	_ClassInfoMapType*							m_classInfoMap;
	static unsigned int							m_initialClassInfoArraySize;

	REFLECTION_DECLARE_BEGIN()
		REGISTER_CLASS("window",	UIWindow)
		REGISTER_CLASS("layout",	LayoutObject)
		REGISTER_CLASS("bitmap",	UIBitmap)
		REGISTER_CLASS("rectangle", UIRectangle)
		//REGISTER_CLASS("color", ResColor)
	REFLECTION_DECLARE_END()
};

#define CREATE(classType, x) (classType)CObjectFactory::GetInstance().CreateObject(x)

template<class ObjectType>
inline ObjectType CreateUIObject(const XMLElement* element)
{
	return static_cast<ObjectType>(CObjectFactory::GetInstance().CreateObject(element));
}

template<class ObjectType>
inline ObjectType CreateUIObject(string sname)
{
	return static_cast<ObjectType>(CObjectFactory::GetInstance().CreateObject(sname));
}