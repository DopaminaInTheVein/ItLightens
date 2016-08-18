#include "mcv_platform.h"
#include "comp_video.h"

bool TCompVideo::load(MKeyValue & atts)
{
	TCHAR szPath[MAX_PATH];
	if (!GetModuleFileName(NULL, szPath, MAX_PATH))
	{
		fatal("Cannot install service (%d)\n", GetLastError());
		return false;
	}

	file = std::string(szPath);
	size_t lastposfolder = file.find_last_of('\\');
	file = file.substr(0, lastposfolder + 1);
	file += atts.getString("file", "");
	return true;
}

void TCompVideo::ReleaseAll() {
	if (pWc)		pWc->Release();
	if (pConfig)	pConfig->Release();
	if (pVmr)		pVmr->Release();
	if (pSeek)		pSeek->Release();
	//if (pVidWin) {
	//	pVidWin->put_Visible(OAFALSE);
	//	pVidWin->put_Owner(NULL);
	//	pVidWin->Release();
	//}
	if (pEvent)		pEvent->Release();
	if (pControl)	pControl->Release();
	if (pGraph)		pGraph->Release();
	CoUninitialize();
	GameController->SetGameState(CGameController::RUNNING);
	CHandle h = CHandle(this).getOwner();
	h.destroy();
}

void TCompVideo::update(float dt) {
	if (!loaded) { return; }
	// Request the VMR to paint the video.
	CApp &appwnd = CApp::get();
	HWND hwndApp = appwnd.getHWnd();

	//HRESULT hr =
	pWc->RepaintVideo(hwndApp, hdc);
	//if (FAILED(hr)) { ReleaseAll(); return; }

	long timeout = dt * 1000;

	long evCode;
	pEvent->WaitForCompletion(timeout, &evCode);

	LONGLONG * pos = new LONGLONG, *duration = new LONGLONG;
	pSeek->GetCurrentPosition(pos);
	pSeek->GetDuration(duration);

	LONGLONG remaining = *duration - *pos;
	if (remaining < 10LL || controller->IsBackPressed()) {
		ReleaseAll();
	}
}

void TCompVideo::init() {
	if (file == "") { return; }

	CApp &appwnd = CApp::get();
	HWND hwndApp = appwnd.getHWnd();

	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr)) { ReleaseAll(); return; }

	hr = CoCreateInstance(CLSID_FilterGraphNoThread, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&pGraph);
	if (FAILED(hr)) { ReleaseAll(); return; }

	hr = pGraph->QueryInterface(IID_IMediaControl, (void **)&pControl);
	if (FAILED(hr)) { ReleaseAll(); return; }

	hr = pGraph->QueryInterface(IID_IMediaEvent, (void **)&pEvent);
	if (FAILED(hr)) { ReleaseAll(); return; }

	//hr = pGraph->QueryInterface(IID_IVideoWindow, (void **)&pVidWin);
	//if (FAILED(hr)) { ReleaseAll(); return; }

	hr = pGraph->QueryInterface(IID_IMediaSeeking, (void **)&pSeek);
	if (FAILED(hr)) { ReleaseAll(); return; }

	//Set the video window.
	//pVidWin->put_Owner((OAHWND)wndh);
	//pVidWin->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);

	//RECT grc;
	//GetClientRect(wndh, &grc);
	//pVidWin->SetWindowPosition(0, 0, grc.right, grc.bottom);

	hr = CoCreateInstance(CLSID_VideoMixingRenderer, NULL, CLSCTX_INPROC, IID_IBaseFilter, (void**)&pVmr);
	if (FAILED(hr)) { ReleaseAll(); return; }

	// Add the VMR to the filter graph.
	hr = pGraph->AddFilter(pVmr, L"Video Mixing Renderer");
	if (FAILED(hr)) { ReleaseAll(); return; }

	// Set the rendering mode.
	hr = pVmr->QueryInterface(IID_IVMRFilterConfig, (void**)&pConfig);
	if (FAILED(hr)) { ReleaseAll(); return; }

	hr = pConfig->SetRenderingMode(VMRMode_Windowless);
	if (FAILED(hr)) { ReleaseAll(); return; }

	hr = pVmr->QueryInterface(IID_IVMRWindowlessControl, (void**)&pWc);
	if (FAILED(hr)) { ReleaseAll(); return; }

	hr = pWc->SetVideoClippingWindow(hwndApp);
	if (FAILED(hr)) { ReleaseAll(); return; }

	// Find the native video size.
	long lWidth, lHeight;
	hr = pWc->GetNativeVideoSize(&lWidth, &lHeight, NULL, NULL);
	if (FAILED(hr)) { ReleaseAll(); return; }

	RECT rcSrc, rcDest;
	// Set the source rectangle.
	SetRect(&rcSrc, 0, 0, lWidth, lHeight);
	// Get the window client area.
	GetClientRect(hwndApp, &rcDest);
	// Set the destination rectangle.
	SetRect(&rcDest, 0, 0, rcDest.right, rcDest.bottom);
	// Set the video position.
	hr = pWc->SetVideoPosition(&rcSrc, &rcDest);
	if (FAILED(hr)) { ReleaseAll(); return; }
	PAINTSTRUCT ps;
	hdc = BeginPaint(hwndApp, &ps);

	HRGN rgnClient = CreateRectRgnIndirect(&rcSrc);
	HRGN rgnVideo = CreateRectRgnIndirect(&rcDest);
	CombineRgn(rgnClient, rgnClient, rgnVideo, RGN_DIFF);

	// Paint on window.
	HBRUSH hbr = GetSysColorBrush(COLOR_BTNFACE);
	FillRgn(hdc, rgnClient, hbr);

	// Clean up.
	DeleteObject(hbr);
	DeleteObject(rgnClient);
	DeleteObject(rgnVideo);

	std::wstring filename = std::wstring(file.begin(), file.end());
	hr = pGraph->RenderFile(filename.c_str(), NULL);
	if (FAILED(hr)) { ReleaseAll(); return; }

	hr = pControl->Run();
	if (FAILED(hr)) { ReleaseAll(); return; }

	hr = pWc->RepaintVideo(hwndApp, hdc);
	if (FAILED(hr)) { ReleaseAll(); return; }

	hr = pWc->DisplayModeChanged();
	if (FAILED(hr)) { ReleaseAll(); return; }

	loaded = true;
	GameController->SetGameState(CGameController::PLAY_VIDEO);
}