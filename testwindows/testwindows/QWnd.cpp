#include ".\qwnd.h"

QWnd::QWnd(void)
{
	m_hWnd = NULL;
	m_lpfnOldWndProc = NULL;
}

QWnd::~QWnd(void)
{

}

BOOL QWnd::SubClassWindow(HWND hWnd)
{
	assert(hWnd);
	if (m_hWnd == hWnd) return TRUE;

	m_lpfnOldWndProc = (WNDPROC)GetWindowLong(hWnd, GWL_WNDPROC);
	if (m_lpfnOldWndProc != QWnd::WndProc)
	{
		m_hWnd = hWnd;
		SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)this);
		SetWindowLong(m_hWnd, GWL_WNDPROC, (LONG)QWnd::WndProc);
		return TRUE;
	}
	return FALSE;
}

QWnd* QWnd::FromHandle(HWND hWnd)
{
	assert(hWnd);

	QWnd* pWnd = (QWnd*)GetWindowLong(hWnd, GWL_USERDATA);
	return pWnd;
}

BOOL QWnd::PreCreateWindow(
	CREATESTRUCT& cs
)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	BOOL bRet = ::GetClassInfoEx(cs.hInstance, cs.lpszClass, &wcex);
	if (bRet)
	{
		return TRUE;
	}

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = cs.hInstance;
	wcex.hIcon = (HICON)::LoadIcon(NULL, IDI_QUESTION);
	wcex.hIconSm = (HICON)::LoadIcon(NULL, IDI_QUESTION);
	wcex.hbrBackground = (HBRUSH)::GetStockObject(WHITE_BRUSH);
	wcex.hCursor = (HCURSOR)::LoadCursor(NULL, IDC_ARROW);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = cs.lpszClass;
	bRet = ::RegisterClassEx(&wcex);
	if (!bRet)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL QWnd::CreateEx(
	DWORD dwExStyle,
	LPCTSTR lpszClassName,
	LPCTSTR lpszWindowName,
	DWORD dwStyle,
	int x,
	int y,
	int nWidth,
	int nHeight,
	HWND hWndParent,
	HMENU nIDorHMenu,
	LPVOID lpParam
)
{
	HINSTANCE hInstance = (HINSTANCE)::GetModuleHandle(NULL);
	assert(hInstance);

	CREATESTRUCT cs;
	cs.cx = nWidth;
	cs.cy = nHeight;
	cs.dwExStyle = dwExStyle;
	cs.hInstance = hInstance;
	cs.hMenu = nIDorHMenu;
	cs.hwndParent = hWndParent;
	cs.lpCreateParams = lpParam;
	cs.lpszClass = lpszClassName;
	cs.lpszName = lpszWindowName;
	cs.style = dwStyle;
	cs.x = x;
	cs.y = y;
	BOOL bRet = PreCreateWindow(cs);
	if (!bRet)
	{
		MessageBox(NULL, _T("注册窗口类失败！"), _T("注册窗口类"), 0);
		return FALSE;
	}

	HWND hWnd = CreateWindowEx(cs.dwExStyle, cs.lpszClass, cs.lpszName, cs.style, cs.x, cs.y,
		cs.cx, cs.cy, cs.hwndParent, cs.hMenu, cs.hInstance, this);
	if (NULL == hWnd)
	{
		return FALSE;
	}
	m_hWnd = hWnd;

	SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)this);

	m_lpfnOldWndProc = (WNDPROC)GetWindowLong(m_hWnd, GWL_WNDPROC);
	if (m_lpfnOldWndProc != QWnd::WndProc)
	{
		// 子类化
		SetWindowLong(m_hWnd, GWL_WNDPROC, (LONG)QWnd::WndProc);
		WindowProc(WM_CREATE, 0, 0);
		WindowProc(WM_NCCREATE, 0, 0);
	}
	return TRUE;
}

// API Functions
BOOL QWnd::ShowWindow(int nCmdShow)
{
	assert(m_hWnd);
	return ::ShowWindow(m_hWnd, nCmdShow);
}

BOOL QWnd::UpdateWindow()
{
	assert(m_hWnd);
	return ::UpdateWindow(m_hWnd);
}

BOOL QWnd::DestroyWindow()
{
	assert(m_hWnd);
	return ::DestroyWindow(m_hWnd);
}

LRESULT QWnd::Default(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_lpfnOldWndProc == QWnd::WndProc)
	{
		return ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
	}

	return m_lpfnOldWndProc(m_hWnd, uMsg, wParam, lParam);
}









