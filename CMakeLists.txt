cmake_minimum_required(VERSION 3.20)
project(perfkit-dashboard-gui)

add_compile_options("/FIiso646.h" /wd4819 /utf-8)

add_subdirectory(third/nana EXCLUDE_FROM_ALL)
add_subdirectory(third/spdlog-1.9.2 EXCLUDE_FROM_ALL)
add_subdirectory(third/nlohmann-json EXCLUDE_FROM_ALL)

if(MSVC)
    add_subdirectory(third/libcurl-bundled-windows)
else()
    find_package(curl CONFIG REQUIRED)
endif()

add_subdirectory(libcore)
add_subdirectory(gui)
