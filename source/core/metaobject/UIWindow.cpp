#include "stdafx.h"
#include "UIWindow.h"

vector<HWND> WndManager::m_allWndVec;
WndManager::WndManager() {}

WndManager& WndManager::GetInstance()
{
	static WndManager wndMgr{};
	return wndMgr;
}
void WndManager::AddWindow(HWND hWnd)
{
	auto it = std::find(m_allWndVec.begin(), m_allWndVec.end(), hWnd);
	if (it != m_allWndVec.end()) {
		WARN("AddWindow warning: window handle already exisits in WndManager, hWnd: {}", (int)hWnd);
		return;
	}
	m_allWndVec.push_back(hWnd);
}
void WndManager::RemoveWindow(HWND hWnd)
{
	auto it = std::find(m_allWndVec.begin(), m_allWndVec.end(), hWnd);
	if (it == m_allWndVec.end()) {
		WARN("RemoveWindow warning: window handle not found in WndManager, hWnd: {}", (int)hWnd);
		return;
	}
	m_allWndVec.erase(it);
}
void WndManager::OnTryExit()
{
	if (m_allWndVec.size() == 0)
		return;

	auto it = m_allWndVec.begin();
	while (it != m_allWndVec.end()) {
		::DestroyWindow(*it);
#ifdef DEBUG
		auto next = m_allWndVec.begin();
		if (next != m_allWndVec.end()) {
			//必须在OnDestroy中Remove调自己的窗口
			ATLASSERT(next != it && *next != *it);
		}
#endif // DEBUG

		it = m_allWndVec.begin();
	}
}
WndManager::~WndManager()
{
	OnTryExit();
}
UIWindow::UIWindow() :m_hWndParent(0)
{
	InitAttrMap();
	InitEventMap();
	InitAttrValuePatternMap();
	InitAttrValueParserMap();

	m_bWndObj = true;
#ifdef USE_D2D_RENDER_MODE
	m_pHwndRenderTarget = nullptr;
#endif
}
bool UIWindow::InitImpl(const XMLElement* pElement)
{
	if (!UIObject::InitImpl(pElement)) { return false; }

	//额外校验、处理一些属性值
	unsigned int minwidth	= atoi(m_attrMap["minwidth"].c_str());
	unsigned int minheight	= atoi(m_attrMap["minheight"].c_str());
	unsigned int maxwidth	= atoi(m_attrMap["minwidth"].c_str());
	unsigned int maxheight	= atoi(m_attrMap["minwidth"].c_str());

	if (m_pos.width < minwidth){
		WARN("UIWindow Init warning: window width:{} is less than minwidth: {}", m_pos.width, minwidth);
		m_pos.width = minwidth;
	}
	if (m_pos.height < minheight) {
		WARN("UIWindow Init warning: window height:{} is less than minheight: {}", m_pos.height, minheight);
		m_pos.height = minheight;
	}
	if (maxwidth != 0 && m_pos.width > maxwidth){
		WARN("UIWindow Init warning: window width:{} is larger than maxwidth: {}", m_pos.width, maxwidth);
		m_pos.width = maxwidth;
	}
	if (maxheight != 0 && m_pos.height > maxheight) {
		WARN("UIWindow Init warning: window width:{} is larger than maxheight: {}", m_pos.height, maxheight);
		m_pos.height = maxheight;
	}
	//return true;
	//--//这里不能直接把xml里所有的<window>标签都一下子创建出来
	return CreateUIWindow();
}
void UIWindow::InitAttrMap()
{
	ADD_ATTR("title",		"")
	ADD_ATTR("layered",		"0")	
	ADD_ATTR("topmost",		"0")	
	ADD_ATTR("caption",		"30")
	ADD_ATTR("min",			"0")
	ADD_ATTR("max",			"0")
	ADD_ATTR("blur",		"0")
	ADD_ATTR("show",		"1")
	ADD_ATTR("shadow",		"0,0,0,0")
	ADD_ATTR("border",		"0,0,0,0")
	ADD_ATTR("resize",		"0,0,0,0")

	//shadow属性"int,int,int,int"依次对应以下四个值
	ADD_ATTR("leftshadow",	"0")
	ADD_ATTR("topshadow",	"0")
	ADD_ATTR("rightshadow",	"0")
	ADD_ATTR("bottomshadow","0")

	//border属性"int,int,int,int"依次对应以下四个值
	ADD_ATTR("leftborder",	"0")
	ADD_ATTR("topborder",	"0")
	ADD_ATTR("rightborder", "0")
	ADD_ATTR("bottomborder","0")

	//resize属性"int,int,int,int"依次对应以下四个值
	ADD_ATTR("minwidth",	"0")
	ADD_ATTR("minheight",	"0")
	ADD_ATTR("maxwidth",	"0")
	ADD_ATTR("maxheight",	"0")
}

