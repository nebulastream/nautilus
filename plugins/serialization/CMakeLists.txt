# Binary serialization of Nautilus IR (docs/ir-serialization.md).
add_library(nautilus-serialization)

target_sources(nautilus-serialization PRIVATE
        src/IRSerializer.cpp
        src/IRDeserializer.cpp
        src/SymbolResolution.cpp
)

target_include_directories(nautilus-serialization PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/>)

# The serializer walks and rebuilds the IR, whose headers are internal to
# nautilus. The public header only forward-declares IRGraph, so consumers do
# not need this path.
target_include_directories(nautilus-serialization PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${nautilus_SOURCE_DIR}/nautilus/src>)

target_link_libraries(nautilus-serialization PUBLIC nautilus)
target_link_libraries(nautilus-serialization PRIVATE fmt::fmt)

if (ENABLE_TESTS)
    add_subdirectory(test)
endif ()

# Install rules: register nautilus-serialization in the same export set used by
# the core nautilus library so that `find_package(nautilus)` exposes this
# plugin as `nautilus::nautilus-serialization`.
include(GNUInstallDirs)
add_library(${PROJECT_NAME}::nautilus-serialization ALIAS nautilus-serialization)

install(
        TARGETS nautilus-serialization
        EXPORT ${PROJECT_NAME}-config
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include/nautilus/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nautilus)
