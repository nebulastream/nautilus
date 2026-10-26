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
