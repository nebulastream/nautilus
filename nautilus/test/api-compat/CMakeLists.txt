# The API-compatibility suite (docs/plans/nautilus-0.4/16-implementation-plan.md, M1.10): test cases of 0.3's execution
# and value tests that include public headers only, name no backend and neither parse IR nor read dumps, copied
# unchanged (the sources are frozen: do not edit them; a failure means the public API changed). The harness in
# common/ runs a body on the interpreter and on every enabled backend.
include(CTest)
include(Catch)

if (ENABLE_TRACING)
    add_executable(nautilus-api-compat-tests
            BoolExecutionTest.cpp
            CastExecutionTest.cpp
            ConcurrentExecutionTest.cpp
            ExecutionTest.cpp
            FloatValTypeTest.cpp
            FunctionPtrValTypeTest.cpp
            FunctionTest.cpp
            IntegerValTypeTest.cpp
            InvokeNonDefaultConstructibleReturnTest.cpp
            MemoryIntrinsicExecutionTest.cpp
            ModuleLifecycleTest.cpp
            PointerExecutionTest.cpp
            PtrValTypeTest.cpp
            SelectExecutionTest.cpp
            StaticValTypeTest.cpp
            ValConceptTest.cpp
    )
    target_include_directories(nautilus-api-compat-tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common)
    target_link_libraries(nautilus-api-compat-tests PRIVATE nautilus Catch2::Catch2WithMain nautilus-sanitizer-suppressions)
    list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
    catch_discover_tests(nautilus-api-compat-tests TEST_PREFIX "api-compat: " EXTRA_ARGS --allow-running-no-tests)
endif ()

# The examples (docs/plans/nautilus-0.4/16-implementation-plan.md, M1.10), copied unchanged. An example that pins no
# backend runs on the default configuration; one that pins a backend is recorded here with it and runs only when that
# backend is enabled. DemoBackends (compares every backend), DemoInliningPlugin (needs the Clang pass plugin) and DemoGPU
# (disabled until M7.7) are not copied.
if (ENABLE_TRACING)
    # api_compat_example(<name> <source> <pinned backend or NONE> [<plugin library>])
    function(api_compat_example name source pinned plugin)
        string(TOUPPER "${pinned}" pinnedUpper)
        if (NOT pinned STREQUAL "NONE")
            if (pinned STREQUAL "cpp")
                set(flag ENABLE_C_BACKEND)
            else ()
                set(flag ENABLE_${pinnedUpper}_BACKEND)
            endif ()
            if (NOT ${flag})
                return()
            endif ()
        endif ()
        # the plugin targets are defined after this directory is processed, so test the option that builds them
        if (plugin)
            string(REPLACE "nautilus-" "" pluginName "${plugin}")
            string(TOUPPER "${pluginName}" pluginUpper)
            if (NOT ENABLE_${pluginUpper}_PLUGIN)
                return()
            endif ()
        endif ()
        add_executable(${name} examples/${source})
        if (plugin)
            # the plugin registers itself from a static initializer, so the linker must keep all of it (portable:
            # --whole-archive on ELF, -force_load on macOS)
            target_link_libraries(${name} PRIVATE nautilus "$<LINK_LIBRARY:WHOLE_ARCHIVE,${plugin}>"
                    nautilus-sanitizer-suppressions)
        else ()
            target_link_libraries(${name} PRIVATE nautilus nautilus-sanitizer-suppressions)
        endif ()
        add_test(NAME "api-compat: example ${name}" COMMAND ${name})
    endfunction()

    api_compat_example(api_compat_demo_modules DemoModules.cpp NONE "")
    api_compat_example(api_compat_demo_runtime_calls DemoRuntimeCalls.cpp NONE "")
    api_compat_example(api_compat_demo_std_plugin DemoStdPlugin.cpp NONE nautilus-std)
    api_compat_example(api_compat_demo_jit DemoJit.cpp cpp "")
    api_compat_example(api_compat_demo_regions DemoRegions.cpp cpp "")
    api_compat_example(api_compat_demo_simd_plugin DemoSimdPlugin.cpp mlir nautilus-simd)
    api_compat_example(api_compat_demo_specialization_plugin DemoSpecializationPlugin.cpp mlir
            nautilus-specialization)
    if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        api_compat_example(api_compat_demo_profiling DemoProfiling.cpp mlir nautilus-profiling)
        if (TARGET api_compat_demo_profiling AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            target_link_options(api_compat_demo_profiling PRIVATE -rdynamic)
        endif ()
    endif ()
endif ()
