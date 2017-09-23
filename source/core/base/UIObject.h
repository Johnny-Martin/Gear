#pragma once
#include "../stdafx.h"
#include "../utils/Micros.h"
#include "XmlUIElement.h"
#include "../entry/RenderManager.h"

using namespace std;
using namespace tinyxml2;

namespace Gear {
	namespace UI {

	}//namespace UI
}//namespace Gear

struct UIPos
{
	UIPos();
	int			 left;
	int			 top;
	unsigned int width;
	unsigned int height;
	RECT GetWndRECT() const;
};
class UIEvent
{
public:
	//UIEvent(const string& sLableAttr_Func_In_XML);
	bool										SetEventHandlerFilePath(const string& sPath);
	bool										SetEventHandlerFuncName(const string& sName);
	shared_ptr<const string>					GetEventHandlerFilePath();
	shared_ptr<const string>					GetEventHandlerFuncName();
	bool										Fire();
	bool										InvokeLuaHandler();
private:
	string m_filePath;							//������¼���lua���ļ�·��
	string m_funcName;							//������¼���lua�ĺ�����
};

typedef  pair<string, UIObject*> PAIR;
class CmpByZorder {
public:
	inline bool operator()(const PAIR& k1, const PAIR& k2);
};
/***************************************
����UIԪ�صĻ���
****************************************/

class UIObject:public XmlUIElement, public RenderTarget
{
	friend RenderTarget;
public:
	//UIBase��������Ҫ��һ���޲ι��죬�������ʼ��m_attrMap �� m_eventMap
	//(��дInitAttrMap��InitEventMap)�˴�Ӧ������ʱǿ�ƣ���δ�뵽�õķ���
												UIObject();
	virtual										~UIObject();
	//ʹ��XML�ڵ��ʼ��һ��UI����
	virtual bool								InitImpl(const XMLElement* pElement);
	bool										CheckEventName(const string& sEventName);
	bool										AddEvent(const string& sEventName, UIEvent* pEventObj = nullptr);
	bool										SetEventHandler(const string& sEventName, const string& sFuncName, const string& sFilePath);
	bool										SetEventHandler(const XMLElement* pEventElement);
	shared_ptr<UIEvent*>						GetEventHandler(const string& sEventName);
	UIObject*									GetParent();
	bool										SetParent(UIObject* pParent);
	bool										AddChild(UIObject* pChild, const string& sChildID = "");
	UIObject*									GetChild(const string& sChildName);
	bool										RemoveChild(const string& sChildName, bool bDestoryChild = true, UIObject** ppChildObj = nullptr);
	bool										CalcPosFromExp();
	const UIPos									GetPosObject();
	void										SortChildrenByZorder();
	UIPos										GetWndCoordinatePos();
#ifdef USE_D2D_RENDER_MODE
public:
	virtual HRESULT								OnDrawImpl(ID2D1RenderTarget* pRenderTarget, const RECT& rcInvalid) = 0;
protected:
	virtual HRESULT								CreateDeviceDependentResources(ID2D1RenderTarget* pRenderTarget) = 0;
	virtual HRESULT								DiscardDeviceDependentResources() = 0;
#else
public:
	virtual HRESULT								OnDrawImpl(Graphics& graphics, const RECT& rcInvalid) = 0;
#endif
protected:
	map<string, UIEvent*>						m_eventMap;//second��Ա�����event����
	UIObject*									m_parentObj;
	map<string, UIObject*>						m_childrenMap;
	vector<PAIR>*								m_pVecChildrenPair;
	UIPos										m_pos;
	bool										m_bWndObj;
	void										InitAttrMap();
	void										InitEventMap();
	void										InitAttrValuePatternMap();
	void										InitAttrValueParserMap();
};

class Test :public UIObject
{
public:
	Test() {
		InitAttrMap();
		return;
	};
	bool Init(const XMLElement* pElement){ return true; };
	void InitAttrMap() {
		ADD_ATTR("TestAttr", "test")
	};
};

class LayoutObject: public UIObject
{
public:
												LayoutObject();
#ifdef USE_D2D_RENDER_MODE
public:
	virtual HRESULT								OnDrawImpl(ID2D1RenderTarget* pRenderTarget, const RECT& rcInvalid);
protected:
	virtual HRESULT								CreateDeviceDependentResources(ID2D1RenderTarget* pRenderTarget);
	virtual HRESULT								DiscardDeviceDependentResources();
#else
	virtual HRESULT								OnDrawImpl(Graphics& graphics, const RECT& rcInvalid);
#endif
protected:
	void										InitAttrMap();
	void										InitEventMap();
};