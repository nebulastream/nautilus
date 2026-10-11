# nautilus-backend-explorer: compiles and runs BenchmarkKernels.hpp under one backend configuration for
# tools/backend-explorer/explore.py. Opt-in (ENABLE_BACKEND_EXPLORER) and clang-only.
add_executable(nautilus-backend-explorer BackendExplorerRunner.cpp)
target_include_directories(nautilus-backend-explorer PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/nautilus/test/common)
target_link_libraries(nautilus-backend-explorer PRIVATE nautilus)
