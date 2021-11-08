project(perfkit-dashboard)

add_executable(
        ${PROJECT_NAME}

        main.cpp
        application.cpp
)

target_link_libraries(
        ${PROJECT_NAME}

        PRIVATE
        nana::nana
        perfkit::dashboard-core
)

set_target_properties(
        ${PROJECT_NAME}

        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
