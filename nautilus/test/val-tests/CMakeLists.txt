add_executable(nautilus-value-tests
        IntegerValTypeTest.cpp
        FloatValTypeTest.cpp
        FunctionPtrValTypeTest.cpp
        PtrValTypeTest.cpp
        FunctionTest.cpp
        ValConceptTest.cpp
        StaticValTypeTest.cpp
        #  Interface/IntegerTypeTest.cpp
        #   Interface/FloatTypeTest.cpp
)

target_link_libraries(nautilus-value-tests PRIVATE nautilus Catch2::Catch2WithMain nautilus-sanitizer-suppressions)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
catch_discover_tests(nautilus-value-tests EXTRA_ARGS --allow-running-no-tests)
