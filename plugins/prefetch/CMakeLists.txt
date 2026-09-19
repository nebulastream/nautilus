
add_library(nautilus-prefetch)

set(PREFETCH_SOURCES
        src/prefetch.cpp
)

if (ENABLE_MLIR_BACKEND OR ENABLE_ASMJIT_BACKEND)
    list(APPEND PREFETCH_SOURCES src/PrefetchPluginInit.cpp)
endif ()

if (ENABLE_MLIR_BACKEND)
    list(APPEND PREFETCH_SOURCES src/MLIRPrefetchIntrinsics.cpp)
endif ()

if (ENABLE_ASMJIT_BACKEND)
    list(APPEND PREFETCH_SOURCES src/AsmJitPrefetchIntrinsics.cpp)
endif ()

target_sources(nautilus-prefetch PRIVATE ${PREFETCH_SOURCES})

target_include_directories(nautilus-prefetch PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/>)

target_include_directories(nautilus-prefetch PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)

target_link_libraries(nautilus-prefetch PUBLIC nautilus)

if (ENABLE_MLIR_BACKEND OR ENABLE_ASMJIT_BACKEND)
    # Both backend plugins need access to nautilus's internal headers.
    target_include_directories(nautilus-prefetch PRIVATE
            $<BUILD_INTERFACE:${nautilus_SOURCE_DIR}/nautilus/src>)
endif ()

if (ENABLE_MLIR_BACKEND)
    find_package(MLIR REQUIRED CONFIG)
    # MLIR intrinsics need access to MLIR/LLVM headers and libraries.
    target_include_directories(nautilus-prefetch SYSTEM PRIVATE
            $<BUILD_INTERFACE:${MLIR_INCLUDE_DIRS}>)
    target_link_libraries(nautilus-prefetch PRIVATE
            MLIRIR
            MLIRLLVMDialect
    )
endif ()

if (ENABLE_ASMJIT_BACKEND)
    # AsmJit Compiler API is needed by the AsmJit intrinsic handlers.
    target_link_libraries(nautilus-prefetch PRIVATE asmjit)
endif ()

if (ENABLE_TESTS)
    add_subdirectory(test)
endif ()

# Install rules: register nautilus-prefetch in the same export set used by the
# core nautilus library so that `find_package(nautilus)` exposes this plugin
# as `nautilus::nautilus-prefetch`.
include(GNUInstallDirs)
add_library(${PROJECT_NAME}::nautilus-prefetch ALIAS nautilus-prefetch)

install(
        TARGETS nautilus-prefetch
        EXPORT ${PROJECT_NAME}-config
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include/nautilus/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nautilus)
