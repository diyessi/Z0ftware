add_executable(tapedump
    tapedump.cpp
)

llvm_map_components_to_libnames(llvm_libs Support)

target_link_libraries(tapedump
    Z0ftware
    ${llvm_libs}
)

add_executable(sharedump
    sharedump.cpp
)

target_link_libraries(sharedump
    Z0ftware
    ${llvm_libs}
    nlohmann_json::nlohmann_json
)
