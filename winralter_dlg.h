#if !defined(AFX_WINRALTERDLG_H__115F4226_5CD5_11D1_ABBA_00A0243D1382__INCLUDED_)
#define AFX_WINRALTERDLG_H__115F4226_5CD5_11D1_ABBA_00A0243D1382__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "combo_completion.h"

namespace winralter {
	/* cwinralter_dlg dialog */

	class cwinralter_dlg : public CDialog {
		afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
		CBrush m_PictureBackgroundBrush;
	public:
		cwinralter_dlg(CWnd* pParent = nullptr);	// standard constructor
		char* m_wrVersion = "WinRAlter 2.1.0.1";
		// Dialog Data
			//{{AFX_DATA(cwinralter_dlg)
		enum { IDD = IDD_WINRALTER_DIALOG };
		ccombo_completion m_combobox;
		//}}AFX_DATA
		afx_msg void on_browse_click();
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(cwinralter_dlg)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
		//}}AFX_VIRTUAL
	// Implementation
		// Generated message map functions
		//{{AFX_MSG(cwinralter_dlg)
		virtual BOOL OnInitDialog();
		virtual void OnOK();
		afx_msg void OnSelchangeCombo();
		afx_msg void OnPaint();
		afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
	};
}
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINRALTERDLG_H__115F4226_5CD5_11D1_ABBA_00A0243D1382__INCLUDED_)
