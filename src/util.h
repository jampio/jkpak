#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <functional>

namespace jkpak {
	std::optional<std::string> env(std::string_view var_name) noexcept;
	const std::string& config_path();
	const std::string& path_sep() noexcept;
	const std::string& tmp_path() noexcept;
	void exec(std::string_view cmd);
	void mkdir(std::string_view path);
	std::string quote(std::string_view s);
	void cp(std::string_view filename, std::string_view destination);
	void unzip(std::string_view filename, std::string_view destination);
	void foreach_pk3(std::string_view path, std::function<void(const char *filepath)> callback);
	bool ends_with(std::string_view str, std::string_view end);
	bool starts_with(std::string_view str, std::string_view start);
	void rmdir(std::string_view path);
	void rm(std::string_view file);
	std::string clean_filename(std::string_view filename);
	std::string tolower(std::string_view str);
	std::size_t casefind(std::string_view str, std::string_view pat);
	std::string strerror(int code, std::string_view generic);
	std::string escape_path(std::string_view path);
	bool blacklisted(std::string_view pak);
	void set_stdin_echo(bool flag);
	template <typename Proc> 
	std::string filter(std::string_view str, Proc&& proc) noexcept {
		std::string res;
		for (auto ch: str) {
			if (!proc(ch)) {
				res.push_back(ch);
			}
		}
		return res;
	}
}