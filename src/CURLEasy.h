#pragma once

#include <curl/curl.h>
#include <stdexcept>
#include <optional>
#include <string>

namespace jkpak {
class CURLEasy {
private:
	CURL *handle;
	CURLEasy(CURL *handle)
		: handle(handle)
	{}
public:
	typedef std::size_t (*write_callback)(void *ptr, std::size_t size, std::size_t nmemb, void *userdata);
	typedef int (*progress_callback)(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow);
	typedef std::size_t (*header_callback)(char *buffer, std::size_t size, std::size_t nitems, void *userdata); 
	CURLEasy(const CURLEasy&) = delete;
	CURLEasy() = delete;
	CURLEasy(CURLEasy&& other)
		: handle(other.handle) {
		other.handle = nullptr;
	}
	CURLEasy& operator=(const CURLEasy&) = delete;
	CURLEasy& operator=(CURLEasy&&) = delete;
	~CURLEasy() {
		if (handle != nullptr) {
			curl_easy_cleanup(handle);
			handle = nullptr;
		}	
	}
	static CURLEasy init() {
		CURL *new_handle = curl_easy_init();
		if (new_handle == nullptr) {
			throw std::runtime_error("curl_easy_init failed");
		}
		return CURLEasy{new_handle};
	}
	template <typename... Param>
	void setopt(CURLoption option, Param... param) {
		CURLcode res = curl_easy_setopt(handle, option, param...);
		if (res != CURLE_OK) {
			throw std::runtime_error(curl_easy_strerror(res));
		}
	}
	template <typename... Param>
	void getinfo(CURLINFO option, Param... param) {
		CURLcode res = curl_easy_getinfo(handle, option, param...);
		if (res != CURLE_OK) {
			throw std::runtime_error(curl_easy_strerror(res));
		}
	}
	void set_url(std::string_view url) {
		setopt(CURLOPT_URL, url.data());
	}
	void set_writedata(void *userdata) {
		setopt(CURLOPT_WRITEDATA, userdata);
	}
	void set_writefunction(write_callback callback) {
		setopt(CURLOPT_WRITEFUNCTION, callback);
	}
	void set_progressfunction(progress_callback callback) {
		setopt(CURLOPT_PROGRESSFUNCTION, callback);
	}
	void set_noprogress(bool disable = true) {
		setopt(CURLOPT_NOPROGRESS, disable ? 1L : 0L);
	}
	void set_nobody(bool disable = true) {
		setopt(CURLOPT_NOBODY, disable ? 1L : 0L);
	}
	void set_headerdata(void *userdata) {
		setopt(CURLOPT_HEADERDATA, userdata);
	}
	void set_headerfunction(header_callback callback) {
		setopt(CURLOPT_HEADERFUNCTION, callback);
	}
	void set_followlocation(bool enable = true) {
		setopt(CURLOPT_FOLLOWLOCATION, enable ? 1L : 0L);
	}
	long get_response_code() {
		long code;
		getinfo(CURLINFO_RESPONSE_CODE, &code);
		return code;
	}
	bool is_response_ok() {
		auto code = get_response_code();
		return code >= 200 && code < 300;
	}
	std::optional<std::string> get_effective_url() {
		char *url;
		getinfo(CURLINFO_EFFECTIVE_URL, &url);
		if (url == nullptr) return {};
		return url;
	}
	void perform() {
		CURLcode res = curl_easy_perform(handle);
		if (res != CURLE_OK) {
			throw std::runtime_error(curl_easy_strerror(res));
		}
	}
};
}