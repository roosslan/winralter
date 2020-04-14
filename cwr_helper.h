#pragma once
#include "combo_completion.h"

namespace winralter {
	std::string get_config_file_path(int config_file_type);
	inline std::string permanent_config = get_config_file_path(0);
	inline std::string temporary_config = get_config_file_path(1);

	class cwr_helper {
	public:
		static void remove_cmd_from_file(const CString& remove_command, ccombo_completion* m_combobox);
		static void load_cmd_from_file(ccombo_completion*m_combobox);
		static bool save_cmd_to_file(const CString& cmd_to_save);
		static bool execute_file(const ccombo_completion* m_combobox);
		static void browse_file(ccombo_completion* m_combobox);
	};

}