// winralter.h: main header file for the WinRAlter application
//

#if !defined(AFX_WINRALTER_H__115F4224_5CD5_11D1_ABBA_00A0243D1382__INCLUDED_)
#define AFX_WINRALTER_H__115F4224_5CD5_11D1_ABBA_00A0243D1382__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

namespace winralter
{
	/////////////////////////////////////////////////////////////////////////////
	// CWinRAlterApp:
	// See winralter.cpp for the implementation of this class

	class CWinRAlterApp : public CWinApp
	{
	public:
		CWinRAlterApp();

		// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(CWinRAlterApp)
	public:
		virtual BOOL InitInstance();
		//}}AFX_VIRTUAL

	// Implementation

		//{{AFX_MSG(CWinRAlterApp)
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
	};

}
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINRALTER_H__115F4224_5CD5_11D1_ABBA_00A0243D1382__INCLUDED_)
