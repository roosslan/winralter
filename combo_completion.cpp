#include "cwr_helper.h"
#include "combo_completion.h"

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/

namespace winralter {

	ccombo_completion::ccombo_completion(){
		m_bAutoComplete = TRUE;
	}

	ccombo_completion::~ccombo_completion(){
	}

	BEGIN_MESSAGE_MAP(ccombo_completion, CComboBox)
		//{{AFX_MSG_MAP(ccombo_completion)
		ON_CONTROL_REFLECT(CBN_EDITUPDATE, OnEditUpdate)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()


	// ccombo_completion message handlers

	BOOL ccombo_completion::PreTranslateMessage(MSG* pMsg) {
		if (pMsg->message == WM_KEYDOWN)
		{
			const int nVirtKey = static_cast<int>(pMsg->wParam);
			const bool ctrl_pressed = (GetKeyState(VK_CONTROL) < 0);
			const bool shift_pressed = (GetKeyState(VK_SHIFT) < 0);

			// Ctrl+A: select the complete contents of the editable field.
			if (nVirtKey == 'A' && ctrl_pressed)
			{
				SetEditSel(0, -1);
				return TRUE;
			}

			// Ctrl+Shift+Delete removes the current history item.
			if (nVirtKey == VK_DELETE && ctrl_pressed && shift_pressed)
			{
				CString command;
				const int current_selection = GetCurSel();

				if (current_selection != CB_ERR)
					GetLBText(current_selection, command);
				else
					GetWindowText(command);

				command.Trim();
				if (!command.IsEmpty())
					cwr_helper::remove_cmd_from_file(command, this);

				return TRUE;
			}

			// Backspace/Delete modify the edit field. Suppress autocomplete for
			// the edit notification caused by that key.
			m_bAutoComplete = !((nVirtKey == VK_DELETE) || (nVirtKey == VK_BACK));
		}

		return CComboBox::PreTranslateMessage(pMsg);
	}

	void ccombo_completion::OnEditUpdate()
	{
		// Ignore only the edit notification caused by Backspace/Delete,
		// then re-enable autocomplete for the next edit.
		if (!m_bAutoComplete)
		{
			m_bAutoComplete = TRUE;
			return;
		}

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