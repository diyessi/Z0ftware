include(GoogleTest)

add_executable(Z0ftware_tests
    cards.cpp
    characters.cpp
    exprs.cpp
    field.cpp
    sap.cpp
    word.cpp
)

target_include_directories(Z0ftware_tests PUBLIC "${cpp-peglib_SOURCE_DIR}")

target_link_libraries(Z0ftware_tests
    Z0ftware
    gtest_main
)

gtest_discover_tests(Z0ftware_tests
    TARGET Z0ftware_tests
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

