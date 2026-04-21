add_library(nautilus-profile)

set(PROFILE_SOURCES
        src/profile_runtime.cpp
)

target_sources(nautilus-profile PRIVATE ${PROFILE_SOURCES})

target_include_directories(nautilus-profile PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/>)

target_include_directories(nautilus-profile PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${nautilus_SOURCE_DIR}/nautilus/src>)

target_link_libraries(nautilus-profile PUBLIC nautilus)

if (ENABLE_LOGGING)
    target_link_libraries(nautilus-profile PRIVATE spdlog::spdlog)
endif ()

if (ENABLE_TESTS)
    add_subdirectory(test)
endif ()
