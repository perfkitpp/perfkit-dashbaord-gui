project(perfkit-dashboard)

add_executable(
        ${PROJECT_NAME}

        main.cpp
)

target_link_libraries(
        ${PROJECT_NAME}

        PRIVATE
        nana::nana
        perfkit::dashboard-core
)
