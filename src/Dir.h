#pragma once

#include <dirent.h>
#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <optional>

namespace jkpak {
class Dir {
private:
	DIR *handle;
	Dir(DIR *handle)
		: handle(handle)
	{}
public:
	Dir() = delete;
	Dir(const Dir&) = delete;
	Dir(Dir&& other)
		: handle(other.handle)
	{
		other.handle = nullptr;
	}
	~Dir() {
		if (handle != nullptr) {
			closedir(handle);
			handle = nullptr;
		}
	}
	Dir& operator=(const Dir&) = delete;
	Dir& operator=(Dir&&) = delete;
	static Dir open(const char *path) {
		auto res = opendir(path);
		if (res == nullptr) {
			throw std::runtime_error(std::strerror(errno));
		}
		return Dir{res};
	}
	std::optional<const dirent*> read() {
		auto res = readdir(handle);
		if (res == nullptr) {
			return {};
		} else {
			return std::make_optional(res);
		}
	}
	void rewind() {
		rewinddir(handle);
	}
};
}