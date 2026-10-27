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

# The direct NIR tracer: golden pre-SSA dumps and agreement with the 0.3 tracer. NAUTILUS_UPDATE_GOLDEN=1 rewrites the
# dumps in data/nir-trace.
target_sources(nautilus-tracing-unit-tests PRIVATE NirTracerTest.cpp)
target_compile_definitions(nautilus-tracing-unit-tests PRIVATE
        NIR_TRACE_GOLDEN_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../data/nir-trace")

# The allocation counter of the direct NIR tracer. Its own executable, because it replaces the global operator new,
# which the address and thread sanitizers define themselves: it is not built under them.
if (NOT ENABLE_ADDRESS_SANITIZER AND NOT ENABLE_THREAD_SANITIZER)
    add_executable(nautilus-nir-tracer-allocation-tests
            NirTracerAllocationTest.cpp
            AllocationCounter.cpp
    )
    target_link_libraries(nautilus-nir-tracer-allocation-tests PRIVATE nautilus Catch2::Catch2WithMain)
    target_include_directories(nautilus-nir-tracer-allocation-tests PRIVATE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../common>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../src>
    )
    target_compile_definitions(nautilus-nir-tracer-allocation-tests PRIVATE
            NIR_TRACE_GOLDEN_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../data/nir-trace")
    catch_discover_tests(nautilus-nir-tracer-allocation-tests EXTRA_ARGS --allow-running-no-tests)
endif ()
