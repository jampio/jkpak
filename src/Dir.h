#pragma once

#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <optional>
#include <string_view>
#include <string>

#if _POSIX_C_SOURCE >= 200809L
#	include <dirent.h>
#	define CLOSE_DIR closedir
#	define USE_WIN32_DIR 0
#	define NULL_DIR_HANDLE nullptr
#elif defined(_WIN32)
#	include <Windows.h>
#	define CLOSE_DIR FindClose
#	define USE_WIN32_DIR 1
#	define NULL_DIR_HANDLE INVALID_HANDLE_VALUE
#else
#	error "Unsupported platform"
#endif

namespace jkpak {
class DirEnt {
private:
#if USE_WIN32_DIR
	WIN32_FIND_DATAA m_entry;
#else
	dirent *m_entry;
#endif
public:
	DirEnt() = delete;
	DirEnt(const DirEnt&) = default;
	DirEnt(decltype(m_entry) entry)
		: m_entry(entry)
	{}
	const char *filename() const {
#if USE_WIN32_DIR
		return m_entry.cFileName;
#else
		return m_entry->d_name;
#endif
	}
};

class Dir {
private:
#if USE_WIN32_DIR
	using DirHandle = HANDLE;
	std::optional<WIN32_FIND_DATAA> first_find;
#else
	using DirHandle = DIR*;
#endif
	DirHandle handle;
	Dir(DirHandle handle)
		: handle(handle)
#if USE_WIN32_DIR
		, first_find()
#endif
	{}
public:
	Dir() = delete;
	Dir(const Dir&) = delete;
	Dir(Dir&& other)
		: handle(other.handle)
#if USE_WIN32_DIR
		, first_find(std::move(other.first_find))
#endif
	{
		other.handle = NULL_DIR_HANDLE;
	}
	~Dir() {
		if (handle != NULL_DIR_HANDLE) {
			(void) CLOSE_DIR(handle);
			handle = NULL_DIR_HANDLE;
		}
	}
	Dir& operator=(const Dir&) = delete;
	Dir& operator=(Dir&&) = delete;
	static Dir open(std::string_view path) {
#if USE_WIN32_DIR
		// Need \* to examine directory contents
		std::string sPath = std::string(path) + "\\*";
		WIN32_FIND_DATAA find_data;
		auto res = FindFirstFileA(sPath.c_str(), &find_data);
		if (res == NULL_DIR_HANDLE) {
			auto errc = GetLastError();
			throw std::runtime_error(jkpak::strerror(errc, "FindFirstFile() failed"));
		}
		Dir dir{res};
		dir.first_find.emplace(find_data);
		return dir;
#else
		auto res = opendir(path.data());
		if (res == NULL_DIR_HANDLE) {
			throw std::runtime_error(jkpak::strerror(errno, "opendir() failed"));
		}
		return Dir{res};
#endif
	}
	std::optional<DirEnt> read() {
#if USE_WIN32_DIR
		if (first_find) {
			DirEnt ent{first_find.value()};
			first_find.reset();
			return ent;
		}
		WIN32_FIND_DATAA data;
		auto res = FindNextFileA(handle, &data);
		if (res == 0) {
			auto errc = GetLastError();
			if (errc == ERROR_NO_MORE_FILES) {
				return {};
			}
			throw std::runtime_error(jkpak::strerror(errc, "FindNextFile() failed"));
		}
		return DirEnt{data};
#else
		errno = 0;
		auto res = readdir(handle);
		if (errno != 0) {
			throw std::runtime_error(jkpak::strerror(errno, "readdir() failed"));
		} else if (res == nullptr) {
			return {};
		} else {
			return std::make_optional(DirEnt{res});
		}
#endif
	}
};
}