void UIWindow::InitEventMap()
{
	ADD_EVENT("OnShowWindow",  nullptr)
	ADD_EVENT("OnNcActivate",  nullptr)
	ADD_EVENT("OnFocusChange", nullptr)
}

void UIWindow::InitAttrValuePatternMap()
{
	ADD_ATTR_PATTERN("layered",		R_CHECK_BOOL)
	ADD_ATTR_PATTERN("topmost",		R_CHECK_BOOL)
	ADD_ATTR_PATTERN("caption",		R_CHECK_INT)
	ADD_ATTR_PATTERN("min",			R_CHECK_BOOL)
	ADD_ATTR_PATTERN("max",			R_CHECK_BOOL)
	ADD_ATTR_PATTERN("blur",		R_CHECK_BOOL)
	ADD_ATTR_PATTERN("show",		R_CHECK_INT)
	ADD_ATTR_PATTERN("shadow",		R_CHECK_WINDOW_ATTR)
	ADD_ATTR_PATTERN("border",		R_CHECK_WINDOW_ATTR)
	ADD_ATTR_PATTERN("resize",		R_CHECK_WINDOW_ATTR)
}

void UIWindow::InitAttrValueParserMap()
{
	auto EraseSpace = [](string& str) {
		for (string::iterator it = str.end(); it != str.begin();) {
			--it;
			if ((*it) == ' ' || (*it) == '\t' || (*it) == '\n' || (*it) == '\r') {
				str.erase(it);
			}
		}
	};

	auto ParseFourValue = [&](const string& sAttrName)->bool {
		auto sPattern = m_attrValuePatternMap[sAttrName];
		auto fourValueExp = m_attrMap[sAttrName];

		regex pattern(sPattern.c_str());
		string firValue		= regex_replace(fourValueExp, pattern, string("$2"));
		string secValue		= regex_replace(fourValueExp, pattern, string("$3"));
		string thirValue	= regex_replace(fourValueExp, pattern, string("$4"));
		string fourthValue  = regex_replace(fourValueExp, pattern, string("$5"));

		//去除表达式中的空白符
		EraseSpace(firValue);
		EraseSpace(secValue);
		EraseSpace(thirValue);
		EraseSpace(fourthValue);

		if (sAttrName == "shadow"){
			SetAttrValue("leftshadow",	firValue);
			SetAttrValue("topshadow",	secValue);
			SetAttrValue("rightshadow", thirValue);
			SetAttrValue("bottomshadow",fourthValue);
		} else if (sAttrName == "border"){
			SetAttrValue("leftborder",	firValue);
			SetAttrValue("topborder",	secValue);
			SetAttrValue("rightborder", thirValue);
			SetAttrValue("bottomborder",fourthValue);
		} else if (sAttrName == "resize"){
			SetAttrValue("minwidth",	firValue);
			SetAttrValue("minheight",	secValue);
			SetAttrValue("maxwidth",	thirValue);
			SetAttrValue("maxheight",	fourthValue);
		} else{
			ERR("fatal error: InitAttrValueParserMap error, unexpected attribute, name: {}, value: {}", sAttrName, fourValueExp);
			return false;
		}
		return true;
	};


	ADD_ATTR_PARSER("shadow", ParseFourValue)
	ADD_ATTR_PARSER("border", ParseFourValue)
	ADD_ATTR_PARSER("resize", ParseFourValue)
}

bool UIWindow::CreateUIWindow()
{
	auto GetWndStyle = [&]() {
		DWORD dWndStyle = WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_SYSMENU;
		if (m_attrMap["min"] != "0") {
			dWndStyle = dWndStyle | WS_MINIMIZEBOX;
		}
		if (m_attrMap["max"] != "0") {
			dWndStyle = dWndStyle | WS_MAXIMIZEBOX;
		}
		if (m_attrMap["visible"] != "0") {
			dWndStyle = dWndStyle | WS_VISIBLE;
		}
		return dWndStyle;
	};
	auto GetExWndStyle = [&]() {
		DWORD dExWndStyle = 0;
		if (m_attrMap["topmost"] != "0") {
			dExWndStyle = WS_EX_TOPMOST;
		}
		return dExWndStyle;
	};

	auto GetWndRect = [&]() {
		RECT rc;
		rc.left		= m_pos.left;
		rc.top		= m_pos.top;
		rc.right	= rc.left + m_pos.width;
		rc.bottom	= rc.top + m_pos.height;

		INFO("CreateUIWindow Info: rc.left:{}, rc.top:{}, rc.right:{}, rc.bottom:{}", rc.left, rc.top, rc.right, rc.bottom);
		return rc;
	};
	//发事件给脚本，处理窗口位置、parent\owner等关系
	//FireLuaEvent();
	wstring wstrWndName = UTF8AToUnicodeW(m_attrMap["title"]);
	Create(m_hWndParent, GetWndRect(), wstrWndName.c_str(), GetWndStyle(), GetExWndStyle());

	ATLASSERT(m_hWnd);
	ShowWindow(atoi(m_attrMap["show"].c_str()));
	
	WndManager::GetInstance().AddWindow(m_hWnd);
	return m_hWnd != NULL;
}
BOOL UIWindow::PreTranslateMessage(MSG* pMsg)
{
	return FALSE;
}
LRESULT UIWindow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);

	bHandled = false;
	return 0;
}
LRESULT UIWindow::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = false;
	return 0;
}