//////////////////////////////////////////////////////////////////////////////////////////////////
// Windows message handler
//////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK QWnd::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_CREATE || uMsg == WM_NCCREATE)
	{
		LPCREATESTRUCT pCs = (LPCREATESTRUCT)lParam;
		if (pCs)
		{
			QWnd* pWnd = (QWnd*)pCs->lpCreateParams;
			if (pWnd)
			{
				pWnd->m_hWnd = hWnd;
				pWnd->m_lpfnOldWndProc = (WNDPROC)GetWindowLong(hWnd, GWL_WNDPROC);
				return pWnd->WindowProc(uMsg, wParam, lParam);
			}
		}
	}

	QWnd* pWnd = (QWnd*)GetWindowLong(hWnd, GWL_USERDATA);
	if (pWnd)
	{
		return pWnd->WindowProc(uMsg, wParam, lParam);
	}
	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT QWnd::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_ACTIVATE:
		return OnActivate(wParam, lParam);
	case WM_ACTIVATEAPP:
		return OnActivateApp(wParam, lParam);
	case WM_ASKCBFORMATNAME:
		return OnAskCbFormatName(wParam, lParam);
	case WM_CANCELMODE:
		return OnCancelMode(wParam, lParam);
	case WM_CAPTURECHANGED:
		return OnCaptureChanged(wParam, lParam);
	case WM_CHANGECBCHAIN:
		return OnChangeCbChain(wParam, lParam);
	case WM_CHAR:
		return OnChar(wParam, lParam);
	case WM_CHARTOITEM:
		return OnCharToItem(wParam, lParam);
	case WM_CHILDACTIVATE:
		return OnChildActivate(wParam, lParam);
	case WM_CLOSE:
		return OnClose(wParam, lParam);
	case WM_COMPACTING:
		return OnCompacting(wParam, lParam);
	case WM_COMPAREITEM:
		return OnCompareItem(wParam, lParam);
	case WM_CONTEXTMENU:
		return OnContextMenu(wParam, lParam);
	case WM_COPYDATA:
		return OnCopyData(wParam, lParam);
	case WM_CREATE:
		return OnCreate(wParam, lParam);

	case WM_DEADCHAR:
		return OnDeadChar(wParam, lParam);
	case WM_DELETEITEM:
		return OnDeleteItem(wParam, lParam);
	case WM_DESTROY:
		return OnDestroy(wParam, lParam);
	case WM_DESTROYCLIPBOARD:
		return OnDestroyClipboard(wParam, lParam);
	case WM_DEVICECHANGE:
		return OnDeviceChange(wParam, lParam);
	case WM_DEVMODECHANGE:
		return OnDevModeChange(wParam, lParam);
	case WM_DRAWCLIPBOARD:
		return OnDrawClipboard(wParam, lParam);
	case WM_DRAWITEM:
		return OnDrawItem(wParam, lParam);
	case WM_DROPFILES:
		return OnDropFiles(wParam, lParam);
	case WM_ENABLE:
		return OnEnable(wParam, lParam);
	case WM_ENDSESSION:
		return OnEndSession(wParam, lParam);
	case WM_ENTERIDLE:
		return OnEnterIdle(wParam, lParam);
	case WM_ERASEBKGND:
		return OnEraseBkgnd(wParam, lParam);

	case WM_FONTCHANGE:
		return OnFontChange(wParam, lParam);
	case WM_GETDLGCODE:
		return OnGetDlgCode(wParam, lParam);
	case WM_GETMINMAXINFO:
		return OnGetMinMaxInfo(wParam, lParam);
	case WM_HELP:
		return OnHelpInfo(wParam, lParam);
	case WM_HSCROLL:
		return OnHScroll(wParam, lParam);
	case WM_HSCROLLCLIPBOARD:
		return OnHScrollClipboard(wParam, lParam);
	case WM_ICONERASEBKGND:
		return OnIconEraseBkgnd(wParam, lParam);
	case WM_INITMENU:
		return OnInitMenu(wParam, lParam);
	case WM_INITMENUPOPUP:
		return OnInitMenuPopup(wParam, lParam);
	case WM_KEYDOWN:
		return OnKeyDown(wParam, lParam);
	case WM_KEYUP:
		return OnKeyUp(wParam, lParam);
	case WM_KILLFOCUS:
		return OnKillFocus(wParam, lParam);

	case WM_LBUTTONDBLCLK:
		return OnLButtonDblClk(wParam, lParam);
	case WM_LBUTTONDOWN:
		return OnLButtonDown(wParam, lParam);
	case WM_LBUTTONUP:
		return OnLButtonUp(wParam, lParam);
	case WM_MBUTTONDBLCLK:
		return OnMButtonDblClk(wParam, lParam);
	case WM_MBUTTONDOWN:
		return OnMButtonDown(wParam, lParam);
	case WM_MBUTTONUP:
		return OnMButtonUp(wParam, lParam);
	case WM_MDIACTIVATE:
		return OnMDIActivate(wParam, lParam);
	case WM_MEASUREITEM:
		return OnMeasureItem(wParam, lParam);
	case WM_MENUCHAR:
		return OnMenuChar(wParam, lParam);
	case WM_MENUSELECT:
		return OnMenuSelect(wParam, lParam);
	case WM_MOUSEACTIVATE:
		return OnMouseActivate(wParam, lParam);
	case WM_MOUSEMOVE:
		return OnMouseMove(wParam, lParam);
	case WM_MOUSEWHEEL:
		return OnMouseWheel(wParam, lParam);
	case WM_MOVE:
		return OnMove(wParam, lParam);
	case WM_MOVING:
		return OnMoving(wParam, lParam);

	case WM_NCACTIVATE:
		return OnNcActivate(wParam, lParam);
	case WM_NCCALCSIZE:
		return OnNcCalcSize(wParam, lParam);
	case WM_NCCREATE:
		return OnNcCreate(wParam, lParam);
	case WM_NCDESTROY:
		return OnNcDestroy(wParam, lParam);
	case WM_NCHITTEST:
		return OnNcHitTest(wParam, lParam);
	case WM_NCLBUTTONDBLCLK:
		return OnNcLButtonDblClk(wParam, lParam);
	case WM_NCLBUTTONDOWN:
		return OnNcLButtonDown(wParam, lParam);
	case WM_NCLBUTTONUP:
		return OnNcLButtonUp(wParam, lParam);
	case WM_NCMBUTTONDBLCLK:
		return OnNcMButtonDblClk(wParam, lParam);
	case WM_NCMBUTTONDOWN:
		return OnNcMButtonDown(wParam, lParam);
	case WM_NCMBUTTONUP:
		return OnNcMButtonUp(wParam, lParam);
	case WM_NCMOUSEMOVE:
		return OnNcMouseMove(wParam, lParam);
	case WM_NCPAINT:
		return OnNcPaint(wParam, lParam);
	case WM_NCRBUTTONDBLCLK:
		return OnNcRButtonDblClk(wParam, lParam);
	case WM_NCRBUTTONDOWN:
		return OnNcRButtonDown(wParam, lParam);
	case WM_NCRBUTTONUP:
		return OnNcRButtonUp(wParam, lParam);

	case WM_PAINT:
		return OnPaint(wParam, lParam);
	case WM_PAINTCLIPBOARD:
		return OnPaintClipboard(wParam, lParam);
	case WM_PALETTECHANGED:
		return OnPaletteChanged(wParam, lParam);
	case WM_PALETTEISCHANGING:
		return OnPaletteIsChanging(wParam, lParam);
	case WM_PARENTNOTIFY:
		return OnParentNotify(wParam, lParam);
	case WM_QUERYDRAGICON:
		return OnQueryDragIcon(wParam, lParam);
	case WM_QUERYENDSESSION:
		return OnQueryEndSession(wParam, lParam);
	case WM_QUERYNEWPALETTE:
		return OnQueryNewPalette(wParam, lParam);
	case WM_QUERYOPEN:
		return OnQueryOpen(wParam, lParam);
	case WM_RBUTTONDBLCLK:
		return OnNcRButtonDblClk(wParam, lParam);
	case WM_RBUTTONDOWN:
		return OnRButtonDown(wParam, lParam);
	case WM_RBUTTONUP:
		return OnRButtonUp(wParam, lParam);
	case WM_RENDERALLFORMATS:
		return OnRenderAllFormats(wParam, lParam);
	case WM_RENDERFORMAT:
		return OnRenderFormat(wParam, lParam);

	case WM_SETCURSOR:
		return OnSetCursor(wParam, lParam);
	case WM_SETFOCUS:
		return OnSetFocus(wParam, lParam);
	case WM_SHOWWINDOW:
		return OnShowWindow(wParam, lParam);
	case WM_SIZE:
		return OnSize(wParam, lParam);
	case WM_SIZECLIPBOARD:
		return OnSizeClipboard(wParam, lParam);
	case WM_SIZING:
		return OnSizing(wParam, lParam);
	case WM_SPOOLERSTATUS:
		return OnSpoolerStatus(wParam, lParam);
	case WM_STYLECHANGED:
		return OnStyleChanged(wParam, lParam);
	case WM_STYLECHANGING:
		return OnStyleChanging(wParam, lParam);
	case WM_SYSCHAR:
		return OnSysChar(wParam, lParam);
	case WM_SYSCOLORCHANGE:
		return OnSysColorChange(wParam, lParam);
	case WM_SYSCOMMAND:
		return OnSysCommand(wParam, lParam);
	case WM_SYSDEADCHAR:
		return OnSysDeadChar(wParam, lParam);
	case WM_SYSKEYDOWN:
		return OnSysKeyDown(wParam, lParam);
	case WM_SYSKEYUP:
		return OnSysKeyUp(wParam, lParam);

	case WM_TCARD:
		return OnTCard(wParam, lParam);
	case WM_TIMECHANGE:
		return OnTimeChange(wParam, lParam);
	case WM_TIMER:
		return OnTimer(wParam, lParam);
	case WM_VKEYTOITEM:
		return OnVKeyToItem(wParam, lParam);
	case WM_VSCROLL:
		return OnVScroll(wParam, lParam);
	case WM_VSCROLLCLIPBOARD:
		return OnVScrollClipboard(wParam, lParam);
	case WM_WINDOWPOSCHANGED:
		return OnWindowPosChanged(wParam, lParam);
	case WM_WINDOWPOSCHANGING:
		return OnWindowPosChanging(wParam, lParam);
	case WM_WININICHANGE:
		return OnWinIniChange(wParam, lParam);

	default:
		break;
	}
	return Default(uMsg, wParam, lParam);
}

