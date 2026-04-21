cmake_dependent_option(ENABLE_CUDA_BACKEND "Enable the CUDA GPU backend" ON "ENABLE_GPU_PLUGIN" OFF)
cmake_dependent_option(ENABLE_METAL_BACKEND "Enable the Apple Metal GPU backend" ON "ENABLE_GPU_PLUGIN" OFF)

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/include/nautilus/gpu/config.hpp.in
        ${CMAKE_CURRENT_BINARY_DIR}/include/nautilus/gpu/config.hpp
)

add_library(nautilus-gpu)

set(GPU_SOURCES
        src/gpu.cpp
        src/gpu_backends.cpp
)

target_sources(nautilus-gpu PRIVATE ${GPU_SOURCES})

if (ENABLE_CUDA_BACKEND)
    add_subdirectory(src/cuda)
endif ()

if (ENABLE_METAL_BACKEND)
    add_subdirectory(src/metal)
endif ()

target_include_directories(nautilus-gpu PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include/>)

target_include_directories(nautilus-gpu PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/nautilus/src>)

target_link_libraries(nautilus-gpu PUBLIC nautilus)
target_link_libraries(nautilus-gpu PRIVATE fmt::fmt)

if (ENABLE_TESTS)
    add_subdirectory(test)
endif ()
