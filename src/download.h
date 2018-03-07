#pragma once

#include <string>
#include <string_view>
#include <optional>

namespace jkpak {
	// returns absolute filename/path
	std::string download(std::string_view url, std::optional<std::string_view> out_filepath = {});
}