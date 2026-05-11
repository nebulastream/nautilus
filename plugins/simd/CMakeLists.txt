
add_library(nautilus-simd)

set(SIMD_SOURCES
        src/vector.cpp
)

if (ENABLE_MLIR_BACKEND OR ENABLE_ASMJIT_BACKEND)
    list(APPEND SIMD_SOURCES src/SimdPluginInit.cpp)
endif ()

if (ENABLE_MLIR_BACKEND)
    list(APPEND SIMD_SOURCES src/MLIRVectorIntrinsics.cpp)
endif ()

if (ENABLE_ASMJIT_BACKEND)
    list(APPEND SIMD_SOURCES src/AsmJitVectorIntrinsics.cpp)
endif ()

target_sources(nautilus-simd PRIVATE ${SIMD_SOURCES})

target_include_directories(nautilus-simd PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/>)

target_include_directories(nautilus-simd PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)

target_link_libraries(nautilus-simd PUBLIC nautilus)

if (ENABLE_MLIR_BACKEND OR ENABLE_ASMJIT_BACKEND)
    # Both backend plugins need access to nautilus's internal headers.
    target_include_directories(nautilus-simd PRIVATE
            $<BUILD_INTERFACE:${nautilus_SOURCE_DIR}/nautilus/src>)
endif ()

if (ENABLE_MLIR_BACKEND)
    find_package(MLIR REQUIRED CONFIG)
    # MLIR intrinsics need access to MLIR/LLVM headers and libraries.
    target_include_directories(nautilus-simd SYSTEM PRIVATE
            $<BUILD_INTERFACE:${MLIR_INCLUDE_DIRS}>)
    target_link_libraries(nautilus-simd PRIVATE
            MLIRIR
            MLIRLLVMDialect
    )
endif ()

if (ENABLE_ASMJIT_BACKEND)
    # AsmJit Compiler API is needed by the AsmJit intrinsic handlers.
    target_link_libraries(nautilus-simd PRIVATE asmjit)
endif ()

if (ENABLE_TESTS)
    add_subdirectory(test)
endif ()
