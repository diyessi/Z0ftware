include(GoogleTest)

add_executable(Z0ftware_tests
    field.cpp
)

target_link_libraries(Z0ftware_tests
    gtest_main
)

gtest_discover_tests(Z0ftware_tests
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

