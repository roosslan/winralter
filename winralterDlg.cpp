// winralterDlg.cpp: implementation file

#include "winralter.h"
#include "winralterDlg.h"
#include "CwrHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
namespace winralter
{
	CWinRAlterDlg::CWinRAlterDlg(CWnd* pParent /*=NULL*/)
		: CDialog(CWinRAlterDlg::IDD, pParent)
	{
		//{{AFX_DATA_INIT(CWinRAlterDlg)
		//}}AFX_DATA_INIT
	}

	void CWinRAlterDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CWinRAlterDlg)
		DDX_Control(pDX, IDC_COMBO, m_ComboBox);
		//}}AFX_DATA_MAP
	}

	BEGIN_MESSAGE_MAP(CWinRAlterDlg, CDialog)
		//{{AFX_MSG_MAP(CWinRAlterDlg)
		ON_CBN_SELCHANGE(IDC_COMBO, OnSelchangeCombo)
		//}}AFX_MSG_MAP
		ON_BN_CLICKED(IDC_BUTTON_BROWSE, &CWinRAlterDlg::OnBnClickedButtonBrowse)
		ON_WM_SYSCOMMAND(WM_SYSCOMMAND, CWinRAlterDlg::OnSysCommand)
		// ON_COMMAND(IDM_ABOUT, CWinRAlterDlg::OnAbout)
	END_MESSAGE_MAP()

	///////////////////////////////////////////////////////////////////////////////
	// CWinRAlterDlg message handlers
	afx_msg void CWinRAlterDlg::OnSysCommand(UINT nID, LPARAM lParam) {

		if ((nID & 0xFFF0) == IDM_ABOUT) {
			int res = AfxMessageBox("(c) 2003-2024 www.0x1e.dev/winralter");
		}
		else {
			CDialog::OnSysCommand(nID, lParam);
		}
	}

	BOOL CWinRAlterDlg::OnInitDialog()
	{
		CDialog::OnInitDialog();
		SetWindowText(m_wrVersion);
		HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_MAIN));
		SetIcon(hIcon, FALSE);

		CMenu* hSysMenu = GetSystemMenu(FALSE);
		AppendMenuW(*hSysMenu, MF_STRING, IDM_ABOUT, L"&About...");

		CwrHelper::LoadCmdFromFile(&m_ComboBox);

		// char s[256];
		// sprintf_s(s, "%d\n,%d\n,%d\n,%d\n", message, wParam, LOWORD(wParam), HIWORD(wParam));
		// OutputDebugStringA(s);

		m_ComboBox.SetFocus();
		SetForegroundWindow();
		return FALSE;				// return TRUE  unless you set the focus to a control
	}

	void CWinRAlterDlg::OnOK()
	{
			if (CwrHelper::SaveCmdToFile(&m_ComboBox))
				EndDialog(IDCANCEL); // Quit the programm
	}

	void CWinRAlterDlg::OnSelchangeCombo()
	{
		// TRACE0("Selection has changed\n");
	}

	void CWinRAlterDlg::OnBnClickedButtonBrowse() // "Browse..." for file button
	{
		CwrHelper::BrowseFile(&m_ComboBox);

		// Adding quotes in case the command contains spaces
		CString strCheckSpaces;
		m_ComboBox.GetWindowText(strCheckSpaces);
		if (static_cast<std::string>(strCheckSpaces).find_first_of(' ') != std::string::npos)
		{
			if (strCheckSpaces[0] != '"')
				strCheckSpaces = '"' + strCheckSpaces;		

			if (strCheckSpaces[strCheckSpaces.GetLength() - 1] != '"') // Check last character
				strCheckSpaces = strCheckSpaces + '"';

			m_ComboBox.SetWindowText(strCheckSpaces);
		}
	}
}