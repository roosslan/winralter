// winralter_dlg.cpp: implementation file

#include "winralter.h"
#include "winralter_dlg.h"
#include "cwr_helper.h"

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/

namespace winralter {
	cwinralter_dlg::cwinralter_dlg(CWnd* pParent /*=nullptr*/)
		: CDialog(cwinralter_dlg::IDD, pParent)
	{
		//{{AFX_DATA_INIT(cwinralter_dlg)
		//}}AFX_DATA_INIT
	}

	void cwinralter_dlg::DoDataExchange(CDataExchange* pDX) {
		CDialog::DoDataExchange(pDX);
		DDX_Control(pDX, IDC_COMBO, m_combobox);
	}

	BEGIN_MESSAGE_MAP(cwinralter_dlg, CDialog)
		ON_CBN_SELCHANGE(IDC_COMBO, OnSelchangeCombo)
		ON_WM_PAINT()
		ON_WM_CTLCOLOR()
		ON_BN_CLICKED(IDC_BUTTON_BROWSE, &cwinralter_dlg::on_browse_click)
		ON_WM_SYSCOMMAND()
		ON_REGISTERED_MESSAGE(
			WM_WINRALTER_ACTIVATE,
			OnActivateExistingInstance)
	END_MESSAGE_MAP()

	LRESULT cwinralter_dlg::OnActivateExistingInstance(WPARAM, LPARAM) {
		if (IsIconic())
			ShowWindow(SW_RESTORE);
		else
			ShowWindow(SW_SHOW);

		// Temporarily place the window above other windows.
		SetWindowPos(&CWnd::wndTopMost,	0, 0, 0, 0,	SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

		BringWindowToTop();
		SetForegroundWindow();
		SetActiveWindow();

		/* Remove the topmost status after raising the window */
		SetWindowPos(
			&CWnd::wndNoTopMost,
			0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

		SetForegroundWindow();
		m_combobox.SetFocus();

		return 0;
	}

	afx_msg void cwinralter_dlg::OnSysCommand(UINT nID, LPARAM lParam) {

		if ((nID & 0xFFF0) == IDM_ABOUT) {		
			const std::string about_text =
				m_wrVersion + std::string("\n(c) 2003-2026\nsborka.dev/winralter");
			AfxMessageBox(CA2T(about_text.c_str()), MB_OK | MB_ICONINFORMATION);
		}
		else {
			CDialog::OnSysCommand(nID, lParam);
		}
	}

	BOOL cwinralter_dlg::OnInitDialog()	{

		CDialog::OnInitDialog();

		/* Place the dialog above the taskbar, near the lower-left corner */
		CRect rcWindow;
		CRect rcWorkArea;
		GetWindowRect(&rcWindow);
		
		if (::SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0)) {
		    const int margin = 20;
		    int x = rcWorkArea.left + margin;
		    int y = rcWorkArea.bottom - rcWindow.Height() - margin;
		
		    if (y < rcWorkArea.top + margin)
		        y = rcWorkArea.top + margin;
		
		    SetWindowPos(nullptr, x, y, 0, 0,
		        SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
		}		

		m_PictureBackgroundBrush.CreateSolidBrush(RGB(255, 255, 255));

		const HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_MAIN));
		SetIcon(hIcon, false);

		const CMenu* hSysMenu = GetSystemMenu(false);
		AppendMenuW(*hSysMenu, MF_STRING, IDM_ABOUT, L"&About...");

		cwr_helper::load_cmd_from_file(&m_combobox);

		// char s[256];
		// sprintf_s(s, "%d\n,%d\n,%d\n,%d\n", message, wParam, LOWORD(wParam), HIWORD(wParam));
		// OutputDebugStringA(s);

		m_combobox.SetFocus();
		SetForegroundWindow();
		return false;				// return true unless you set the focus to a control
	}

	void cwinralter_dlg::OnPaint() {
		CPaintDC dc(this);

		CRect rect;
		GetClientRect(&rect);

		const int splitY = 110;

		// ¬ерхн€€ половина Ч бела€
		CBrush whiteBrush(RGB(255, 255, 255));
		dc.FillRect(
			CRect(
				rect.left,
				rect.top,
				rect.right,
				splitY),
			&whiteBrush);

		// Ќижн€€ половина Ч светло-сера€
		CBrush grayBrush(RGB(230, 230, 230));
		dc.FillRect(
			CRect(
				rect.left,
				splitY,
				rect.right,
				rect.bottom),
			&grayBrush);
	}

	HBRUSH cwinralter_dlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) {
		if (nCtlColor == CTLCOLOR_STATIC) {
			pDC->SetBkMode(TRANSPARENT);

			if (pWnd->GetDlgCtrlID() == IDC_PICTURE) {
				// ‘он Picture Control всегда белый
				return (HBRUSH)m_PictureBackgroundBrush.GetSafeHandle();
			}

			// ќстальные StaticText Ч прозрачные
			return (HBRUSH)GetStockObject(HOLLOW_BRUSH);
		}

		return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	}

	void cwinralter_dlg::OnOK() {
		CString command;
		m_combobox.GetWindowText(command);
		command.Trim();
		if (command.IsEmpty())
			return;

		if (!cwr_helper::execute_file(&m_combobox))
			return;

		/* History failures should not prevent a successfully launched app from running */
		if (!cwr_helper::save_cmd_to_file(command))	{
			AfxMessageBox(
				L"The command was started, but WinRAlter could not update the command history.",
				MB_OK | MB_ICONWARNING);
		}
		EndDialog(IDOK);
	}

	void cwinralter_dlg::OnSelchangeCombo()	{
		// TRACE0("Selection has changed\n");
	}

	/* "Browse..." for file button */
	void cwinralter_dlg::on_browse_click() {
		cwr_helper::browse_file(&m_combobox);
	}

}
