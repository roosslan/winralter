
#include "ComboCompletion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace winralter
{

	/////////////////////////////////////////////////////////////////////////////
	// CComboCompletion

	CComboCompletion::CComboCompletion()
	{
		m_bAutoComplete = TRUE;
	}

	CComboCompletion::~CComboCompletion()
	{
	}

	BEGIN_MESSAGE_MAP(CComboCompletion, CComboBox)
		//{{AFX_MSG_MAP(CComboCompletion)
		ON_CONTROL_REFLECT(CBN_EDITUPDATE, OnEditUpdate)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CComboCompletion message handlers

	BOOL CComboCompletion::PreTranslateMessage(MSG* pMsg)
	{
		// Need to check for backspace/delete. These will modify the text in
		// the edit box, causing the auto complete to just add back the text
		// the user has just tried to delete. 

		if (pMsg->wParam == 'A' && GetKeyState(VK_CONTROL) < 0) // Ctrl+A is pressed
		{
			this->SetEditSel(0, -1); // Select all text and move cursor to the end
		}

		if (pMsg->message == WM_KEYDOWN)
		{
			m_bAutoComplete = TRUE;

			int nVirtKey = (int)pMsg->wParam;
			if (nVirtKey == VK_DELETE || nVirtKey == VK_BACK)
				m_bAutoComplete = FALSE;
		}

		// if (pMsg->message == WM_HOTKEY){ }

		return CComboBox::PreTranslateMessage(pMsg);
	}

	void CComboCompletion::OnEditUpdate()
	{
		// if we are not to auto update the text, get outta here
		if (!m_bAutoComplete)
			return;

		// Get the text in the edit box
		CString str;
		GetWindowText(str);
		int nLength = str.GetLength();

		// Currently selected range
		DWORD dwCurSel = GetEditSel();
		WORD dStart = LOWORD(dwCurSel);
		WORD dEnd = HIWORD(dwCurSel);

		// Search for, and select in, and string in the combo box that is prefixed
		// by the text in the edit box
		if (SelectString(-1, str) == CB_ERR)
		{
			SetWindowText(str);		// No text selected, so restore what was there before
			if (dwCurSel != CB_ERR)
				SetEditSel(dStart, dEnd);	//restore cursor postion
		}

		// Set the text selection as the additional text that we have added
		if (dEnd < nLength && dwCurSel != CB_ERR)
			SetEditSel(dStart, dEnd);
		else
			SetEditSel(nLength, -1);
	}
}