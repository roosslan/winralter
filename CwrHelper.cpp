#include "CwrHelper.h"

namespace winralter
{
	std::string CwrHelper::GetConfigFilePath(int ConfigFileType)
	{
		CHAR current_path[MAX_PATH], my_documents[MAX_PATH];

		char* fn_ini = "";
		if (ConfigFileType == PermanentConfig)
			fn_ini = "\\winralter.ini";
		else if (ConfigFileType == TemporaryConfig)
			fn_ini = "\\winralter.inf";				// Temporary config file

		DWORD res = GetModuleFileName(NULL, current_path, MAX_PATH);

		// Removes filename from the path	
		PathRemoveFileSpec(current_path);								
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

	bool CwrHelper::ExecuteFile(CComboCompletion* m_ComboBox)
	{
			bool asAdministrator = false;
			CString cmdToLaunch;
			TCHAR sys_path[MAX_PATH];
			LPSTR programName = NULL;

			m_ComboBox->GetWindowText(cmdToLaunch);
			cmdToLaunch = std::regex_replace(static_cast<std::string>(cmdToLaunch), std::regex("^ +| +$"), "$1").c_str();	// removing leading and trailing spaces

			/* if (m_ComboBox.FindStringExact(-1, cmdToLaunch) == CB_ERR)
				m_ComboBox.AddString(cmdToLaunch);	*/

			GetSystemDirectory(sys_path, sizeof(sys_path));

			SHELLEXECUTEINFO ShExecInfo;
			ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
			ShExecInfo.fMask = NULL;
			ShExecInfo.hwnd = NULL;

			// Since SHORT is signed, high - order bit equals sign bit.
			// Therefore to check if CTRL key is pressed, we check if the value returned by GetKeyState() is negative
			if (GetKeyState(VK_LCONTROL) < 0 || GetKeyState(VK_RCONTROL) < 0)
				asAdministrator = true;

			ShExecInfo.lpVerb = asAdministrator ? "runas" : nullptr;  // "runas" Launches an application as Administrator
			ShExecInfo.lpParameters = PathGetArgs(cmdToLaunch);
			programName = cmdToLaunch.GetBuffer(cmdToLaunch.GetLength());
			PathRemoveArgs(programName);
			ShExecInfo.lpFile = programName;

			ShExecInfo.lpDirectory = sys_path;
			ShExecInfo.nShow = SW_NORMAL;
			ShExecInfo.hInstApp = NULL;

			if (ShellExecuteEx(&ShExecInfo))
				return true;
			else
				return false;
	}

	void CwrHelper::BrowseFile(CComboCompletion* m_ComboBox)
	{
		CString cmdToLaunch, textInCombobox;
		m_ComboBox->GetWindowText(textInCombobox);

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
							m_ComboBox->SetWindowText(cmdToLaunch);
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

	void CwrHelper::LoadCmdFromFile(CComboCompletion* m_ComboBox)
	{
		m_ComboBox->ResetContent();	// remove all items

		std::filesystem::path cwd = CwrHelper::GetConfigFilePath(CwrHelper::PermanentConfig);
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
				m_ComboBox->AddString(token.c_str());
				m_ComboBox->SetWindowText(token.c_str());
			}
			line.erase(0, pos + delimiter.length());
		}
	}

	bool CwrHelper::SaveCmdToFile(CString cmdToSave) // CComboCompletion* m_ComboBox)
	{
		CString cmdToLaunch;
		std::string readedLine, cmdForIni;

		cmdToLaunch = cmdToSave; // m_ComboBox->GetWindowText(cmdToLaunch);
		cmdForIni = cmdToLaunch;
		cmdForIni = std::regex_replace(cmdForIni, std::regex("^ +| +$"), "$1"); // removing leading and trailing spaces

//		if (ExecuteFile(m_ComboBox))
		{
			std::ofstream of;
			of.open(CwrHelper::GetConfigFilePath(CwrHelper::TemporaryConfig), std::ofstream::out | std::ofstream::app);

			bool cmdFound = false;
			std::ifstream ifs(CwrHelper::GetConfigFilePath(CwrHelper::PermanentConfig), std::ios_base::binary); // We'll looking for (already) existing line-command

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
				std::filesystem::remove(CwrHelper::GetConfigFilePath(CwrHelper::PermanentConfig));
				std::filesystem::rename(CwrHelper::GetConfigFilePath(CwrHelper::TemporaryConfig),
					CwrHelper::GetConfigFilePath(CwrHelper::PermanentConfig));
			}
			catch (std::filesystem::filesystem_error& e) {
				std::cout << e.what() << '\n';
			}
			return true;
		}
//		else
//			return false;
	}

	void CwrHelper::RemoveCmdFromFile(std::string removeCommand, CComboCompletion* m_ComboBox)
	{
		std::string readedLine;
		removeCommand = std::regex_replace(removeCommand, std::regex("^ +| +$"), "$1"); // removing leading and trailing spaces

		std::ofstream of;
		of.open(GetConfigFilePath(TemporaryConfig), std::ofstream::out | std::ofstream::app);

		bool cmdFound = false;
		std::ifstream ifs(GetConfigFilePath(PermanentConfig), std::ios_base::binary); // We'll looking for (already) existing line-command

		while (getline(ifs, readedLine)) {
			readedLine.erase(std::remove(readedLine.begin(), readedLine.end(), '\r'), readedLine.end());
			readedLine.erase(std::remove(readedLine.begin(), readedLine.end(), '\n'), readedLine.end());
			if (readedLine == removeCommand) {
				cmdFound = true;
			}
			else of << readedLine << "\n";
			//				if (cmdFound)
			//					break;
		}

		of.flush();
		of.close();

		ifs.close();

		try {
			std::filesystem::remove(GetConfigFilePath(PermanentConfig));
			std::filesystem::rename(GetConfigFilePath(TemporaryConfig), GetConfigFilePath(PermanentConfig));
			LoadCmdFromFile(m_ComboBox);
		}
		catch (std::filesystem::filesystem_error& e) {
			std::cout << e.what() << '\n';
		}
	}
}

