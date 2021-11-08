#pragma once
#include <memory>
#include <stdexcept>
#include <string>

#include <common/array_view.hxx>
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

using session_id_t = int64_t;

class context {
 public:
  using login_type = context_login_t;

 public:
  context();
  ~context();

 public:
  /**
   * execute worker
   * @param active_session
   */
  void start();

  /**
   * stop and reset worker status
   */
  void stop();
  void wait_stop();

  /**
   * select list of sessions to periodically update
   * @param session
   */
  void focus(session_id_t session, std::chrono::milliseconds interval);

  /**
   * reset fence indices of given session
   * @param session -1 indicates all sessions
   */
  void reset_cache(session_id_t session = -1);

  bool valid() const noexcept;
  void login(login_type const& args);

  /**
   * gets latest curl fetch latency.
   * @return
   */
  std::chrono::nanoseconds latest_fetch_latency() const noexcept;

  common::delegate<context*> on_relogin_required;

  common::delegate<session_id_t, session_info const&> on_session_registered;
  common::delegate<session_id_t> on_session_unregistered;

  common::delegate<session_id_t> on_new_config_category;
  common::delegate<session_id_t> on_config_update;

  common::delegate<session_id_t, std::string_view> on_receive_shell;
  common::delegate<session_id_t, std::string_view, common::array_view<std::string_view>>
          on_receive_suggests;

 private:
  class impl;
  std::unique_ptr<impl> self;
};

}  // namespace perfkit::dashboard
