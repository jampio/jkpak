#include "Config.h"
#include "util.h"
#include "File.h"
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

const std::string config_filename = jkpak::config_path() + jkpak::path_sep() + "config.json";

jkpak::Config jkpak::Config::load() {
	try {
		auto file = File::open(config_filename, "r");
		auto contents = file.contents();
		json cfg = json::parse(contents);
		std::string install_path = cfg["install_path"];
		return Config(std::move(install_path));
	} catch (...) {
		auto e = std::current_exception();
		std::cerr << "failed opening config file" << std::endl;
		std::rethrow_exception(e);
	}
}

void jkpak::Config::set_install_path(std::string_view install_path) {
	mkdir(config_path());
	auto file = File::open(config_filename, "w+");
	auto contents = std::string("{\n    \"install_path\": ") + quote(escape_path(install_path)) + "\n}";
	file.write(contents);
}