// WM_ Message Handlers: A - C 
LRESULT QWnd::OnActivate(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_ACTIVATE, wParam, lParam);
}

LRESULT QWnd::OnActivateApp(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_ACTIVATEAPP, wParam, lParam);
}

LRESULT QWnd::OnAskCbFormatName(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_ASKCBFORMATNAME, wParam, lParam);
}

LRESULT QWnd::OnCancelMode(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_CANCELMODE, wParam, lParam);
}

LRESULT QWnd::OnCaptureChanged(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_CAPTURECHANGED, wParam, lParam);
}

LRESULT QWnd::OnChangeCbChain(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_CHANGECBCHAIN, wParam, lParam);
}

LRESULT QWnd::OnChar(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_CHAR, wParam, lParam);
}

LRESULT QWnd::OnCharToItem(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_CHARTOITEM, wParam, lParam);
}

LRESULT QWnd::OnChildActivate(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_CHILDACTIVATE, wParam, lParam);
}

LRESULT QWnd::OnClose(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_CLOSE, wParam, lParam);
}

LRESULT QWnd::OnCompacting(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_COMPACTING, wParam, lParam);
}

LRESULT QWnd::OnCompareItem(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_COMPAREITEM, wParam, lParam);
}

LRESULT QWnd::OnContextMenu(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_CONTEXTMENU, wParam, lParam);
}

