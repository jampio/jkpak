#pragma once

#include <string>
#include <string_view>

namespace jkpak {
	// returns absolute filename/path
	std::string download(std::string_view url);
}