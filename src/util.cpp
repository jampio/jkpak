#include "util.h"
#include <cstdlib>
#include <stdexcept>
#include <cstring>
#include "Dir.h"
#include <cctype>
#include <iostream>

#ifdef _WIN32
#	include <shlobj.h>
#endif

constexpr auto NT_PATH = "\\";
constexpr auto POSIX_PATH = "/";

const std::string& jkpak::path_sep() noexcept {
	// although Win32 api does translate posix paths
	// I don't want to rely on it
#if defined(_WIN32)
	static const std::string sep = NT_PATH;
#else
	static const std::string sep = POSIX_PATH;
#endif
	return sep;
}

std::optional<std::string> jkpak::env(std::string_view var_name) noexcept {
#if defined(_MSC_VER)
	char *str = nullptr;
	auto errc = _dupenv_s(&str, nullptr, var_name.data());
	if (errc == 0) {
		std::string res{str};
		free(str);
		return res;
	} else return {};
#else
	if (auto var = getenv(var_name.data())) return var;
	return {};
#endif
}

const std::string& jkpak::config_path() {
#if defined(__linux__)
	static const std::string path = 
		env("XDG_CONFIG_HOME").value_or(env("HOME").value() + "/.config") + "/jkpak";
	return path;
#elif defined(_WIN32)
	static const std::string path = 
		env("USERPROFILE").value() + path_sep() + "AppData" + path_sep() + "Local" + path_sep() + "jkpak";
	return path;
#else
#	error "jkpak::config_path() not implemented for platform"
#endif
}

#ifdef _WIN32
static std::string get_win32_tmp() {
	char buf[MAX_PATH+1];
	auto res = GetTempPathA(sizeof(buf), buf);
	if (res == 0) {
		auto errc = GetLastError();
		throw std::runtime_error(jkpak::strerror(errc, "get_win32_tmp() failed"));
	}
	// trim trailing path sep
	if (res > 0 && (buf[res-1] == '\\' || buf[res-1] == '/')) {
		buf[res-1] = 0;
	}
	return std::string{buf};
}
#endif

const std::string& jkpak::tmp_path() noexcept {
#if defined(__linux__)
	static const std::string path = "/tmp/jkpak";
	return path;
#elif defined(_WIN32)
	static const std::string path = get_win32_tmp();
	return path;
#else
#	error "jkpak::tmp_path() not implemented for platform"
#endif
}

void jkpak::exec(std::string_view cmd) {
	auto res = std::system(cmd.data());
	if (res == EXIT_FAILURE) throw std::runtime_error(jkpak::strerror(errno, "system() failed"));
}

std::string jkpak::quote(std::string_view s) {
	return std::string("\"") + s.data() + "\"";
}

void jkpak::mkdir(std::string_view path) {
#if defined(_WIN32)
	// _mkdir() and CreateDirectory are not recursive
	// COM MKDIR does not have a "-p" like option
	auto errc = SHCreateDirectoryExA(nullptr, path.data(), nullptr);
	if (errc == ERROR_SUCCESS || errc == ERROR_FILE_EXISTS || errc == ERROR_ALREADY_EXISTS) {
		return;
	} else throw std::runtime_error(jkpak::strerror(errc, "SHCreateDirectory() failed"));
#else
	exec(std::string("mkdir -p ") + quote(path));
#endif
}

void jkpak::cp(std::string_view filename, std::string_view destination) {
#if defined(_WIN32)
	exec(std::string("copy ") + quote(filename) + " " + quote(destination));
#else
	exec(std::string("cp ") + quote(filename) + " " + quote(destination));
#endif
}

void jkpak::unzip(std::string_view filename, std::string_view destination) {
	exec(std::string("unzip ") + quote(filename) + " -d " + quote(destination));
}

bool jkpak::ends_with(std::string_view str, std::string_view end) {
	auto npos = std::string_view::npos;
	return str.size() >= end.size() && str.compare(str.size() - end.size(), npos, end) == 0;
}

bool jkpak::starts_with(std::string_view str, std::string_view start) {
	return str.size() >= start.size() && str.compare(0, start.size(), start) == 0;
}

void jkpak::foreach_pk3(std::string_view path, std::function<void(const char *filepath)> callback) {
	auto dir = Dir::open(path);
	for (auto entry = dir.read(); entry.has_value(); entry = dir.read()) {
		auto filename = entry.value().filename();
		if (ends_with(filename, ".pk3")
		    && !blacklisted(filename)) {
			callback(filename);
		}
	}
}

void jkpak::rmdir(std::string_view path) {
#if defined(_WIN32)
	SHFILEOPSTRUCTA ops = {};
	// buf must be double null terminated
	// so zero-initialising whole array should be safe
	// and only copy up to N-1
	char buf[MAX_PATH] = {};
	(void) strcpy_s(buf, sizeof(buf)-1, path.data());
	ops.wFunc = FO_DELETE;
	ops.pFrom = buf;
	ops.fFlags = FOF_NO_UI;
	auto errc = SHFileOperationA(&ops);
	if (errc == 0) return;
	throw std::runtime_error(jkpak::strerror(errc, "SHFileOperationA() failed"));
#else
	exec(std::string("rm -rf ") + quote(path));
#endif
}

void jkpak::rm(std::string_view file) {
#if defined(_WIN32)
	exec(std::string("del ") + quote(file));
#else
	exec(std::string("rm ") + quote(file));
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

std::string jkpak::strerror(int code, std::string_view generic) {
#ifdef _MSC_VER
	char buf[1024];
	auto err = strerror_s(buf, code);
	std::string msg{err == 0 ? buf : generic.data()};
	return msg;
#else
	return std::strerror(code);
#endif
}

std::string jkpak::escape_path(std::string_view path) {
	std::string res;
	for (auto c: path) {
		if (c == '\\') { res.push_back('\\'); res.push_back('\\'); }
		else res.push_back(c);
	}
	return res;
}