LRESULT QWnd::OnCopyData(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_COPYDATA, wParam, lParam);
}

LRESULT QWnd::OnCreate(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_CREATE, wParam, lParam);
}

// WM_ Message Handlers: D - E 
LRESULT QWnd::OnDeadChar(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_DEADCHAR, wParam, lParam);
}

LRESULT QWnd::OnDeleteItem(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_DELETEITEM, wParam, lParam);
}

LRESULT QWnd::OnDestroy(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_DESTROY, wParam, lParam);
}

LRESULT QWnd::OnDestroyClipboard(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_DESTROYCLIPBOARD, wParam, lParam);
}

LRESULT QWnd::OnDeviceChange(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_DEVICECHANGE, wParam, lParam);
}

LRESULT QWnd::OnDevModeChange(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_DEVMODECHANGE, wParam, lParam);
}

LRESULT QWnd::OnDrawClipboard(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_DRAWCLIPBOARD, wParam, lParam);
}

LRESULT QWnd::OnDrawItem(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_DRAWITEM, wParam, lParam);
}

LRESULT QWnd::OnDropFiles(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_DROPFILES, wParam, lParam);
}

LRESULT QWnd::OnEnable(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_ENABLE, wParam, lParam);
}

LRESULT QWnd::OnEndSession(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_ENDSESSION, wParam, lParam);
}

