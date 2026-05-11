
# NautilusInline.cmake is normally included from the top-level CMakeLists.txt
# so `nautilus_inline()` is globally defined. Re-including here is a no-op
# thanks to the `include_guard(GLOBAL)` inside the file.
include(${CMAKE_CURRENT_LIST_DIR}/cmake/NautilusInline.cmake)

add_library(nautilus-inlining)

set(INLINING_SOURCES
        src/InlineFunctionRegistry.cpp
        src/InliningPluginInit.cpp
)

if (ENABLE_MLIR_BACKEND)
    list(APPEND INLINING_SOURCES
            src/LLVMInliningUtils.cpp)
endif ()

target_sources(nautilus-inlining PRIVATE ${INLINING_SOURCES})

target_include_directories(nautilus-inlining PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/>)

target_include_directories(nautilus-inlining PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)

target_link_libraries(nautilus-inlining PUBLIC nautilus)

# The two `register...PleaseIgnoreThisThanks` C symbols and the
# `__attribute__((used)) static const auto` that references them in
# `inline.hpp` need to be externally visible so the JIT's dlsym() lookup can
# resolve them when bitcode-inlined modules reference them by name.
set_target_properties(nautilus-inlining PROPERTIES
        CXX_VISIBILITY_PRESET default
        VISIBILITY_INLINES_HIDDEN OFF)

if (ENABLE_MLIR_BACKEND)
    find_package(MLIR REQUIRED CONFIG)
    # The plugin needs to see LLVMBackendHooks.hpp from the core nautilus src
    # directory, plus the MLIR/LLVM headers to operate on llvm::Module.
    target_include_directories(nautilus-inlining PRIVATE
            $<BUILD_INTERFACE:${nautilus_SOURCE_DIR}/nautilus/src>)
    target_include_directories(nautilus-inlining SYSTEM PRIVATE
            $<BUILD_INTERFACE:${MLIR_INCLUDE_DIRS}>)
    target_link_libraries(nautilus-inlining PRIVATE
            fmt::fmt
            MLIRExecutionEngine
    )
    # Enables the MLIR-gated portion of InliningPluginInit.cpp that installs
    # the LLVMBackendHooks closures. Without this define, that TU still
    # defines the anchor symbol used by `inline.hpp`, so the static archive
    # can always be linked safely.
    target_compile_definitions(nautilus-inlining PRIVATE NAUTILUS_INLINING_MLIR_ENABLED)
endif ()

add_subdirectory(pass)

if (ENABLE_TESTS)
    add_subdirectory(test)
endif ()

# Install rules: register nautilus-inlining (and the optional InliningPass
# LLVM pass plugin) in the same export set used by the core nautilus library
# so that `find_package(nautilus)` exposes the plugin as
# `nautilus::nautilus-inlining`. The NautilusInline.cmake helper is also
# shipped so consumers can call `nautilus_inline(target)` after installation.
include(GNUInstallDirs)
add_library(${PROJECT_NAME}::nautilus-inlining ALIAS nautilus-inlining)

install(
        TARGETS nautilus-inlining
        EXPORT ${PROJECT_NAME}-config
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include/nautilus/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nautilus)

if (TARGET InliningPass)
    install(
            TARGETS InliningPass
            EXPORT ${PROJECT_NAME}-config
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif ()

install(FILES ${CMAKE_CURRENT_LIST_DIR}/cmake/NautilusInline.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
