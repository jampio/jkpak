#pragma once

#include <string_view>

namespace jkpak::cmd {
	void install(std::string_view pkg_name);
	void set_install_path(std::string_view install_path);
	void list();
	void delete_file(std::string_view pak);
	void update();
#ifdef __linux__
	void install_basejka_linux();
#endif
}