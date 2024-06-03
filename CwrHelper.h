#pragma once
#include "ComboCompletion.h"

namespace winralter
{
	class CwrHelper
	{
	public:
		static constexpr unsigned char PermanentConfig = 0;
		static constexpr unsigned char TemporaryConfig = 1;
		static std::string GetConfigFilePath(int ConfigFileType);
		static void RemoveCmdFromFile(std::string removeCommand, CComboCompletion* m_ComboBox);
		static void LoadCmdFromFile(CComboCompletion *m_ComboBox);
		static bool SaveCmdToFile(CString cmdToSave); //CComboCompletion* m_ComboBox);
		static bool ExecuteFile(CComboCompletion* m_ComboBox);
		static void BrowseFile(CComboCompletion* m_ComboBox);
	};

}