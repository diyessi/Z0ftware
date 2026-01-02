llvm_map_components_to_libnames(llvm_libs Support)

add_executable(sharedump
    sharedump.cpp
)

target_link_libraries(sharedump
    Z0ftware
    ${llvm_libs}
    nlohmann_json::nlohmann_json
)
