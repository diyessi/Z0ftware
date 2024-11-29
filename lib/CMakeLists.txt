
add_library(Z0ftware
    asm.cpp
    bcd.cpp
    card.cpp
    disasm.cpp
    exprs.cpp
    op.cpp
    operation.cpp
    parity.cpp
    parser.cpp
    tape.cpp
    utils.cpp
)

target_include_directories(Z0ftware PUBLIC "${cpp-peglib_SOURCE_DIR}")
