#include <core/curl.hpp>
#include <curl/curl.h>
#include <curl/easy.h>
#include <spdlog/spdlog.h>

namespace {
struct INTERNAL_curl_init_t {
  INTERNAL_curl_init_t() noexcept { curl_global_init(CURL_GLOBAL_ALL); }
} INTERNAL_curl_init;
}  // namespace

size_t perfkit::dashboard::curl::instance::_curl_on_read(void* buf, size_t size, size_t nmemb, void* user) {
  SPDLOG_TRACE("curl read: {} bytes, {} nmemb", size, nmemb);
  ((instance*)user)->_reused.append((char const*)buf, size * nmemb);
  return size * nmemb;
}
perfkit::dashboard::curl::instance::instance() noexcept {
  _curl.reset(curl_easy_init());
  curl_easy_setopt(_curl.get(), CURLOPT_WRITEFUNCTION, &_curl_on_read);
  curl_easy_setopt(_curl.get(), CURLOPT_WRITEDATA, this);
  _errbuf.resize(CURL_ERROR_SIZE);
  curl_easy_setopt(_curl.get(), CURLOPT_ERRORBUFFER, _errbuf.data());
}

std::string const* perfkit::dashboard::curl::instance::_fetch(std::string const& uri) {
  curl_easy_setopt(_curl.get(), CURLOPT_URL, uri.c_str());

  auto now = std::chrono::steady_clock::now();
  _reused.clear();
  _errc        = curl_easy_perform(_curl.get());
  auto elapsed = std::chrono::steady_clock::now() - now;
  _latency     = elapsed;

  if (_errc != CURLE_OK) {
    SPDLOG_WARN("cURL error: ({}) {}", strerr(), errdetail());
    return &_reused;
  }

  return &_reused;
}

char const* perfkit::dashboard::curl::instance::strerr() const noexcept {
  return curl_easy_strerror((CURLcode)_errc);
}

char const* perfkit::dashboard::curl::instance::errdetail() const noexcept {
  return _errbuf.c_str();
}

void perfkit::dashboard::curl::_release_curl_handle::operator()(
        perfkit::dashboard::curl::_release_curl_handle::pointer p) noexcept {
  curl_easy_cleanup(p);
}
