#include <atomic>
#include <charconv>
#include <optional>
#include <unordered_set>

#include <common/poll_timer.hxx>
#include <core/context.hpp>
#include <core/curl.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using namespace std::literals;

namespace perfkit::dashboard::detail {
struct session_context {
  int64_t fence_shell   = 0;
  int64_t fence_context = 0;
  int64_t fence_trace   = 0;
  int64_t fence_images  = 0;
};
}  // namespace perfkit::dashboard::detail

namespace perfkit::dashboard::detail {
struct if_session_fetch {
  std::map<std::string, session_info> sessions;

  friend void from_json(nlohmann::json const& j, if_session_fetch& o) {
    j.at("sessions").get_to(o.sessions);
  }
};
}  // namespace perfkit::dashboard::detail

void perfkit::dashboard::from_json(const nlohmann::json& j, perfkit::dashboard::session_info& o) {
  j.at("name").get_to(o.name);
  j.at("ip").get_to(o.ip);
  j.at("pid").get_to(o.pid);
  j.at("machine_name").get_to(o.machine_name);
  j.at("epoch").get_to(o.epoch);
  j.at("description").get_to(o.description);
}

class perfkit::dashboard::context::impl {
  template <typename Dst_, typename... Args_>
  std::optional<Dst_> _fetch_as(Args_&&... args) {
    auto pstr        = _curl.fetch(std::forward<Args_>(args)...);
    _delay_ns_latest = _curl.latency().count();

    if (not pstr)
      return {};

    auto json = nlohmann::json::parse(*pstr, nullptr, false);
    if (json.is_discarded())
      return {};

    try {
      return json.template get<Dst_>();
    } catch (std::exception& e) {
      SPDLOG_ERROR("json parse error: {} ... content was: {}", e.what(), json.dump(2));
      return {};
    }
  }

 public:
  void start() {
    wait_stop();

    _worker_active.store(true);
    _worker = std::thread(&impl::_worker_loop, this);
  }
  void stop() {
    _worker_active.store(false);
  }
  void wait_stop() {
    stop();

    if (_worker.joinable())
      _worker.join();
  }

  bool valid() const noexcept {
    std::lock_guard _{_worker_lock};
    return _valid();
  }

  void login(login_type const& args) {
    std::lock_guard _{_worker_lock};

    _login = args;
    _curl.baseurl(_login.url);

    _auth_token = "ok";
  }

  void reset_cache(session_id_t session) {
    std::lock_guard _{_worker_lock};

    if (session == -1)
      _sessions.clear();
    else
      _sessions.erase(session);
  }

 private:
  void _worker_loop() {
    while (_worker_active) {
      if (not _intervals.minimal()) {
        std::this_thread::sleep_for(10ms);
        continue;
      }

      std::lock_guard _{_worker_lock};
      if (not _valid()) {
        SPDLOG_WARN("context still invalid! relogin required ...");
        _owner->on_relogin_required.invoke(_owner);
        _sessions.clear();  // clears all cache as new login possibly generated.u
        continue;
      }

      // poll sessions
      if (_intervals.sessions()) {
        auto sessions = _fetch_as<detail::if_session_fetch>("/sessions");
        if (sessions)
          _refresh_sessions(std::move(*sessions));
        else {
          SPDLOG_WARN("invalid fetch result from /sessions");
          _invalidate();
          continue;
        }
      }

      // poll shell/configs/traces/images of focusing session
    }
  }

  bool _valid() const noexcept {
    return not _auth_token.empty();
  }

  void _invalidate() noexcept {
    _auth_token.clear();
  }

  void _refresh_sessions(detail::if_session_fetch&& sess) {
    std::unordered_set<session_id_t> sessions;
    sessions.reserve(_sessions.size());
    std::transform(_sessions.begin(), _sessions.end(),
                   std::inserter(sessions, sessions.end()),
                   [](auto&& i) { return i.first; });

    for (auto& [key, info] : sess.sessions) {
      session_id_t session_id;
      if (std::from_chars(key.data(), key.data() + key.size(), session_id).ec != std::errc{}) {
        SPDLOG_WARN("invalid message format: key {} cannot be converted into number", key);
        continue;
      }

      if (_sessions.find(session_id) == _sessions.end()) {
        _sessions[session_id] = {};
        _owner->on_session_registered.invoke(session_id, info);
      }

      sessions.erase(session_id);
    }

    for (auto erased_id : sessions) {
      _sessions.erase(erased_id);
      _owner->on_session_unregistered.invoke(erased_id);
    }
  }

 public:
  context* _owner;

 private:
  curl::instance _curl;
  std::string _auth_token;
  std::string _reused_str;

  std::atomic_bool _worker_active;
  std::thread _worker;
  mutable std::recursive_mutex _worker_lock;

  std::atomic_int64_t _delay_ns_latest;
  std::unordered_map<session_id_t, detail::session_context> _sessions;

  struct intervals_t {
    common::poll_timer minimal{10ms};
    common::poll_timer sessions{3000ms};
    common::poll_timer shell{100ms};
    common::poll_timer configs{500ms};
    common::poll_timer trace{200ms};
    common::poll_timer image{80ms};
  } _intervals;
  login_type _login;
};

perfkit::dashboard::context::context() : self(std::make_unique<impl>()) {
  self->_owner = this;
}

perfkit::dashboard::context::~context() {
  self->wait_stop();
}

bool perfkit::dashboard::context::valid() const noexcept {
  return self->valid();
}
void perfkit::dashboard::context::login(
        const perfkit::dashboard::context::login_type& args) {
  self->login(args);
}
void perfkit::dashboard::context::start() {
  self->start();
}
void perfkit::dashboard::context::stop() {
  self->stop();
}

void perfkit::dashboard::context::reset_cache(perfkit::dashboard::session_id_t session) {
  self->reset_cache(session);
}
void perfkit::dashboard::context::wait_stop() {
  self->wait_stop();
}
