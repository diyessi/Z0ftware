add_executable(tape
    tape.cpp
)

llvm_map_components_to_libnames(llvm_libs Support)

target_link_libraries(tape
    Z0ftware
    ${llvm_libs}
)
