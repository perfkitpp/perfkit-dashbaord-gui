#pragma once
#include <chrono>
#include <memory>
#include <optional>
#include <string>

namespace perfkit::dashboard::curl {
struct _release_curl_handle {
  using pointer = void*;
  void operator()(pointer p) noexcept;
};

using ptr = std::unique_ptr<void, _release_curl_handle>;

class instance {
 public:
  instance() noexcept;
  void baseurl(std::string_view url) noexcept { _baseurl = url; }
  void auth(std::string const& token);

  template <typename... Args_>
  std::string const* fetch(Args_&&... args) {
    _reused = _baseurl;
    (_reused.append(std::forward<Args_>(args)), ...);
    return _fetch(_reused);
  }

  int errc() const noexcept { return _errc; }
  char const* strerr() const noexcept;
  char const* errdetail() const noexcept;

  std::chrono::nanoseconds latency() const noexcept { return _latency; }

 private:
  static size_t _curl_on_read(void* buf, size_t size, size_t nmemb, void* user);
  std::string const* _fetch(std::string const& uri);

 private:
  ptr _curl;
  std::string _baseurl;
  std::string _reused;
  std::chrono::nanoseconds _latency;

  int _errc = {};
  std::string _errbuf;
};

}  // namespace perfkit::dashboard::curl
