# nautilus-profiling: in-process hardware performance monitoring.
#
# Complements the `perf` engine option (which writes jitdump records for an
# out-of-process `perf record` / `perf inject` run, see docs/profiling.md) with
# counters and sampling read from inside the process, via the vendored perf-cpp.
#
# The public headers never include a perfcpp header: every class is a pimpl, so
# perf-cpp stays a private implementation detail and the same headers compile on
# platforms where the perf subsystem does not exist at all.

add_library(nautilus-profiling)

set(PROFILING_SOURCES)

# perf_event_open(2) is Linux-only. Everywhere else the plugin still builds and
# the API still exists -- it just reports itself permanently unavailable, the
# same warning-and-continue contract the `perf` option already honours.
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(NAUTILUS_PROFILING_SUPPORTED ON)
else ()
    set(NAUTILUS_PROFILING_SUPPORTED OFF)
    message(STATUS "nautilus-profiling: perf_event_open is Linux-only; building unsupported-platform stubs")
endif ()

if (NAUTILUS_PROFILING_SUPPORTED)
    list(APPEND PROFILING_SOURCES
            src/Availability.cpp
            src/Counters.cpp
            src/JitSymbolResolver.cpp
            src/Measurement.cpp
            src/SampleAggregation.cpp
            src/SampleReport.cpp
            src/Sampler.cpp
            src/ThreadSlots.cpp
    )
else ()
    list(APPEND PROFILING_SOURCES
            src/UnsupportedPlatform.cpp
            src/Measurement.cpp
            src/SampleReport.cpp
    )
endif ()

target_sources(nautilus-profiling PRIVATE ${PROFILING_SOURCES})

target_include_directories(nautilus-profiling PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/>)

target_include_directories(nautilus-profiling PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)

target_link_libraries(nautilus-profiling PUBLIC nautilus)

if (NAUTILUS_PROFILING_SUPPORTED)
    # PRIVATE: perf-cpp is an implementation detail behind the pimpls, so it does
    # not appear in the public include path or link interface of a consumer.
    target_link_libraries(nautilus-profiling PRIVATE perf-cpp)
    target_compile_definitions(nautilus-profiling PUBLIC NAUTILUS_PROFILING_SUPPORTED=1)
else ()
    target_compile_definitions(nautilus-profiling PUBLIC NAUTILUS_PROFILING_SUPPORTED=0)
endif ()

if (ENABLE_LOGGING)
    target_link_libraries(nautilus-profiling PRIVATE spdlog::spdlog)
endif ()

if (ENABLE_TESTS)
    add_subdirectory(test)
endif ()

# Install rules: register nautilus-profiling in the same export set used by the core
# nautilus library so that `find_package(nautilus)` exposes this plugin as
# `nautilus::nautilus-profiling`.
include(GNUInstallDirs)
add_library(${PROJECT_NAME}::nautilus-profiling ALIAS nautilus-profiling)

# perf-cpp is linked PRIVATE, but a static library still records a private
# dependency in its link interface ($<LINK_ONLY:perf-cpp>), so it has to be in an
# export set or the export of nautilus-profiling fails -- and a consumer linking
# the installed archive genuinely needs it to resolve symbols. Installing it into
# the same set is the pattern dyncallback_s already follows in
# nautilus/CMakeLists.txt. Only the archive is installed: perf-cpp's headers stay
# out of the consumer's prefix, since our public headers never include them.
if (NAUTILUS_PROFILING_SUPPORTED)
    list(APPEND PROFILING_EXPORT_TARGETS perf-cpp)
endif ()

install(
        TARGETS nautilus-profiling ${PROFILING_EXPORT_TARGETS}
        EXPORT ${PROJECT_NAME}-config
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include/nautilus/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nautilus)
