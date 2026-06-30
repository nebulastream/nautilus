# Differential AST fuzzer: generates random well-defined integer programs,
# realizes them as traced Nautilus kernels, and asserts every backend (plus the
# interpreter) agrees with an independent native C++ oracle. Coverage-guided via
# libFuzzer. Opt-in through -DENABLE_FUZZING=ON (Clang only); kept out of the
# default ctest run so normal builds are unaffected.
#
# Build & run:
#   cmake -DENABLE_FUZZING=ON -DCMAKE_CXX_COMPILER=clang++-21 ..
#   cmake --build . --target nautilus-fuzz
#   ./nautilus/test/fuzz/nautilus-fuzz -max_total_time=120
#
# For memory/UB checking, additionally configure with
# -DENABLE_ADDRESS_SANITIZER=ON (or -DENABLE_UB_SANITIZER=ON); those flags are
# applied globally and compose with the libFuzzer instrumentation.

add_executable(nautilus-fuzz FuzzMain.cpp)

target_link_libraries(nautilus-fuzz PRIVATE nautilus)
if (ENABLE_LOGGING)
    target_link_libraries(nautilus-fuzz PRIVATE spdlog::spdlog)
endif ()

# Link the libFuzzer runtime (provides main + the coverage-driven mutation loop).
# The rest of the build is instrumented with -fsanitize=fuzzer-no-link, set
# globally when ENABLE_FUZZING is on.
target_compile_options(nautilus-fuzz PRIVATE -fsanitize=fuzzer)
target_link_options(nautilus-fuzz PRIVATE -fsanitize=fuzzer)
