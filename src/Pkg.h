#pragma once

#include <string>
#include <utility>
#include <optional>
#include <string_view>

namespace jkpak {
class Pkg {
private:
	std::string m_url;
	Pkg(std::string url)
		: m_url(std::move(url))
	{}
public:
	Pkg() = delete;
	Pkg(const Pkg&) = delete;
	Pkg(Pkg&&) = default;
	Pkg& operator=(const Pkg&) = delete;
	Pkg& operator=(Pkg&&) = delete;
	constexpr const std::string& url() const noexcept {
		return m_url;
	}
	static std::optional<Pkg> find(std::string_view pkg_name);
	static void update();
};
}