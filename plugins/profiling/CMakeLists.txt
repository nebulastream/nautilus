
add_library(nautilus-profiling)

target_sources(nautilus-profiling PRIVATE
        src/Profiler.cpp
        src/ProfilingInstrumentationPhase.cpp)

target_include_directories(nautilus-profiling PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/>)

target_include_directories(nautilus-profiling PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/nautilus/src>)

target_link_libraries(nautilus-profiling PUBLIC nautilus)

if (ENABLE_TESTS)
    add_subdirectory(test)
endif ()
