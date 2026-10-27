include(CTest)
include(Catch)

add_executable(nautilus-tracing-unit-tests
        AliveVariableHashTest.cpp
        ArenaTest.cpp
        ChunkPoolTest.cpp
        TrieTest.cpp
)

target_link_libraries(nautilus-tracing-unit-tests PRIVATE nautilus Catch2::Catch2WithMain nautilus-sanitizer-suppressions)
target_include_directories(nautilus-tracing-unit-tests PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../common>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../src>
)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
catch_discover_tests(nautilus-tracing-unit-tests EXTRA_ARGS --allow-running-no-tests)

# The direct NIR tracer: golden pre-SSA dumps, agreement with the 0.3 tracer, and the allocation counter. Its own
# executable, because it replaces the global operator new. NAUTILUS_UPDATE_GOLDEN=1 rewrites the dumps in data/nir-trace.
add_executable(nautilus-nir-tracer-tests
        NirTracerTest.cpp
        AllocationCounter.cpp
)
target_link_libraries(nautilus-nir-tracer-tests PRIVATE nautilus Catch2::Catch2WithMain)
target_include_directories(nautilus-nir-tracer-tests PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../common>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../src>
)
target_compile_definitions(nautilus-nir-tracer-tests PRIVATE
        NIR_TRACE_GOLDEN_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../data/nir-trace")
catch_discover_tests(nautilus-nir-tracer-tests EXTRA_ARGS --allow-running-no-tests)
