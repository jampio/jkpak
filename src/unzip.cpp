#include "unzip.h"
#include <Poco/Zip/Decompress.h>
#include <Poco/FileStream.h>

void jkpak::unzip(std::string_view filename, std::string_view destination) {
	Poco::FileInputStream i(filename.data(), std::ios::binary);
	constexpr auto flattenDirs = true;
	Poco::Zip::Decompress dec(i, destination.data(), flattenDirs);
	dec.decompressAllFiles();
}