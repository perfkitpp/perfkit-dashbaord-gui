//
// Created by ki608 on 2021-11-08.
//

#include "application.hpp"

#include <nana/gui/msgbox.hpp>

perfkit::dashboard::context* svc_context() {
  static perfkit::dashboard::context _inst;
  if (not _inst.valid()) {
    // create login form
    nana::inputbox login{nullptr, "Enter API Server/Auth Info", "Sign in"};
    nana::inputbox::text url{"URL", "http://"};
    nana::inputbox::text id{"ID", ""};
    nana::inputbox::text password{"Password", ""};
    password.mask_character('*');

    if (login.show(url, id, password)) {
      perfkit::dashboard::context_login_t lg;
      lg.url      = url.value();
      lg.id       = id.value();
      lg.password = password.value();

      _inst.login(lg);
    }
  }

  if (not _inst.valid())
    throw error_auth_failed{};

  return &_inst;
}
