#include "util.h"
#include <cstdlib>
#include <stdexcept>
#include <cstring>
#include "Dir.h"
#include <cctype>

const std::string& jkpak::path_sep() noexcept {
#if defined(__linux__)
	static std::string sep = "/";
	return sep;
#else
#	error "jkpak::path_sep() not implemented for platform"
#endif
}

std::optional<std::string> jkpak::env(std::string_view var_name) noexcept {
	if (auto var = getenv(var_name.data())) return var;
	return {};
}

const std::string& jkpak::config_path() {
#if defined(__linux__)
	static std::string path = 
		env("XDG_CONFIG_HOME").value_or(env("HOME").value() + "/.config") + "/jkpak";
	return path;
#else
#	error "jkpak::config_path() not implemented for platform"
#endif
}

const std::string& jkpak::tmp_path() noexcept {
#if defined(__linux__)
	static std::string path = "/tmp/jkpak";
	return path;
#else
#	error "jkpak::tmp_path() not implemented for platform"
#endif
}

void jkpak::exec(std::string_view cmd) {
	auto res = std::system(cmd.data());
	if (res == EXIT_FAILURE) throw std::runtime_error(std::strerror(errno));
}

std::string jkpak::quote(std::string_view s) {
	return std::string("\"") + s.data() + "\"";
}

void jkpak::mkdir(std::string_view path) {
#if defined(__linux__)
	exec(std::string("mkdir -p ") + quote(path));
#else
#	error "jkpak::mkdir() not implemented for platform"
#endif
}

void jkpak::cp(std::string_view filename, std::string_view destination) {
#if defined(__linux__)
	exec(std::string("cp ") + quote(filename) + " " + quote(destination));
#else
#	error "jkpak::cp() not implemented for platform"
#endif
}

void jkpak::unzip(std::string_view filename, std::string_view destination) {
#if defined(__linux__)
	exec(std::string("unzip ") + quote(filename) + " -d " + quote(destination));
#else
#	error "jkpak::unzip() not implemented for platform"
#endif
}

bool jkpak::ends_with(std::string_view str, std::string_view end) {
	auto npos = std::string_view::npos;
	return str.size() >= end.size() && str.compare(str.size() - end.size(), npos, end) == 0;
}

bool jkpak::starts_with(std::string_view str, std::string_view start) {
	return str.size() >= start.size() && str.compare(0, start.size(), start) == 0;
}

void jkpak::foreach_pk3(std::string_view path, std::function<void(const char *filepath)> callback) {
#if defined(__linux__)
	auto dir = Dir::open(path.data());
	for (auto entry = dir.read(); entry.has_value(); entry = dir.read()) {
		auto filename = entry.value()->d_name;
		if (ends_with(filename, ".pk3")
		    && !blacklisted(filename)) {
			callback(filename);
		}
	}
#else
#	error "jkpak::foreach_pk3 not implemented for platform"
#endif
}

void jkpak::rmdir(std::string_view path) {
#if defined(__linux__)
	exec(std::string("rm -rf ") + quote(path));
#else
#	error "jkpak::rmdir() not implemented for platform"
#endif
}

void jkpak::rm(std::string_view file) {
#if defined(__linux__)
	exec(std::string("rm ") + quote(file));
#else
#	error "jkpak::rm() not implemented for platform"
#endif
}

std::string jkpak::clean_filename(std::string_view filename) {
	return filter(filename, [](auto ch) {
		return ch == '"' || ch == '/' || ch == '\\' || std::isspace(ch) || std::iscntrl(ch);
	});
}

std::string jkpak::tolower(std::string_view str) {
	std::string res;
	for (auto c: str) {
		res.push_back(std::tolower(c));
	}
	return res;
}

std::size_t jkpak::casefind(std::string_view str, std::string_view pat) {
	auto npos = std::string_view::npos;
	if (auto pos = str.find(pat); pos != npos) {
		return pos;
	}
	if (auto pos = str.find(tolower(pat)); pos != npos) {
		return pos;
	}
	return npos;
}

bool jkpak::blacklisted(std::string_view pak) {
	static const char *blacklist[] = {
		"assets0.pk3",
		"assets1.pk3",
		"assets2.pk3",
		"assets3.pk3"
	};
	for (auto file: blacklist) {
		if (ends_with(pak, file)) return true;
	}
	return false;
}