project(nlohmann-json)

add_library(${PROJECT_NAME} INTERFACE)
add_library(perfkit::json ALIAS ${PROJECT_NAME})

target_include_directories(
        ${PROJECT_NAME}

        INTERFACE
        include
)