LRESULT QWnd::OnEnterIdle(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_ENTERIDLE, wParam, lParam);
}

LRESULT QWnd::OnEraseBkgnd(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_ERASEBKGND, wParam, lParam);
}

// WM_ Message Handlers: F - K
LRESULT QWnd::OnFontChange(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_FONTCHANGE, wParam, lParam);
}

LRESULT QWnd::OnGetDlgCode(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_GETDLGCODE, wParam, lParam);
}

LRESULT QWnd::OnGetMinMaxInfo(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_GETMINMAXINFO, wParam, lParam);
}

LRESULT QWnd::OnHelpInfo(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_HELP, wParam, lParam);
}

LRESULT QWnd::OnHScroll(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_HSCROLL, wParam, lParam);
}

LRESULT QWnd::OnHScrollClipboard(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_HSCROLLCLIPBOARD, wParam, lParam);
}

LRESULT QWnd::OnIconEraseBkgnd(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_ICONERASEBKGND, wParam, lParam);
}

LRESULT QWnd::OnInitMenu(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_INITMENU, wParam, lParam);
}

LRESULT QWnd::OnInitMenuPopup(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_INITMENUPOPUP, wParam, lParam);
}

LRESULT QWnd::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_KEYDOWN, wParam, lParam);
}

LRESULT QWnd::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_KEYUP, wParam, lParam);
}

LRESULT QWnd::OnKillFocus(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_KILLFOCUS, wParam, lParam);
}

//WM_ Message Handlers: L - M 
LRESULT QWnd::OnLButtonDblClk(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_LBUTTONDBLCLK, wParam, lParam);
}

LRESULT QWnd::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_LBUTTONDOWN, wParam, lParam);
}

LRESULT QWnd::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_LBUTTONUP, wParam, lParam);
}

LRESULT QWnd::OnMButtonDblClk(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_MBUTTONDBLCLK, wParam, lParam);
}

LRESULT QWnd::OnMButtonDown(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_MBUTTONDOWN, wParam, lParam);
}

LRESULT QWnd::OnMButtonUp(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_MBUTTONUP, wParam, lParam);
}

LRESULT QWnd::OnMDIActivate(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_MDIACTIVATE, wParam, lParam);
}

LRESULT QWnd::OnMeasureItem(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_MEASUREITEM, wParam, lParam);
}

LRESULT QWnd::OnMenuChar(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_MENUCHAR, wParam, lParam);
}

LRESULT QWnd::OnMenuSelect(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_MENUSELECT, wParam, lParam);
}

LRESULT QWnd::OnMouseActivate(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_MOUSEACTIVATE, wParam, lParam);
}

LRESULT QWnd::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_MOUSEMOVE, wParam, lParam);
}

LRESULT QWnd::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_MOUSEWHEEL, wParam, lParam);
}

LRESULT QWnd::OnMove(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_MOVE, wParam, lParam);
}

LRESULT QWnd::OnMoving(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_MOVING, wParam, lParam);
}

//WM_ Message Handlers: N - O 
LRESULT QWnd::OnNcActivate(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_NCACTIVATE, wParam, lParam);
}

LRESULT QWnd::OnNcCalcSize(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_NCCALCSIZE, wParam, lParam);
}

LRESULT QWnd::OnNcCreate(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_NCCREATE, wParam, lParam);
}

LRESULT QWnd::OnNcDestroy(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_NCDESTROY, wParam, lParam);
}

LRESULT QWnd::OnNcHitTest(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_NCHITTEST, wParam, lParam);
}

LRESULT QWnd::OnNcLButtonDblClk(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_NCLBUTTONDBLCLK, wParam, lParam);
}

LRESULT QWnd::OnNcLButtonDown(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_NCLBUTTONDOWN, wParam, lParam);
}

LRESULT QWnd::OnNcLButtonUp(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_NCLBUTTONUP, wParam, lParam);
}

LRESULT QWnd::OnNcMButtonDblClk(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_NCRBUTTONDBLCLK, wParam, lParam);
}

LRESULT QWnd::OnNcMButtonDown(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_NCMBUTTONDOWN, wParam, lParam);
}

LRESULT QWnd::OnNcMButtonUp(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_NCMBUTTONUP, wParam, lParam);
}

LRESULT QWnd::OnNcMouseMove(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_NCMOUSEMOVE, wParam, lParam);
}

LRESULT QWnd::OnNcPaint(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_NCPAINT, wParam, lParam);
}

