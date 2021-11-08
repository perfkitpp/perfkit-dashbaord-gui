#include <core/context.hpp>
#include <curl/curl.h>
#include <curl/easy.h>

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
// namespace perfkit::dashboard::detail

class perfkit::dashboard::context::impl {
 public:
  bool valid() const noexcept { return false; }
  void login(login_type const& args) {
  }

 private:
  void _query(std::string* buf) {}

 private:
  detail::curl_ptr _curl;
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
