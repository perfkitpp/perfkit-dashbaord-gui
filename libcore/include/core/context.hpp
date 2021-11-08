#pragma once
#include <memory>
#include <stdexcept>
#include <string>

#include <common/delegate.hxx>
#include <nlohmann/json_fwd.hpp>

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

struct session_info {
  std::string name;
  std::string ip;
  int64_t pid;
  std::string machine_name;
  int64_t epoch;
  std::string description;

  friend void from_json(nlohmann::json const& j, session_info& o);
};

class context {
 public:
  using login_type = context_login_t;

 public:
  context();
  ~context();

 public:
  void poll();

  bool valid() const noexcept;
  void login(login_type const& args);

  common::delegate<int64_t, session_info const&> session_registered;
  common::delegate<int64_t> session_unregistered;

 private:
  class impl;
  std::unique_ptr<impl> self;
};

}  // namespace perfkit::dashboard
