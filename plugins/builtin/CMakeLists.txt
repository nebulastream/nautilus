
add_library(nautilus-builtin)

set(BUILTIN_SOURCES
        src/prefetch/prefetch.cpp
)

if (ENABLE_MLIR_BACKEND OR ENABLE_ASMJIT_BACKEND)
    list(APPEND BUILTIN_SOURCES src/BuiltinPluginInit.cpp)
endif ()

if (ENABLE_MLIR_BACKEND)
    list(APPEND BUILTIN_SOURCES src/prefetch/MLIRPrefetchIntrinsics.cpp)
endif ()

if (ENABLE_ASMJIT_BACKEND)
    list(APPEND BUILTIN_SOURCES src/prefetch/AsmJitPrefetchIntrinsics.cpp)
endif ()

target_sources(nautilus-builtin PRIVATE ${BUILTIN_SOURCES})

target_include_directories(nautilus-builtin PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/>)

target_include_directories(nautilus-builtin PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)

target_link_libraries(nautilus-builtin PUBLIC nautilus)

if (ENABLE_MLIR_BACKEND OR ENABLE_ASMJIT_BACKEND)
    # Both backend plugins need access to nautilus's internal headers.
    target_include_directories(nautilus-builtin PRIVATE
            $<BUILD_INTERFACE:${nautilus_SOURCE_DIR}/nautilus/src>)
endif ()

if (ENABLE_MLIR_BACKEND)
    find_package(MLIR REQUIRED CONFIG)
    # MLIR intrinsics need access to MLIR/LLVM headers and libraries.
    target_include_directories(nautilus-builtin SYSTEM PRIVATE
            $<BUILD_INTERFACE:${MLIR_INCLUDE_DIRS}>)
    target_link_libraries(nautilus-builtin PRIVATE
            MLIRIR
            MLIRLLVMDialect
    )
endif ()

if (ENABLE_ASMJIT_BACKEND)
    # AsmJit Compiler API is needed by the AsmJit intrinsic handlers.
    target_link_libraries(nautilus-builtin PRIVATE asmjit)
endif ()

if (ENABLE_TESTS)
    add_subdirectory(test)
endif ()

# Install rules: register nautilus-builtin in the same export set used by the
# core nautilus library so that `find_package(nautilus)` exposes this plugin
# as `nautilus::nautilus-builtin`.
include(GNUInstallDirs)
add_library(${PROJECT_NAME}::nautilus-builtin ALIAS nautilus-builtin)

install(
        TARGETS nautilus-builtin
        EXPORT ${PROJECT_NAME}-config
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include/nautilus/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nautilus)
