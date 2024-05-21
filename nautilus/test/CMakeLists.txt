
include(CTest)
include(Catch)

set(TEST_DATA_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/test/data/")

add_subdirectory(execution-tests)
add_subdirectory(val-tests)

