// winralterDlg.h: header file

#if !defined(AFX_WINRALTERDLG_H__115F4226_5CD5_11D1_ABBA_00A0243D1382__INCLUDED_)
#define AFX_WINRALTERDLG_H__115F4226_5CD5_11D1_ABBA_00A0243D1382__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ComboCompletion.h"

namespace winralter
{
	// CWinRAlterDlg dialog

	class CWinRAlterDlg : public CDialog
	{
		afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
		// Construction
	public:
		CWinRAlterDlg(CWnd* pParent = NULL);	// standard constructor
		char* m_wrVersion = "WinRAlter 1.0.2.1";
		// Dialog Data
			//{{AFX_DATA(CWinRAlterDlg)
		enum { IDD = IDD_WINRALTER_DIALOG };
		CComboCompletion m_ComboBox;
		//}}AFX_DATA
		afx_msg void OnBnClickedButtonBrowse();
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CWinRAlterDlg)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
		//}}AFX_VIRTUAL
	// Implementation
		// Generated message map functions
		//{{AFX_MSG(CWinRAlterDlg)
		virtual BOOL OnInitDialog();
		virtual void OnOK();
		afx_msg void OnSelchangeCombo();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
	};
}
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINRALTERDLG_H__115F4226_5CD5_11D1_ABBA_00A0243D1382__INCLUDED_)
