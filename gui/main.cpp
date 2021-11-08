#include <nana/gui.hpp>

#include "application.hpp"

int main(int argc, char** argv) {
  auto context = svc_context();
  context->start();

  getchar();
  context->stop();
  return 0;
}
