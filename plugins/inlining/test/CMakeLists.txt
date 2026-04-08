include(CTest)
include(Catch)

add_executable(nautilus-inlining-tests
        InliningExecutionTest.cpp
        FunctionPtrInliningTest.cpp
)

nautilus_inline(nautilus-inlining-tests)

target_link_libraries(nautilus-inlining-tests PUBLIC nautilus-inlining Catch2::Catch2WithMain nautilus-sanitizer-suppressions)
if (ENABLE_LOGGING)
    target_link_libraries(nautilus-inlining-tests PRIVATE spdlog::spdlog)
endif ()

# The plugin tests drive the same API as the core execution tests, but they
# deliberately include their OWN annotated copies of the common helpers
# (under plugins/inlining/test/common) so NAUTILUS_INLINE stays confined to
# the plugin. The core nautilus/test/common dir provides ExecutionTest.hpp
# (backend iteration helpers) which both trees share. The inlining-plugin
# smoke test additionally needs to see LLVMBackendHooks.hpp from the core
# nautilus src tree to verify the hook registration.
target_include_directories(nautilus-inlining-tests PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/nautilus/test/common>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/nautilus/src>)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
catch_discover_tests(nautilus-inlining-tests EXTRA_ARGS --allow-running-no-tests)
