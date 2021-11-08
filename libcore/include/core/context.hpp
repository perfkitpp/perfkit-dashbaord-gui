#pragma once
#include <memory>
#include <stdexcept>
#include <string>

namespace perfkit::dashboard {
struct context_login_t {
  std::string url;
  std::string id;
  std::string password;
};

struct context_error : std::exception {
  const char* what() const override { return _m.c_str(); }
  context_error& message(std::string msg) {
    _m = std::exception::what() + (": " + std::move(msg));
    return *this;
  }

 private:
  std::string _m = std::exception::what();
};

struct context_login_error : context_error {};
struct context_invalid_url : context_login_error {};
struct context_invalid_auth : context_login_error {};

class context {
 public:
  using login_type = context_login_t;

 public:
  context();
  ~context();

 public:
  bool valid() const noexcept;
  void login(login_type const& args);

 private:
  class impl;
  std::unique_ptr<impl> self;
};

}  // namespace perfkit::dashboard
