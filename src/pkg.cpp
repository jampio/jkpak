#include "Pkg.h"
#include <nlohmann/json.hpp>
#include "util.h"
#include "File.h"
#include "download.h"
#include <iostream>

using json = nlohmann::json;

const std::string repo_url = "https://raw.githubusercontent.com/jampio/jkpak-repo/master/repo.json";
const std::string repo_filepath = jkpak::config_path() + jkpak::path_sep() + "repo.json";

void jkpak::Pkg::update() {
	mkdir(config_path());
	std::cout << "Downloading: " << repo_url << " to " << repo_filepath << std::endl;
	download(repo_url, repo_filepath);
}

std::optional<jkpak::Pkg> jkpak::Pkg::find(std::string_view pkg_name) {
	if (!File::exists(repo_filepath)) {
		throw std::runtime_error("repo.json does not exist, try running jkpak update");
	}
	auto file = File::open(repo_filepath, "r");
	auto contents = file.contents();
	auto repo = json::parse(contents);
	if (repo.find(pkg_name.data()) == repo.end()) {
		return {};
	}
	std::string url = repo[pkg_name.data()]["url"];
	return Pkg(std::move(url));
}