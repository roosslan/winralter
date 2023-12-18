// winralterDlg.cpp: implementation file

#include "winralter.h"
#include "winralterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
namespace winralter
{
	constexpr auto PermanentConfig = 0;
	constexpr auto TemporaryConfig = 1;

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

	std::string CWinRAlterDlg::GetConfigFilePath(int ConfigFileType) {

		CHAR current_path[MAX_PATH], my_documents[MAX_PATH];

		char* fn_ini = "";
		if (ConfigFileType == PermanentConfig)
			fn_ini = "\\winralter.ini";
		else if (ConfigFileType == TemporaryConfig)
			fn_ini = "\\winralter.inf";		// Temporary config file

		DWORD res = GetModuleFileName(NULL, current_path, MAX_PATH);
		PathRemoveFileSpec(current_path);						// Removes filename from the path	
		strcat(current_path, fn_ini);
//		Use MyDocuments in case of file not found
		if (!std::filesystem::exists(std::string(current_path)) && ConfigFileType != TemporaryConfig)
		{
			HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);
			strcat(my_documents, fn_ini);
			return std::string(my_documents);
		}
		return std::string(current_path);
	}

	void CWinRAlterDlg::LoadCmdFromFile() {

		std::filesystem::path cwd = GetConfigFilePath(PermanentConfig);
		std::ifstream input_stream(cwd, std::ios_base::binary);

		try {
			if (input_stream.fail())
				throw std::runtime_error("Failed to open winralter.ini");
		}
		catch (std::exception& ex) {
			AfxMessageBox(ex.what());
		}

		std::stringstream buffer;
		buffer << input_stream.rdbuf();
		std::string line = buffer.str();
		std::string delimiter = "\r\n";
		size_t pos = 0;
		std::string token;
		// Splitting data from file into strings:
		while ((pos = line.find(delimiter)) != std::string::npos) {
			token = line.substr(0, pos);
			if (token != "") {
				m_ComboBox.AddString(token.c_str());
				m_ComboBox.SetWindowText(token.c_str());
			}
			line.erase(0, pos + delimiter.length());
		}
	}

	bool CWinRAlterDlg::SaveCmdToFile(bool asAdministrator) {

		CString cmdToLaunch;
		TCHAR sys_path[MAX_PATH];
		LPSTR programName = NULL;
		std::string readedLine, cmdForIni;

		m_ComboBox.GetWindowText(cmdToLaunch);
		cmdForIni = cmdToLaunch;
		cmdForIni = std::regex_replace(cmdForIni, std::regex("^ +| +$"), "$1"); // removing leading and trailing spaces
		cmdToLaunch = cmdForIni.c_str();

		if (m_ComboBox.FindStringExact(-1, cmdToLaunch) == CB_ERR)
			m_ComboBox.AddString(cmdToLaunch);

		GetSystemDirectory(sys_path, sizeof(sys_path));

		SHELLEXECUTEINFO ShExecInfo;
		ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask = NULL;
		ShExecInfo.hwnd = NULL;
		if (asAdministrator)
			ShExecInfo.lpVerb = "runas"; // "runas" Launches an application as Administrator.
		else
			ShExecInfo.lpVerb = NULL;
		ShExecInfo.lpParameters = PathGetArgs(cmdToLaunch);
		programName = cmdToLaunch.GetBuffer(cmdToLaunch.GetLength());
		PathRemoveArgs(programName);
		ShExecInfo.lpFile = programName;

		ShExecInfo.lpDirectory = sys_path;
		ShExecInfo.nShow = SW_NORMAL;
		ShExecInfo.hInstApp = NULL;

		if (ShellExecuteEx(&ShExecInfo))
		{
			// std::filesystem::path cwd = std::filesystem::current_path() / "winralter.inf";
			std::ofstream of;
			of.open(GetConfigFilePath(TemporaryConfig), std::ofstream::out | std::ofstream::app);

			bool cmdFound = false;
			std::ifstream ifs(GetConfigFilePath(PermanentConfig), std::ios_base::binary); // We'll looking for (already) existing line-command

			while (getline(ifs, readedLine)) {
				readedLine.erase(std::remove(readedLine.begin(), readedLine.end(), '\r'), readedLine.end());
				readedLine.erase(std::remove(readedLine.begin(), readedLine.end(), '\n'), readedLine.end());
				if (readedLine == cmdForIni) {
					cmdFound = true;
					//	readedLine.replace(readedLine.find(cmdForIni), cmdForIni.length(), "");
				}
				else of << readedLine << "\n";
				//				if (cmdFound)
				//					break;
			}

			//			if (!cmdFound)				
			of << cmdForIni << "\n";

			of.flush();
			of.close();

			ifs.close();


			try {
				std::filesystem::remove(GetConfigFilePath(PermanentConfig));
				std::filesystem::rename(GetConfigFilePath(TemporaryConfig), GetConfigFilePath(PermanentConfig));
			}
			catch (std::filesystem::filesystem_error& e) {
				std::cout << e.what() << '\n';
			}
			return true;
		}
		else
			return false;
		
	}

	BOOL CWinRAlterDlg::OnInitDialog()
	{
		CDialog::OnInitDialog();

		HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_MAIN));
		SetIcon(hIcon, FALSE);

		CMenu* hSysMenu = GetSystemMenu(FALSE);
		AppendMenuW(*hSysMenu, MF_STRING, IDM_ABOUT, L"&About...");

		LoadCmdFromFile();

		// char s[256];
		// sprintf_s(s, "%d\n,%d\n,%d\n,%d\n", message, wParam, LOWORD(wParam), HIWORD(wParam));
		// OutputDebugStringA(s);

		m_ComboBox.SetFocus();
		SetForegroundWindow();
		return FALSE;	// return TRUE  unless you set the focus to a control
	}

	void CWinRAlterDlg::OnOK()
	{
		// Since SHORT is signed, high - order bit equals sign bit.
		// Therefore to check if CTRL key is pressed, we check if the value returned by GetKeyState() is negative
		if (GetKeyState(VK_LCONTROL) < 0 || GetKeyState(VK_RCONTROL) < 0) {
			if (SaveCmdToFile(true))
				EndDialog(IDCANCEL); // Quit the programm
		}
		else
			if(SaveCmdToFile(false))
				EndDialog(IDCANCEL); // Quit the programm
	}

	void CWinRAlterDlg::OnSelchangeCombo()
	{
		// TRACE0("Selection has changed\n");
	}

	void CWinRAlterDlg::OnBnClickedButtonBrowse() // "Browse..." for file button
	{
		CString cmdToLaunch, textInCombobox;
		m_ComboBox.GetWindowText(textInCombobox);

		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
			COINIT_DISABLE_OLE1DDE);
		if (SUCCEEDED(hr))
		{
			IFileOpenDialog* pFileOpen;

			// Create the FileOpenDialog object.
			hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
				IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

			if (SUCCEEDED(hr))
			{
				// Show the Open dialog box.
				hr = pFileOpen->Show(NULL);

				// Get the file name from the dialog box.
				if (SUCCEEDED(hr))
				{
					IShellItem* pItem;
					hr = pFileOpen->GetResult(&pItem);
					if (SUCCEEDED(hr))
					{
						PWSTR pszFilePath;
						hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

						// Display the file name to the user.
						if (SUCCEEDED(hr))
						{
							cmdToLaunch = textInCombobox + CW2T(pszFilePath);
							m_ComboBox.SetWindowText(cmdToLaunch);
							CoTaskMemFree(pszFilePath);
						}
						pItem->Release();
					}
				}
				pFileOpen->Release();
			}
			CoUninitialize();
		}
	}
}