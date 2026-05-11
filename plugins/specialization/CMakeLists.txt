
add_library(nautilus-specialization)

set(SPECIALIZATION_SOURCES
        src/Profile.cpp
        src/assume.cpp
)

if (ENABLE_MLIR_BACKEND)
    list(APPEND SPECIALIZATION_SOURCES
            src/MLIRAssumeIntrinsics.cpp
            src/SpecializationPluginInit.cpp)
endif ()

target_sources(nautilus-specialization PRIVATE ${SPECIALIZATION_SOURCES})

target_include_directories(nautilus-specialization PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/>)

target_include_directories(nautilus-specialization PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)

target_link_libraries(nautilus-specialization PUBLIC nautilus)

if (ENABLE_MLIR_BACKEND)
    find_package(MLIR REQUIRED CONFIG)
    # MLIR intrinsics need access to nautilus internal headers and MLIR/LLVM
    target_include_directories(nautilus-specialization PRIVATE
            $<BUILD_INTERFACE:${nautilus_SOURCE_DIR}/nautilus/src>)
    target_include_directories(nautilus-specialization SYSTEM PRIVATE
            $<BUILD_INTERFACE:${MLIR_INCLUDE_DIRS}>)
    target_link_libraries(nautilus-specialization PRIVATE
            MLIRIR
            MLIRLLVMDialect
    )
endif ()

if (ENABLE_TESTS)
    add_subdirectory(test)
endif ()

# Install rules: register nautilus-specialization in the same export set used
# by the core nautilus library so that `find_package(nautilus)` exposes this
# plugin as `nautilus::nautilus-specialization`.
include(GNUInstallDirs)
add_library(${PROJECT_NAME}::nautilus-specialization ALIAS nautilus-specialization)

install(
        TARGETS nautilus-specialization
        EXPORT ${PROJECT_NAME}-config
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include/nautilus/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nautilus)
