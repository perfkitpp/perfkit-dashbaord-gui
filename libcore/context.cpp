#include <core/context.hpp>
#include <curl/curl.h>
#include <curl/easy.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace {
struct INTERNAL_curl_init_t {
  INTERNAL_curl_init_t() noexcept { curl_global_init(CURL_GLOBAL_ALL); }
} INTERNAL_curl_init;
}  // namespace

namespace perfkit::dashboard::detail {
struct _release_curl_handle {
  using pointer = CURL*;
  void operator()(pointer p) noexcept { curl_easy_cleanup(p); }
};

using curl_ptr = std::unique_ptr<CURL, _release_curl_handle>;
}  // namespace perfkit::dashboard::detail

void perfkit::dashboard::from_json(const nlohmann::json& j, perfkit::dashboard::session_info& o) {
  o.name         = j.value("name", "");
  o.ip           = j.value("ip", "");
  o.pid          = j.value("pid", 0);
  o.machine_name = j.value("machine_name", "");
  o.epoch        = j.value("epoch", 0);
  o.description  = j.value("description", "");
}

class perfkit::dashboard::context::impl {
 private:
  template <typename... Args_>
  char const* _url(Args_&&... args) {
    _reused_str = _login.url;
    (_reused_str.append(std::forward<Args_>(args)), ...);
    return _reused_str.c_str();
  }

  template <typename... Args_>
  std::string const& _fetch(Args_&&... args) {
    curl_easy_setopt(_curl.get(), CURLOPT_URL, this->_url(std::forward<Args_>(args)...));

    _reused_str.clear();
    curl_easy_perform(_curl.get());
    return _reused_str;
  }

 public:
  bool valid() const noexcept { return not _auth_token.empty(); }
  void login(login_type const& args) {
    _login = args;
    _curl.reset();
    _validate_curl();

    // TODO: implement valid login
    _auth_token = "hell!";

  }

 private:
  void _validate_curl() {
    if (not _curl.get()) {
      _curl.reset(curl_easy_init());
      curl_easy_setopt(_curl.get(), CURLOPT_WRITEFUNCTION, &_curl_on_read);
      curl_easy_setopt(_curl.get(), CURLOPT_WRITEDATA, this);
    }
  }

  static size_t _curl_on_read(void* buf, size_t size, size_t nmemb, void* user) {
    SPDLOG_TRACE("curl read: {} bytes, {} nmemb", size, nmemb);
    ((impl*)user)->_reused_str.append((char const*)buf, size * nmemb);
    return size;
  }

 private:
  detail::curl_ptr _curl;
  std::string _auth_token;

  std::string _reused_str;

  login_type _login;
};

perfkit::dashboard::context::context() : self(std::make_unique<impl>()) {}
perfkit::dashboard::context::~context() = default;

bool perfkit::dashboard::context::valid() const noexcept {
  return self->valid();
}
void perfkit::dashboard::context::login(
        const perfkit::dashboard::context::login_type& args) {
  self->login(args);
}
