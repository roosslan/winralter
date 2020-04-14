#if !defined(AFX_ComboCompletion_H__115F422E_5CD5_11D1_ABBA_00A0243D1382__INCLUDED_)
#define AFX_ComboCompletion_H__115F422E_5CD5_11D1_ABBA_00A0243D1382__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

namespace winralter {
	/////////////////////////////////////////////////////////////////////////////
	// ccombo_completion window

	class ccombo_completion final : public CComboBox {
		// Construction
	public:
		ccombo_completion();
		// Attributes
	public:
		// Operations
	public:
		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(ccombo_completion)
	public:
		virtual BOOL PreTranslateMessage(MSG* pMsg);
		//}}AFX_VIRTUAL
	// Implementation
	public:
		virtual ~ccombo_completion();
		BOOL m_bAutoComplete;
		// Generated message map functions
	protected:
		//{{AFX_MSG(ccombo_completion)
		afx_msg void OnEditUpdate();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
	};
}
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ComboCompletion_H__115F422E_5CD5_11D1_ABBA_00A0243D1382__INCLUDED_)
