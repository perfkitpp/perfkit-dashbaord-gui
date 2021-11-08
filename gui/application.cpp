//
// Created by ki608 on 2021-11-08.
//

#include "application.hpp"

#include <core/context.hpp>
#include <spdlog/spdlog.h>

using namespace perfkit::dashboard;

application::application(perfkit::dashboard::context* context) {
  context->on_relogin_required.add([&] { _reset(); });
  context->on_session_registered.add([&](int64_t id, session_info info) {
    SPDLOG_INFO("new session: {}", info.name);
  });
}
