project(libcurl-bundled)

add_library(${PROJECT_NAME} INTERFACE)
add_library(perfkit::libcurl ALIAS ${PROJECT_NAME})

target_link_libraries(
        ${PROJECT_NAME}

        INTERFACE
        libcurl_imp.lib
)

target_link_directories(
        ${PROJECT_NAME}

        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/libcurl.dll
        ${CMAKE_BINARY_DIR}/libcurl.dll
        COPYONLY
)

target_include_directories(
        ${PROJECT_NAME}

        INTERFACE
        include
)
