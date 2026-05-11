
add_library(nautilus-std)

set(STD_SOURCES
        src/bit.cpp
        src/cmath.cpp
        src/cstdlib.cpp
        src/cstring.cpp
        src/string.cpp
        src/string_view.cpp
        src/vector.cpp
        src/iostream.cpp
        src/atomic.cpp
)

if (ENABLE_MLIR_BACKEND)
    list(APPEND STD_SOURCES src/MLIRMathIntrinsics.cpp src/MLIRBitIntrinsics.cpp src/MLIRAtomicIntrinsics.cpp src/StdPluginInit.cpp)
endif ()

target_sources(nautilus-std PRIVATE ${STD_SOURCES})

target_include_directories(nautilus-std PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/>)

target_include_directories(nautilus-std PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)

target_link_libraries(nautilus-std PUBLIC nautilus)

if (ENABLE_MLIR_BACKEND)
    find_package(MLIR REQUIRED CONFIG)
    # MLIR intrinsics need access to nautilus internal headers and MLIR/LLVM
    target_include_directories(nautilus-std PRIVATE
            $<BUILD_INTERFACE:${nautilus_SOURCE_DIR}/nautilus/src>)
    target_include_directories(nautilus-std SYSTEM PRIVATE
            $<BUILD_INTERFACE:${MLIR_INCLUDE_DIRS}>)
    target_link_libraries(nautilus-std PRIVATE
            MLIRIR
            MLIRLLVMDialect
            MLIRMathDialect
    )
endif ()

if (ENABLE_TESTS)
    add_subdirectory(test)
endif ()

# Install rules: register nautilus-std in the same export set used by the core
# nautilus library so that `find_package(nautilus)` exposes this plugin as
# `nautilus::nautilus-std`.
include(GNUInstallDirs)
add_library(${PROJECT_NAME}::nautilus-std ALIAS nautilus-std)

install(
        TARGETS nautilus-std
        EXPORT ${PROJECT_NAME}-config
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include/nautilus/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nautilus)
