//
// Created by ki608 on 2021-11-08.
//

#pragma once
#include <core/context.hpp>
#include <nana/gui/widgets/form.hpp>

class application : public nana::form {

private:
  perfkit::dashboard::context _context;
};
