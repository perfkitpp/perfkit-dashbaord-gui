#pragma once
#include <string>

namespace perfkit::dashboard {
struct context_login_t {
  std::string url;
  std::string id;
  std::string password;
};

class context {
public:
  using login_type = context_login_t;

public:
  void login(login_type const &args);
};

} // namespace perfkit::dashboard
