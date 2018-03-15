#pragma once

#include <string_view>

namespace jkpak {
	void unzip(std::string_view filename, std::string_view destination);
}