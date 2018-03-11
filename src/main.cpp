#include "lib.h"
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <iostream>

#ifdef _WIN32
constexpr auto USAGE = R"(
Usage: jkpak install <PKG_NAME>|<URL>|steam-assets
       jkpak set-install-path <PATH>
       jkpak ls|list
       jkpak rm|remove <PK3>
       jkpak update
)";
#else
constexpr auto USAGE = R"(
Usage: jkpak install <PKG_NAME>|<URL>|steam-assets|basejka
       jkpak set-install-path <PATH>
       jkpak ls|list
       jkpak rm|remove <PK3>
       jkpak update
)";
#endif

int main(int argc, const char **argv) {
	try {
		if (argc == 3 && !strcmp(argv[1], "set-install-path")) {
			jkpak::cmd::set_install_path(argv[2]);
#ifdef __linux__
		} else if (argc == 3 && !strcmp(argv[1], "install") && !strcmp(argv[2], "basejka")) {
			jkpak::cmd::install_basejka_linux();
#endif
		} else if (argc == 3 && !strcmp(argv[1], "install") && !strcmp(argv[2], "steam-assets")) {
			jkpak::cmd::install_steam_assets();
		} else if (argc == 3 && !strcmp(argv[1], "install")) {
			jkpak::cmd::install(argv[2]);
		} else if (argc == 2 && (!strcmp(argv[1], "ls") || !strcmp(argv[1], "list"))) {
			jkpak::cmd::list();
		} else if (argc == 3 && (!strcmp(argv[1], "rm") || !strcmp(argv[1], "remove"))) {
			jkpak::cmd::delete_file(argv[2]);
		} else if (argc == 2 && !strcmp(argv[1], "update")) {
			jkpak::cmd::update();
		} else {
			std::cout << USAGE << std::endl;
		}
		return EXIT_SUCCESS;
	} catch (std::exception &e) {
		std::cerr << "jkpak error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}