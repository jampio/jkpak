#pragma once

#include <cstdio>
#include <cstring>
#include <cerrno>
#include <stdexcept>
#include <string>
#include <string_view>
#include <memory>

namespace jkpak {
class File {
private:
	std::FILE *handle;
	File(std::FILE *handle)
		: handle(handle)
	{}
public:
	File(const File&) = delete;
	File() = delete;
	File(File&& other)
		: handle(other.handle) {
		other.handle = nullptr;
	}
	File& operator=(const File&) = delete;
	File& operator=(File&&) = delete;
	~File() {
		if (handle != nullptr) {
			fclose(handle);
			handle = nullptr;
		}
	}
	bool error() {
		auto res = std::ferror(handle);
		return res != 0;
	}
	bool eof() {
		auto res = std::feof(handle);
		return res != 0;
	}
	static File open(std::string_view filename, std::string_view mode) {
		auto fp = std::fopen(filename.data(), mode.data());
		if (fp == nullptr) {
			throw std::runtime_error(std::strerror(errno));
		}
		return File{fp};
	}
	std::size_t write(const void *buffer, std::size_t size, std::size_t count) {
		auto res = std::fwrite(buffer, size, count, handle);
		if (error()) throw std::runtime_error(std::strerror(errno));
		return res;
	}
	std::size_t write(std::string_view str) {
		return write(str.data(), sizeof(char), str.size());
	}
	std::size_t read(void *buffer, std::size_t size, std::size_t count) {
		auto res = std::fread(buffer, size, count, handle);
		if (error()) throw std::runtime_error(std::strerror(errno));
		return res;

	}
	void seek(long offset, int origin) {
		auto res = std::fseek(handle, offset, origin);
		if (res != 0) {
			throw std::runtime_error(std::strerror(errno));
		}
	}
	long tell() {
		auto res = ftell(handle);
		if (res == -1L) {
			throw std::runtime_error(std::strerror(errno));
		}
		return res;
	}
	void rewind() {
		std::rewind(handle);
	}
	std::string contents() {
		seek(0, SEEK_END);
		auto size = tell();
		rewind();
		std::unique_ptr<char[]> buf = std::make_unique<char[]>(size);
		auto res = read(buf.get(), sizeof(char), size);
		if (res != size) {
			throw std::runtime_error("contents() failed, size of read did not match");
		}
		return std::string(buf.get(), size);
	}
};
}