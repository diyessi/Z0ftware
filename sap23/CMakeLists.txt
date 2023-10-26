add_executable(sap23
    sap23.cpp
)

llvm_map_components_to_libnames(llvm_libs Support)

target_link_libraries(sap23
    Z0ftware
    ${llvm_libs}
)
