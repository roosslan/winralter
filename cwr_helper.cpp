#include "cwr_helper.h"
#include <vector>
#include <atlconv.h>

namespace winralter {
	std::string get_config_file_path(const int config_file_type){
		// Keep the temporary file beside the destination: replacement must not
		// cross volumes, and should be atomic whenever the filesystem supports it.
		if (config_file_type == 1) {
			const std::filesystem::path history(permanent_config);
			return (history.parent_path() / "winralter.inf").string();
		}

		// Use explicit ANSI APIs because this function stores paths in std::string.
		// Generic Win32 APIs resolve to their Unicode variants in Unicode builds.
		CHAR current_path[MAX_PATH]{};
		const char* fn_ini = "\\winralter.ini";

		const DWORD length = GetModuleFileNameA(nullptr, current_path, MAX_PATH);
		if (length == 0 || length >= MAX_PATH)
			return std::string(fn_ini + 1);

		if (!PathRemoveFileSpecA(current_path))
			return std::string(fn_ini + 1);

		if (strcat_s(current_path, MAX_PATH, fn_ini) != 0)
			return std::string(fn_ini + 1);

		// Fall back to My Documents if the adjacent INI file does not exist.
		if (!std::filesystem::exists(std::string(current_path)) && config_file_type != 1) {
			CHAR my_documents[MAX_PATH]{};
			const HRESULT result = SHGetFolderPathA(
				nullptr, CSIDL_PERSONAL, nullptr, SHGFP_TYPE_CURRENT, my_documents);
			if (SUCCEEDED(result) &&
				strcat_s(my_documents, MAX_PATH, fn_ini) == 0)
				return std::string(my_documents);
		}
		return std::string(current_path);
	}

	static CString shell_error_text(const DWORD error) {
		switch (error)
		{
		case ERROR_FILE_NOT_FOUND:
			return L"The specified program or file was not found.";
		case ERROR_PATH_NOT_FOUND:
			return L"The specified path was not found.";
		case ERROR_ACCESS_DENIED:
			return L"Access was denied. Try Ctrl+Enter to run as Administrator.";
		case ERROR_BAD_EXE_FORMAT:
			return L"The selected file is not a valid Windows application.";
		default:
			break;
		}

		WCHAR buffer[512]{};
		const DWORD length = FormatMessageW(
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr,
			error,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			buffer,
			static_cast<DWORD>(std::size(buffer)),
			nullptr);

		if (length != 0)
		{
			CString message(buffer, static_cast<int>(length));
			message.Trim();
			return message;
		}

		return L"Windows could not start the command.";
	}

	bool cwr_helper::execute_file(const ccombo_completion* m_combobox){
		CStringW command;
		m_combobox->GetWindowText(command);
		command.Trim();
		if (command.IsEmpty())
			return false;

		// Windows command lines allow a quoted executable path followed by arguments.
		// For unquoted commands, treat the first whitespace as the argument boundary.
		std::wstring text(command.GetString());
		std::wstring executable;
		std::wstring parameters;
		if (!text.empty() && text.front() == L'"') {
			const size_t closing_quote = text.find(L'"', 1);
			if (closing_quote == std::wstring::npos)
			{
				AfxMessageBox(
					L"Unable to start the command because the executable path has an unmatched quote.",
					MB_OK | MB_ICONWARNING);
				return false;
			}
			executable = text.substr(1, closing_quote - 1);
			parameters = text.substr(closing_quote + 1);
			const size_t first = parameters.find_first_not_of(L" 	");
			parameters = first == std::wstring::npos ? L"" : parameters.substr(first);
		}
		else {
			const size_t first_space = text.find_first_of(L" 	");
			executable = text.substr(0, first_space);
			if (first_space != std::wstring::npos) {
				const size_t first = text.find_first_not_of(L" 	", first_space);
				parameters = first == std::wstring::npos ? L"" : text.substr(first);
			}
		}
		if (executable.empty())
			return false;

		SHELLEXECUTEINFOW info{};
		info.cbSize = sizeof(info);
		info.fMask = SEE_MASK_FLAG_NO_UI;
		info.hwnd = m_combobox->GetSafeHwnd();
		const bool run_as_admin = (::GetKeyState(VK_CONTROL) < 0);
		info.lpVerb = run_as_admin ? L"runas" : nullptr;
		info.lpFile = executable.c_str();
		info.lpParameters = parameters.empty() ? nullptr : parameters.c_str();
		info.lpDirectory = nullptr;
		info.nShow = SW_SHOWNORMAL;

		if (ShellExecuteExW(&info))
			return true;

		const DWORD error = GetLastError();
		if (error != ERROR_CANCELLED)
		{
			CString message = L"Unable to start the command.";
			message += L"\n\n";
			message += shell_error_text(error);
			message.AppendFormat(L"\n\nWindows error %lu.", error);
			AfxMessageBox(message, MB_OK | MB_ICONERROR);
		}
		return false;
	}

