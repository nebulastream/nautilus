cmake_minimum_required(VERSION 3.22.1)

project(nautilus-example)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CATCH_ENABLE_REPRODUCIBLE_BUILD OFF CACHE INTERNAL "Turn off tests")
set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -Werror=extra -Werror=all -fpermissive -fPIC -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_BINARY_DIR}/nautilus)

add_executable(demo src/DemoJit.cpp)

target_link_libraries(demo nautilus)