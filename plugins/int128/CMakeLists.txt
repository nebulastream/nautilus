add_library(nautilus-int128 src/int128.cpp)

if (ENABLE_MLIR_BACKEND)
    target_sources(nautilus-int128 PRIVATE src/MLIRInt128Intrinsics.cpp)
    find_package(MLIR REQUIRED CONFIG)
    target_include_directories(nautilus-int128 PRIVATE ${nautilus_SOURCE_DIR}/nautilus/src)
    target_include_directories(nautilus-int128 SYSTEM PRIVATE ${MLIR_INCLUDE_DIRS})
    target_link_libraries(nautilus-int128 PRIVATE MLIRIR MLIRLLVMDialect MLIRArithDialect)
endif ()

target_include_directories(nautilus-int128 PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/>)
target_include_directories(nautilus-int128 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(nautilus-int128 PUBLIC nautilus)

include(GNUInstallDirs)
add_library(${PROJECT_NAME}::nautilus-int128 ALIAS nautilus-int128)
install(TARGETS nautilus-int128 EXPORT ${PROJECT_NAME}-config
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY include/nautilus/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nautilus)

if (ENABLE_TESTS)
    add_subdirectory(test)
endif ()
