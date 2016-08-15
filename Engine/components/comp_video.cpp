#include "mcv_platform.h"
#include "comp_video.h"

bool TCompVideo::load(MKeyValue & atts)
{
	file = atts.getString("file", "");
	return true;
}

void TCompVideo::update(float dt) {
	if (file == "") { return; }
	BSTR * status = new BSTR;
	m_spWMPPlayer->get_status(status);
}

void TCompVideo::init() {
	if (file == "") { return; }
	AtlAxWinInit();
	CComPtr<IAxWinHostWindow>  spHost;
	HRESULT  hr;

	HWND hwnd = CApp::get().getHWnd();
	LPRECT * rcClient = new LPRECT;
	GetClientRect(hwnd, *rcClient);
	m_wndView.Create(hwnd, *rcClient, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);

	hr = m_wndView.QueryHost(&spHost);

	hr = spHost->CreateControl(CComBSTR(_T(file.c_str())), m_wndView, 0);

	hr = m_wndView.QueryControl(&m_spWMPPlayer);
}