LRESULT UIWindow::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HRESULT hr = S_OK;
#ifdef USE_D2D_RENDER_MODE
	hr = CreateDeviceDependentResources(m_pHwndRenderTarget);
	if(SUCCEEDED(hr)){
		m_pHwndRenderTarget->BeginDraw();
		//获取无效矩形区域
		//m_pHwndRenderTarget->GetEffectInvalidRectangles();
		//通知与无效矩形有交集的、可见的（visible=1并且未被完全遮挡）孩子
		m_pHwndRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		m_pHwndRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

		RECT rcInvalid;
		auto size = m_pHwndRenderTarget->GetPixelSize();
		rcInvalid.left = 0;
		rcInvalid.top = 0;
		rcInvalid.right = size.width + rcInvalid.left;
		rcInvalid.bottom = size.height + rcInvalid.top;
		for (auto it=m_pVecChildrenPair->begin(); it!= m_pVecChildrenPair->end();++it)
		{
			it->second->Draw(m_pHwndRenderTarget, rcInvalid, it->second);
		}
		//draw all children in invalid rentangle?
		hr = m_pHwndRenderTarget->EndDraw();
		ValidateRect(NULL);
	}
	if (hr == D2DERR_RECREATE_TARGET)
	{
		hr = S_OK;
		DiscardDeviceDependentResources();
		//通知所有孩子，丢弃设备相关资源
	}
#else
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(&ps);
	Graphics graphics(hdc);
	for (auto it=m_pVecChildrenPair->begin(); it!=m_pVecChildrenPair->end(); ++it)
	{
		it->second->Draw(graphics, ps.rcPaint, it->second);
	}
	EndPaint(&ps);
#endif
	bHandled = false;
	return hr;
}

LRESULT UIWindow::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//要想绘制非客户区，必须使用GDI而不能使用GDI+
	bHandled = false;
	return 0;
}

LRESULT UIWindow::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT UIWindow::OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

//当所有的UI窗口都（filterCount==0）被关闭后，进程会退出.
//假如脚本想要退出进程，但此时尚有多个窗口存在，需调用WndManager::OnTryExit
//依次销毁所有的窗口、然后退出进程
LRESULT UIWindow::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);

	WndManager::GetInstance().RemoveWindow(m_hWnd);
	//FireLuaEvent()
	//模仿脚本中退出
	//WndManager::GetInstance().OnTryExit();

	//窗口即将销毁，
	//--//m_hWnd = NULL;
	auto filterCount = pLoop->m_aMsgFilter.GetSize();
	if (filterCount == 0)
	{
		PostQuitMessage(0);
	}

	bHandled = FALSE;
	return 0;
}
///////////////////////////////////////Direct2D渲染模式相关代码///////////////////////////////////
#ifdef USE_D2D_RENDER_MODE
HRESULT	UIWindow::OnDrawImpl(ID2D1RenderTarget* pRenderTarget, const D2D1_RECT_F& rcWndPos, const RECT* rcInvalidPtr/* = nullptr*/)
{
	return S_OK;
}
HRESULT	UIWindow::CreateDeviceDependentResources(ID2D1RenderTarget* pRenderTarget)
{
	if (m_pHwndRenderTarget) return S_OK;
	RECT rc;
	//GetClientRect(m_hwnd, &rc);
	GetWindowRect(&rc);

	D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left,rc.bottom - rc.top);
	HRESULT hr = RenderManager::m_pD2DFactory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(m_hWnd, size),
		&m_pHwndRenderTarget);

	ATLASSERT(SUCCEEDED(hr));
	return hr;
}
HRESULT	UIWindow::DiscardDeviceDependentResources()
{
	SafeRelease(&m_pHwndRenderTarget);
	return S_OK;
}
/////////////////////////////////////////GDI+渲染模式相关代码/////////////////////////////////////
#else
HRESULT	UIWindow::OnDrawImpl(Graphics& graphics, const UIPos& rcWndPos)
{
	HRESULT hr = S_OK;

	return hr;
}

#endif