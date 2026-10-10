include(CTest)
include(Catch)

add_executable(nautilus-c-api-tests
        IRCApiPrograms.c
        IRCApiTest.cpp
        EngineCApiTest.cpp
)

# The programs are plain C, which is the point: they prove the headers are usable from C.
set_source_files_properties(IRCApiPrograms.c PROPERTIES LANGUAGE C)
target_link_libraries(nautilus-c-api-tests PRIVATE nautilus Catch2::Catch2WithMain nautilus-sanitizer-suppressions)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
catch_discover_tests(nautilus-c-api-tests EXTRA_ARGS --allow-running-no-tests)
