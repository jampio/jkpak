#include "lib.h"
#include "CURLEasy.h"
#include "Pkg.h"
#include <iostream>
#include "util.h"
#include "Config.h"
#include "download.h"

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