LRESULT QWnd::OnNcRButtonDblClk(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_NCRBUTTONDBLCLK, wParam, lParam);
}

LRESULT QWnd::OnNcRButtonDown(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_NCRBUTTONDOWN, wParam, lParam);
}

LRESULT QWnd::OnNcRButtonUp(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_NCRBUTTONUP, wParam, lParam);
}

// WM_ Messages: P - R 
LRESULT QWnd::OnPaint(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_PAINT, wParam, lParam);
}

LRESULT QWnd::OnPaintClipboard(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_PAINTCLIPBOARD, wParam, lParam);
}

LRESULT QWnd::OnPaletteChanged(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_PALETTECHANGED, wParam, lParam);
}

LRESULT QWnd::OnPaletteIsChanging(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_PALETTEISCHANGING, wParam, lParam);
}

LRESULT QWnd::OnParentNotify(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_PARENTNOTIFY, wParam, lParam);
}

LRESULT QWnd::OnQueryDragIcon(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_QUERYDRAGICON, wParam, lParam);
}

LRESULT QWnd::OnQueryEndSession(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_QUERYENDSESSION, wParam, lParam);
}

LRESULT QWnd::OnQueryNewPalette(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_QUERYNEWPALETTE, wParam, lParam);
}

LRESULT QWnd::OnQueryOpen(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_QUERYOPEN, wParam, lParam);
}

LRESULT QWnd::OnRButtonDblClk(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_RBUTTONDBLCLK, wParam, lParam);
}

LRESULT QWnd::OnRButtonDown(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_RBUTTONDOWN, wParam, lParam);
}

LRESULT QWnd::OnRButtonUp(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_RBUTTONUP, wParam, lParam);
}

LRESULT QWnd::OnRenderAllFormats(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_RENDERALLFORMATS, wParam, lParam);
}

LRESULT QWnd::OnRenderFormat(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_RENDERFORMAT, wParam, lParam);
}


// WM_ Messages: S 
LRESULT QWnd::OnSetCursor(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_SETCURSOR, wParam, lParam);
}

LRESULT QWnd::OnSetFocus(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_SETFOCUS, wParam, lParam);
}

LRESULT QWnd::OnShowWindow(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_SHOWWINDOW, wParam, lParam);
}

LRESULT QWnd::OnSize(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_SIZE, wParam, lParam);
}

LRESULT QWnd::OnSizeClipboard(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_SIZECLIPBOARD, wParam, lParam);
}

LRESULT QWnd::OnSizing(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_SIZING, wParam, lParam);
}

LRESULT QWnd::OnSpoolerStatus(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_SPOOLERSTATUS, wParam, lParam);
}

LRESULT QWnd::OnStyleChanged(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_STYLECHANGED, wParam, lParam);
}

LRESULT QWnd::OnStyleChanging(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_STYLECHANGING, wParam, lParam);
}

LRESULT QWnd::OnSysChar(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_SYSCHAR, wParam, lParam);
}

LRESULT QWnd::OnSysColorChange(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_SYSCOLORCHANGE, wParam, lParam);
}

LRESULT QWnd::OnSysCommand(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_SYSCOMMAND, wParam, lParam);
}

LRESULT QWnd::OnSysDeadChar(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_DEADCHAR, wParam, lParam);
}

LRESULT QWnd::OnSysKeyDown(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_SYSKEYDOWN, wParam, lParam);
}

LRESULT QWnd::OnSysKeyUp(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_SYSKEYUP, wParam, lParam);
}

//WM_ Messages: T - Z 
LRESULT QWnd::OnTCard(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_TCARD, wParam, lParam);
}

LRESULT QWnd::OnTimeChange(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_TIMECHANGE, wParam, lParam);
}

LRESULT QWnd::OnTimer(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_TIMECHANGE, wParam, lParam);
}

LRESULT QWnd::OnVKeyToItem(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_VKEYTOITEM, wParam, lParam);
}

LRESULT QWnd::OnVScroll(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_VSCROLL, wParam, lParam);
}

LRESULT QWnd::OnVScrollClipboard(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_VSCROLLCLIPBOARD, wParam, lParam);
}

LRESULT QWnd::OnWindowPosChanged(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_WINDOWPOSCHANGED, wParam, lParam);
}

LRESULT QWnd::OnWindowPosChanging(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_WINDOWPOSCHANGING, wParam, lParam);
}

LRESULT QWnd::OnWinIniChange(WPARAM wParam, LPARAM lParam)
{
	return Default(WM_WININICHANGE, wParam, lParam);
}