	void cwr_helper::browse_file(ccombo_completion* m_combobox)
	{
		const HRESULT init_result = CoInitializeEx(
			nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

		if (init_result == RPC_E_CHANGED_MODE)
		{
			AfxMessageBox(
				L"Unable to open the file picker because this thread uses an incompatible COM apartment.",
				MB_OK | MB_ICONERROR);
			return;
		}

		if (FAILED(init_result))
		{
			AfxMessageBox(
				L"Unable to initialize the Windows file picker.",
				MB_OK | MB_ICONERROR);
			return;
		}

		const HWND owner = m_combobox->GetParent()->GetSafeHwnd();
		CComPtr<IFileOpenDialog> file_open;
		HRESULT hr = CoCreateInstance(
			CLSID_FileOpenDialog,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&file_open));

		if (SUCCEEDED(hr))
		{
			DWORD options = 0;
			if (SUCCEEDED(file_open->GetOptions(&options)))
				file_open->SetOptions(
					options | FOS_FILEMUSTEXIST | FOS_PATHMUSTEXIST | FOS_FORCEFILESYSTEM);

			hr = file_open->Show(owner);
			if (SUCCEEDED(hr))
			{
				CComPtr<IShellItem> item;
				hr = file_open->GetResult(&item);

				if (SUCCEEDED(hr))
				{
					PWSTR file_path = nullptr;
					hr = item->GetDisplayName(SIGDN_FILESYSPATH, &file_path);

					if (SUCCEEDED(hr) && file_path != nullptr)
					{
						CString command(file_path);
						CoTaskMemFree(file_path);

						if (command.Find(_T(' ')) != -1 &&
							(command.IsEmpty() || command[0] != _T('"')))
						{
							command = _T('"') + command + _T('"');
						}

						m_combobox->SetWindowText(command);
						m_combobox->SetFocus();
						m_combobox->SetEditSel(command.GetLength(), -1);
					}
				}
			}
		}

		if (hr != HRESULT_FROM_WIN32(ERROR_CANCELLED) && FAILED(hr))
		{
			CString message;
			message.Format(L"Unable to open the file picker (HRESULT 0x%08lX).", hr);
			AfxMessageBox(message, MB_OK | MB_ICONERROR);
		}

		CoUninitialize();
	}

	void cwr_helper::load_cmd_from_file(ccombo_completion* m_combobox)
	{
		m_combobox->ResetContent();
		std::ifstream input_stream(std::filesystem::path(permanent_config), std::ios::binary);
		if (!input_stream.is_open()) {
			if (std::filesystem::exists(std::filesystem::path(permanent_config)))
				AfxMessageBox(L"Unable to read the command history.", MB_OK | MB_ICONWARNING);
			return;
		}

		std::string line;
		while (std::getline(input_stream, line)) {
			if (!line.empty() && line.back() == '\r')
				line.pop_back();
			if (line.empty())
				continue;
			CStringW command(CA2W(line.c_str(), CP_UTF8));
			if (m_combobox->FindStringExact(-1, command) == CB_ERR)
				m_combobox->AddString(command);
		}
		m_combobox->SetWindowText(L"");
	}

	bool cwr_helper::save_cmd_to_file(const CString& cmd_to_save)
	{
		CStringW command(cmd_to_save);
		command.Trim();
		if (command.IsEmpty())
			return false;

		CT2A utf8_command(command, CP_UTF8);
		const std::string command_utf8(utf8_command);
		const std::filesystem::path destination(permanent_config);
		const std::filesystem::path temporary(temporary_config);
		std::error_code ec;

		std::ifstream input(destination, std::ios::binary);
		if (!input && std::filesystem::exists(destination, ec))
			return false;
		std::vector<std::string> lines;
		std::string line;
		while (std::getline(input, line)) {
			if (!line.empty() && line.back() == '\r')
				line.pop_back();
			if (!line.empty() && line != command_utf8)
				lines.push_back(line);
		}
		if (input.bad())
			return false;
		input.close();
		lines.push_back(command_utf8); // Most recently used command goes last.

		{
			std::ofstream output(temporary, std::ios::binary | std::ios::trunc);
			if (!output)
				return false;
			for (const auto& entry : lines)
				output << entry << '\n';
			output.flush();
			if (!output)
				return false;
		}

		// Replace only after the complete new file has been written successfully.
		if (!MoveFileExW(temporary.c_str(), destination.c_str(),
			MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
			return false;
		return true;
	}

	void cwr_helper::remove_cmd_from_file(const CString& remove_command, ccombo_completion* m_combobox)
	{
		CT2A utf8_remove(remove_command, CP_UTF8);
		const std::string remove_command_utf8(utf8_remove);
		const std::filesystem::path destination(permanent_config);
		const std::filesystem::path temporary(temporary_config);
		std::ifstream input(destination, std::ios::binary);
		if (!input)
			return;

		std::vector<std::string> lines;
		std::string line;
		while (std::getline(input, line)) {
			if (!line.empty() && line.back() == '\r')
				line.pop_back();
			if (!line.empty() && line != remove_command_utf8)
				lines.push_back(line);
		}
		if (input.bad())
			return;
		input.close();

		{
			std::ofstream output(temporary, std::ios::binary | std::ios::trunc);
			if (!output)
				return;
			for (const auto& entry : lines)
				output << entry << '\n';
			output.flush();
			if (!output)
				return;
		}
		if (MoveFileExW(temporary.c_str(), destination.c_str(),
			MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
			load_cmd_from_file(m_combobox);
	}

}

