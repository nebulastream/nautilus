add_library(nautilus-cache)

target_sources(nautilus-cache PRIVATE
        src/CachingCompiler.cpp
        src/CacheSafetyAnalysis.cpp
        src/PersistentModuleCache.cpp
        src/CacheOptions.cpp)

include(GNUInstallDirs)
target_include_directories(nautilus-cache PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_include_directories(nautilus-cache PRIVATE
        $<BUILD_INTERFACE:${nautilus_SOURCE_DIR}/nautilus/src>)

target_link_libraries(nautilus-cache PUBLIC nautilus)

find_package(MLIR REQUIRED CONFIG)
target_include_directories(nautilus-cache SYSTEM PRIVATE
        $<BUILD_INTERFACE:${MLIR_INCLUDE_DIRS}>
        $<BUILD_INTERFACE:${LLVM_INCLUDE_DIRS}>)
target_link_libraries(nautilus-cache PRIVATE
        LLVMOrcJIT
        LLVMTarget
        LLVMSupport)

if (ENABLE_LOGGING)
    target_link_libraries(nautilus-cache PRIVATE spdlog::spdlog)
endif ()

if (CMAKE_SYSTEM_NAME STREQUAL "Linux" AND BUILD_SHARED_LIBS)
    target_link_options(nautilus-cache PRIVATE "LINKER:--build-id=sha1")
endif ()

if (ENABLE_TESTS)
    add_subdirectory(test)
endif ()

add_library(${PROJECT_NAME}::nautilus-cache ALIAS nautilus-cache)

install(
        TARGETS nautilus-cache
        EXPORT ${PROJECT_NAME}-config
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/nautilus/cache/plugin.hpp
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nautilus/cache)
