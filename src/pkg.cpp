#include "Pkg.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

auto repo = R"(
	{
		"rj_sublime3": {"url": "https://mrwonko.de/jk3files/files/22040_rj_sublime3-academy.zip"},
		"countryacademy": {"url":"https://files.jkhub.org/jka/maps/ffa/countryacademy.zip"},
		"ffa_country2": {"url":"https://files.jkhub.org/jko/maps/ffa/ffa_country2.zip"}
	}
)"_json;

std::optional<jkpak::Pkg> jkpak::Pkg::find(std::string_view pkg_name) {
	if (repo.find(pkg_name.data()) == repo.end()) {
		return {};
	}
	std::string url = repo[pkg_name.data()]["url"];
	return Pkg(std::move(url));
}