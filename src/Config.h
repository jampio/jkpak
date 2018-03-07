#pragma once

#include <string>
#include <utility>

namespace jkpak {
class Config {
private:
	std::string m_install_path;
	Config(std::string install_path)
		: m_install_path(std::move(install_path))
	{}
public:
	Config() = delete;
	Config(const Config&) = delete;
	Config(Config&&) = default;
	Config& operator=(const Config&) = delete;
	Config& operator=(Config&&) = delete;
	static Config load();
	static void set_install_path(std::string_view install_path);
	constexpr const std::string& install_path() const noexcept {
		return m_install_path;
	}
};
}