#include "main.hpp"

#include <nana/gui.hpp>
#include <nana/gui/msgbox.hpp>
#include <spdlog/spdlog.h>

#include "application.hpp"

int main(int argc, char** argv) {
  auto context = svc_context();

  // Initialize GUI. The ctor will internally register various handlers
  application app{context};
  app.show();

  // Start context worker.
  context->start();

  // Launch GUI thread. This will block precess until the last window is closed.
  nana::exec();

  // Cleanup context.
  context->wait_stop();
  return 0;
}

perfkit::dashboard::context* svc_context() {
  static auto _inst{[] {
    auto instance = std::make_unique<perfkit::dashboard::context>();
    instance->on_relogin_required.add(
            [](perfkit::dashboard::context* context) {
              // Create login form.
              // As this is registered at relogin notifier, this login modal will always be shown
              //  whenever the context is invalidated.
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

                context->login(lg);
              } else {
                context->stop();
              }
            });
    return instance;
  }()};

  return _inst.get();
}
