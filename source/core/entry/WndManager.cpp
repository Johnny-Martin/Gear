#include "stdafx.h"
#include "WndManager.h"
#include "../metaobject/UIWindow.h"

vector<UIWindow*> WndManager::m_allWndVec;
WndManager::WndManager() {}

WndManager& WndManager::GetInstance()
{
	static WndManager wndMgr{};
	return wndMgr;
}
void WndManager::AddWindow(UIWindow* pUIWnd)
{
	auto it = std::find(m_allWndVec.begin(), m_allWndVec.end(), pUIWnd);
	if (it != m_allWndVec.end()) {
		WARN("AddWindow warning: window handle already exisits in WndManager, hWnd: {}", (int)pUIWnd->m_hWnd);
		return;
	}
	m_allWndVec.push_back(pUIWnd);
}
void WndManager::RemoveWindow(UIWindow* pUIWnd)
{
	auto it = std::find(m_allWndVec.begin(), m_allWndVec.end(), pUIWnd);
	if (it == m_allWndVec.end()) {
		WARN("RemoveWindow warning: window handle not found in WndManager, hWnd: {}", (int)pUIWnd->m_hWnd);
		return;
	}
	m_allWndVec.erase(it);
}
void WndManager::RePaintAllWnd()
{
	for (auto it = m_allWndVec.begin(); it != m_allWndVec.end(); ++it) {
		(*it)->InvalidateRect(NULL);
	}
}
void WndManager::OnTryExit()
{
	if (m_allWndVec.size() == 0)
		return;

	auto it = m_allWndVec.begin();
	while (it != m_allWndVec.end()) {
		::DestroyWindow((*it)->m_hWnd);
#ifdef DEBUG
		auto next = m_allWndVec.begin();
		if (next != m_allWndVec.end()) {
			//必须在OnDestroy中Remove掉自己的窗口
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
