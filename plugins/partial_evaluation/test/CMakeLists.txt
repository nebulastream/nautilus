include(CTest)
include(Catch)

# Plugin-local test binary. Exists only under ENABLE_CONSTANT_TRACER
# (the parent CMakeLists early-returns otherwise, so this file isn't
# entered at all in default builds).

add_executable(nautilus-partial-evaluation-tests
        ConstantTracerTest.cpp
)

target_link_libraries(nautilus-partial-evaluation-tests PRIVATE
        nautilus
        Catch2::Catch2WithMain
        nautilus-sanitizer-suppressions)

if (ENABLE_LOGGING)
    target_link_libraries(nautilus-partial-evaluation-tests PRIVATE spdlog::spdlog)
endif ()

# The test includes <catch2/catch_all.hpp> and Nautilus public headers —
# both already reachable via nautilus's PUBLIC include path and Catch2.
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
catch_discover_tests(nautilus-partial-evaluation-tests EXTRA_ARGS --allow-running-no-tests)
