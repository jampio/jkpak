#include "download.h"
#include "CURLEasy.h"
#include "File.h"
#include <iostream>
#include <cmath>
#include <cstdio>
#include <optional>
#include "util.h"
#include <cctype>

static std::size_t write_proc(void *ptr, std::size_t size, std::size_t nmemb, void *userdata) {
	auto file = (jkpak::File*)userdata;
	return file->write(ptr, size, nmemb);
}

static int progress_proc(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow) {
	if (dltotal <= 0.0)  {
		return 0;
	}
	constexpr int DOTS_TOTAL = 40;
	const double frac = dlnow / dltotal;
	const auto dots = int(std::round(frac * DOTS_TOTAL));
	printf("%3d%% [", int(frac * 100));
	int dot_index = 0;
	while (dot_index++ < dots) {
		std::cout.put('=');
	}
	while (dot_index++ < DOTS_TOTAL) {
		std::cout.put(' ');
	}
	std::cout << "]\r";
	std::cout.flush();
	return 0;
}

struct HeaderData {
	std::optional<std::string> location;
	std::optional<std::string> filename;
};

static std::size_t header_callback(char *buffer, std::size_t size, std::size_t nitems, void *userdata) {
	// buffer is not null terminated
	std::string s(buffer, nitems);
	constexpr auto CD_PATTERN = "Content-Disposition: attachment; filename=";
	if (auto pos = jkpak::casefind(s, CD_PATTERN); pos != std::string::npos) {
		auto substr = s.substr(pos + strlen(CD_PATTERN));
		auto data = (HeaderData*)userdata;
		data->filename.emplace(std::move(substr));
	}
	return size * nitems;
}

static HeaderData get_headerdata(std::string_view url) {
	HeaderData data;
	auto curl = jkpak::CURLEasy::init();
	curl.set_url(url);
	curl.set_headerdata(&data);
	curl.set_headerfunction(header_callback);
	curl.set_nobody();
	curl.set_followlocation();
	curl.perform();
	if (!curl.is_response_ok()) {
		throw std::runtime_error("Response code was not 2xx in get_headerdata");
	}
	if (auto location = curl.get_effective_url(); location) {
		data.location.emplace(location.value());
	}
	return data;
}

static std::optional<std::string> try_url_filename(std::string_view url) {
	auto pos = url.rfind('/');
	if (pos == std::string_view::npos) {
		return {};
	}
	return url.substr(pos).data();
}

static std::string get_filename(std::string_view url) {
	auto data = get_headerdata(url);
	if (data.filename) {
		return data.filename.value();
	} else if (data.location) {
		if (auto res = try_url_filename(data.location.value()); res) {
			return res.value();
		} else {
			throw std::runtime_error("Could not determine filename");
		}
	} else if (auto res = try_url_filename(url); res) {
		return res.value();
	} else {
		throw std::runtime_error("Could not determine filename");
	}
}

std::string jkpak::download_ignore_ext(std::string_view url) {
	return download(url, {}, true);
}

std::string jkpak::download(std::string_view url,
                            std::optional<std::string_view> out_filepath,
                            bool ignore_ext) {
	std::string filepath;
	if (out_filepath) {
		filepath = out_filepath.value().data();
	} else {
		auto filename = jkpak::clean_filename(get_filename(url));
		std::cout << "Detected filename: \"" << filename << "\"" << std::endl;
		if (!ignore_ext && !ends_with(filename, ".pk3") && !ends_with(filename, ".zip")) {
			throw std::runtime_error("Unknown file extension");
		}
		filepath = tmp_path() + path_sep() + filename;
	}
	auto file = File::open(filepath, "wb");
	auto curl = CURLEasy::init();
	curl.set_url(url);
	curl.set_writedata(&file);
	curl.set_writefunction(write_proc);
	curl.set_noprogress(false);
	curl.set_progressfunction(progress_proc);
	curl.set_followlocation();
	curl.perform();
	if (!curl.is_response_ok()) {
		throw std::runtime_error("Response code in jkpak::download() was not 2xx");
	}
	std::cout << std::endl;
	return filepath;
}