
add_library(nautilus-simd)

set(SIMD_SOURCES
        src/vector.cpp
)

if (ENABLE_MLIR_BACKEND)
    list(APPEND SIMD_SOURCES src/MLIRVectorIntrinsics.cpp src/SimdPluginInit.cpp)
endif ()

target_sources(nautilus-simd PRIVATE ${SIMD_SOURCES})

target_include_directories(nautilus-simd PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/>)

target_include_directories(nautilus-simd PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)

target_link_libraries(nautilus-simd PUBLIC nautilus)

if (ENABLE_MLIR_BACKEND)
    find_package(MLIR REQUIRED CONFIG)
    # MLIR intrinsics need access to nautilus internal headers and MLIR/LLVM
    target_include_directories(nautilus-simd PRIVATE
            $<BUILD_INTERFACE:${nautilus_SOURCE_DIR}/nautilus/src>)
    target_include_directories(nautilus-simd SYSTEM PRIVATE
            $<BUILD_INTERFACE:${MLIR_INCLUDE_DIRS}>)
    # Link MLIR libraries needed by MLIRVectorIntrinsics.cpp
    target_link_libraries(nautilus-simd PRIVATE
            MLIRIR
            MLIRLLVMDialect
    )
endif ()

if (ENABLE_TESTS)
    add_subdirectory(test)
endif ()
