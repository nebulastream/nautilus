add_library(nautilus-profile)

set(PROFILE_SOURCES
        src/profile_runtime.cpp
        src/JitSymbols.cpp
        src/Sampler.cpp
)

if (ENABLE_MLIR_BACKEND)
    list(APPEND PROFILE_SOURCES src/MLIRJitEventListener.cpp)
endif ()

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

find_package(Threads REQUIRED)
target_link_libraries(nautilus-profile PUBLIC Threads::Threads ${CMAKE_DL_LIBS})

if (ENABLE_MLIR_BACKEND)
    # MLIRJitEventListener.cpp subclasses llvm::JITEventListener, so it needs
    # to be compiled with -fno-rtti to match how the LLVM static libraries
    # are built (same reason the existing MLIRJitEventListenerTestFixture.cpp
    # is no-rtti). The rest of the plugin keeps RTTI enabled.
    set_source_files_properties(src/MLIRJitEventListener.cpp
            PROPERTIES COMPILE_OPTIONS -fno-rtti)
    find_package(MLIR QUIET CONFIG)
    if (MLIR_INCLUDE_DIRS)
        target_include_directories(nautilus-profile SYSTEM PRIVATE ${MLIR_INCLUDE_DIRS})
    endif ()
endif ()

if (ENABLE_TESTS)
    add_subdirectory(test)
endif ()
