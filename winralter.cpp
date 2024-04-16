// winralter.cpp: Defines the class behaviors for the application

#include "winralter.h"
#include "winralterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace winralter
{
	// CWinRAlterApp
	BEGIN_MESSAGE_MAP(CWinRAlterApp, CWinApp)
		//{{AFX_MSG_MAP(CWinRAlterApp)
		//}}AFX_MSG
		//ON_COMMAND(ID_HELP, CWinApp::OnHelp)	
	END_MESSAGE_MAP()

	// CWinRAlterApp construction
	CWinRAlterApp::CWinRAlterApp()
	{
	}

	// The one and only CWinRAlterApp object
	CWinRAlterApp theApp;

	// CWinRAlterApp initialization
	BOOL CWinRAlterApp::InitInstance()
	{
		// Standard initialization

#ifdef _AFXDLL
	// CWinApp::Enable3dControls is no longer needed.You should remove this call
	// Enable3dControls();			// Call this when using MFC in a shared DLL
#else
		Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

//		BOOL rhk = RegisterHotKey(NULL, 1, MOD_WIN | MOD_NOREPEAT, 0x52);  // 0x52 is 'R'

		CWinRAlterDlg dlg;
		m_pMainWnd = &dlg;
		int nResponse = dlg.DoModal();
		if (nResponse == IDOK)
		{
		}
		else if (nResponse == IDCANCEL)
		{
		}
		else
		{
		}
		// Since the dialog has been closed, return FALSE so that we exit the
		//  application, rather than start the application's message pump.
		return FALSE;
	} // InitInstance
}