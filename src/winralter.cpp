// winralter.cpp: Defines the class behaviors for the application

#include "winralter.h"
#include "winralter_dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace winralter {

	UINT WM_WINRALTER_ACTIVATE =
		::RegisterWindowMessageW(L"winralter.activate.1");
	BEGIN_MESSAGE_MAP(CWinRAlterApp, CWinApp)
		//{{AFX_MSG_MAP(CWinRAlterApp)
		//}}AFX_MSG
		//ON_COMMAND(ID_HELP, CWinApp::OnHelp)	
	END_MESSAGE_MAP()

	CWinRAlterApp::CWinRAlterApp() { }

	/* The one and only CWinRAlterApp object */
	CWinRAlterApp the_app;

	BOOL CWinRAlterApp::InitInstance() {
		// Standard initialization

#ifdef _AFXDLL
	// CWinApp::Enable3dControls is no longer needed.You should remove this call
	// Enable3dControls();			// Call this when using MFC in a shared DLL
#else
		Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
		// Allow only one running instance of WinRAlter.
		HANDLE hMutex = ::CreateMutexW(
			nullptr, FALSE, L"Local\\WinRAlter.SingleInstance");
		if (hMutex == nullptr)
			return false;

		if (::GetLastError() == ERROR_ALREADY_EXISTS) {
			/* Ask the running instance to activate its dialog */
			DWORD_PTR result = 0;
			::SendMessageTimeoutW(
			HWND_BROADCAST,
			WM_WINRALTER_ACTIVATE,
			0,
			0,
			SMTO_ABORTIFHUNG,
			1000,
			&result);
			::CloseHandle(hMutex);

			return false;
		}

	/*	BOOL b_hk = RegisterHotKey(nullptr, 1, MOD_WIN | MOD_NOREPEAT, 0x52);  // 0x52 is 'R' */

		cwinralter_dlg dlg;
		m_pMainWnd = &dlg;
		int n_response = dlg.DoModal();
		if (n_response == IDOK) { }
		else if (n_response == IDCANCEL) { }
		else { }
		::CloseHandle(hMutex);
		/* Since the dialog has been closed, return FALSE so that we exit the
		   application, rather than start the application's message pump		*/
		return false;
	} // InitInstance
}