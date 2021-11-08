//
// Created by ki608 on 2021-11-08.
//

#pragma once
#include <core/context.hpp>
#include <nana/gui/widgets/form.hpp>

struct error_auth_failed : std::exception {};

perfkit::dashboard::context* svc_context();
