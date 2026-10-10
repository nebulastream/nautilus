include(CTest)
include(Catch)

# The programs are plain C, which is the point: they prove the headers are
# usable from C. Strict C11 with warnings as errors keeps them that way.
set(NAUTILUS_C_API_STRICT_C_OPTIONS -std=c11 -Wall -Wextra -Wpedantic -Werror)
set_source_files_properties(IRCApiPrograms.c SharedLibraryTest.c PROPERTIES
        LANGUAGE C
        COMPILE_OPTIONS "${NAUTILUS_C_API_STRICT_C_OPTIONS}")

add_executable(nautilus-c-api-tests
        IRCApiPrograms.c
        IRCApiTest.cpp
        EngineCApiTest.cpp
)
target_link_libraries(nautilus-c-api-tests PRIVATE nautilus Catch2::Catch2WithMain nautilus-sanitizer-suppressions)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
catch_discover_tests(nautilus-c-api-tests EXTRA_ARGS --allow-running-no-tests)

if (ENABLE_C_API_SHARED_LIBRARY)
    # A C program that links libnautilus-c and nothing else, as a binding does.
    add_executable(nautilus-c-api-shared-library-test SharedLibraryTest.c IRCApiPrograms.c)
    target_include_directories(nautilus-c-api-shared-library-test PRIVATE ${PROJECT_SOURCE_DIR}/nautilus/include)
    target_link_libraries(nautilus-c-api-shared-library-test PRIVATE nautilus-c)
    add_test(NAME nautilus-c-api-shared-library-test COMMAND nautilus-c-api-shared-library-test)

    if (UNIX AND NOT APPLE)
        # The library must export the C API and nothing else.
        add_test(NAME nautilus-c-api-shared-library-exports
                COMMAND sh -c "! nm -D --defined-only \"$1\" | awk '{print $3}' | grep -v -e '^nautilus_' -e '^_init$' -e '^_fini$' | grep ."
                _ $<TARGET_FILE:nautilus-c>)
    endif ()
endif ()
