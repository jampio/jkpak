#include "lib.h"
#include "CURLEasy.h"
#include "Pkg.h"
#include <iostream>
#include "util.h"
#include "Config.h"
#include "download.h"
#include "Dir.h"
#include "File.h"

class SetupDir {
public:
	~SetupDir() {
		try {
			jkpak::rmdir(jkpak::tmp_path());
		} catch (std::exception &e) {
			std::cerr << "~SetupDir() Exception caught: " << e.what() << std::endl;
		}
	}
	SetupDir() {
		jkpak::rmdir(jkpak::tmp_path());
		jkpak::mkdir(jkpak::tmp_path());
	}
};

class PathDelete {
private:
	bool m_should_delete;
	std::string m_path;
public:
	PathDelete(std::string path)
		: m_should_delete(!jkpak::File::exists(path))
		, m_path(std::move(path))
	{}
	~PathDelete() {
		if (m_should_delete) jkpak::rmdir(m_path);
	}
};

class SetupSteamDir {
private:
#if defined(__linux__)
	PathDelete path1{jkpak::env("HOME").value() + jkpak::path_sep() + ".steam"};
	PathDelete path2{jkpak::env("HOME").value() + jkpak::path_sep() + "Steam"};
#endif
};

void jkpak::cmd::install(std::string_view pkg_name) {
	SetupDir setupdir;
	auto cfg = Config::load();
	std::cout << "Using install path: " << cfg.install_path() << std::endl;
	std::string url;
	if (starts_with(pkg_name, "http")) {
		url = pkg_name;
	} else {
		auto pkg = Pkg::find(pkg_name);
		if (!pkg) throw std::runtime_error("package not found");
		url = pkg.value().url();
	}
	std::cout << "Downloading: " << url << std::endl;
	auto filename = download(url);
	if (ends_with(filename, ".pk3")) {
		std::cout << "Copying " << filename << " to " << cfg.install_path() << std::endl;
		cp(filename, cfg.install_path());
	} else if (ends_with(filename, ".zip")) {
		auto dest = tmp_path() + path_sep() + "unzip";
		mkdir(dest);
		unzip(filename, dest);
		foreach_pk3(dest, [&](auto filename) {
			std::cout << "Copying " << filename << " to " << cfg.install_path() << std::endl;
			cp(dest + path_sep() + filename, cfg.install_path());
		});
	} else {
		throw std::runtime_error("unknown file extension");
	}
}

void jkpak::cmd::set_install_path(std::string_view install_path) {
	Config::set_install_path(install_path);
}

void jkpak::cmd::list() {
	auto cfg = Config::load();
	foreach_pk3(cfg.install_path(), [](auto filename) {
		std::cout << filename << std::endl;
	});
}

void jkpak::cmd::delete_file(std::string_view pak) {
	auto cfg = Config::load();
	foreach_pk3(cfg.install_path(), [&](auto filename) {
		if (filename == pak) {
			std::cout << "Deleting " << filename << std::endl;
			rm(cfg.install_path() + path_sep() + filename);
		}
	});
}

void jkpak::cmd::update() {
	Pkg::update();
}

#ifdef __linux__
void jkpak::cmd::install_basejka_linux() {
	SetupDir setupdir;
	auto cfg = Config::load();
	std::cout << "Using install path: " << cfg.install_path() << std::endl;
	auto url = "https://files.jkhub.org/jka/official/jalinuxded_1.011.zip";
	std::cout << "Downloading " << url << std::endl;
	auto file = download(url);
	auto zip_dir = tmp_path() + path_sep() + "unzip";
	mkdir(zip_dir);
	unzip(file, zip_dir);
	auto jampgame = zip_dir + path_sep() + "jampgamei386.so";
	std::cout << "Copying " << jampgame << " to " << cfg.install_path() << std::endl;
	cp(jampgame, cfg.install_path());
	auto cmd = std::string("install ") + quote(zip_dir + path_sep() + "libcxa.so.1") + " /usr/lib32/";
	std::cout << cmd << std::endl;
	exec(cmd);
}
#endif

#if defined(__linux__)
constexpr auto STEAMCMD_URL = "http://media.steampowered.com/installer/steamcmd_linux.tar.gz";
constexpr auto STEAMCMD = "steamcmd.sh";
#elif defined(_WIN32)
constexpr auto STEAMCMD_URL = "http://media.steampowered.com/installer/steamcmd.zip";
constexpr auto STEAMCMD = "steamcmd.exe";
#endif

void jkpak::cmd::install_steam_assets() {
	SetupDir setupdir__;
	SetupSteamDir steamdir__;
	auto cfg = Config::load();
	std::cout << "Using install path: " << cfg.install_path() << std::endl;
	std::cout << "Downloading: " << STEAMCMD_URL << std::endl;
	auto file = download_ignore_ext(STEAMCMD_URL);
	auto zip_dir = tmp_path() + path_sep() + "unzip";
	mkdir(zip_dir);
#if defined(__linux__)
	exec(std::string("tar -xvzf ") + quote(file) + " -C " + quote(zip_dir));
#elif defined(_WIN32)
	unzip(file, zip_dir);
#endif
	std::cout << "Steam username: ";
	std::string username;
	std::cin >> username;
	std::cout << "Steam password: ";
	set_stdin_echo(false);
	std::string password;
	std::cin >> password;
	set_stdin_echo(true);
	auto steam_path = tmp_path() + path_sep() + "SteamJKA";
	auto cmd = zip_dir + path_sep() + STEAMCMD + " "
	         + "+@sSteamCmdForcePlatformType windows "
	         + "+login " + quote(username) + " " + quote(password) + " "
	         + "+force_install_dir " + quote(steam_path) + " "
	         + "+app_update 6020 validate +quit"
	         ;
	// steamcmd does not give reliable exit codes
	auto _ignored_ = std::system(cmd.c_str());
	auto base_path = steam_path + path_sep() + "GameData" + path_sep() + "base";
	for (auto i = 0; i <= 3; i++) {
		auto asset_path = base_path + path_sep() + "assets" + std::to_string(i) + ".pk3";
		std::cout << "Copying " << asset_path << " to " << cfg.install_path() << std::endl;
		cp(asset_path, cfg.install_path());
	}
}