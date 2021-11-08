project(perfkit-dashboard-corelib)

add_library(
        ${PROJECT_NAME}

        STATIC
        context.cpp
)

add_library(
        perfkit::dashboard-core
        ALIAS
        ${PROJECT_NAME}
)

target_link_libraries(
        ${PROJECT_NAME}

        PUBLIC
        perfkit::libcurl
        spdlog::spdlog
        perfkit::json
)


target_include_directories(
        ${PROJECT_NAME}

        PUBLIC
        include
)

target_compile_features(
        ${PROJECT_NAME}

        PUBLIC
        cxx_std_17
)
