#pragma once

#include <string>
#include <string_view>
#include <optional>

namespace jkpak {
	std::string download(std::string_view url,
	                     std::optional<std::string_view> out_filepath = {},
	                     bool ignore_ext = false);
	std::string download_ignore_ext(std::string_view url);
}