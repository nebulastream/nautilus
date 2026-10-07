include(CTest)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)

add_executable(nautilus-cache-tests
        CompilationCacheTest.cpp
        IntrinsicCacheFingerprintTest.cpp
        CacheSafetyAnalysisTest.cpp)

target_link_libraries(nautilus-cache-tests PRIVATE
        nautilus-cache
        nautilus
        Catch2::Catch2WithMain
        nautilus-sanitizer-suppressions
        MLIRArithDialect
        MLIRIR)

if (ENABLE_LOGGING)
    target_link_libraries(nautilus-cache-tests PRIVATE spdlog::spdlog)
endif ()

target_include_directories(nautilus-cache-tests PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src>
        $<BUILD_INTERFACE:${nautilus_SOURCE_DIR}/nautilus/src>
        $<BUILD_INTERFACE:${nautilus_SOURCE_DIR}/nautilus/test/common>)
target_include_directories(nautilus-cache-tests SYSTEM PRIVATE
        $<BUILD_INTERFACE:${MLIR_INCLUDE_DIRS}>
        $<BUILD_INTERFACE:${LLVM_INCLUDE_DIRS}>)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_library(nautilus-cache-unidentified-proxy SHARED UnidentifiedCacheProxy.cpp)
    target_link_options(nautilus-cache-unidentified-proxy PRIVATE "LINKER:--build-id=none")
    target_link_libraries(nautilus-cache-tests PRIVATE nautilus-cache-unidentified-proxy ${CMAKE_DL_LIBS})
    target_compile_options(nautilus-cache-tests PRIVATE -fPIE)
    target_link_options(nautilus-cache-tests PRIVATE -pie "LINKER:--build-id=sha1")
endif ()

catch_discover_tests(nautilus-cache-tests
        TEST_PREFIX "cache:"
        EXTRA_ARGS --allow-running-no-tests
        PROPERTIES LABELS "cache;plugin")

if (BUILD_SHARED_LIBS AND CMAKE_SYSTEM_NAME STREQUAL "Linux"
        AND CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|amd64|AMD64)$" AND CMAKE_SIZEOF_VOID_P EQUAL 8)
    find_package(Python3 REQUIRED COMPONENTS Interpreter)
    find_program(NAUTILUS_CACHE_TEST_OBJCOPY NAMES llvm-objcopy objcopy REQUIRED)

    add_executable(nautilus-cache-shared-image-compatibility SharedImageCompatibilityChild.cpp)
    target_link_libraries(nautilus-cache-shared-image-compatibility PRIVATE
            nautilus-cache nautilus nautilus-sanitizer-suppressions ${CMAKE_DL_LIBS})
    target_include_directories(nautilus-cache-shared-image-compatibility PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/../src
            ${nautilus_SOURCE_DIR}/nautilus/src)
    target_compile_options(nautilus-cache-shared-image-compatibility PRIVATE -fPIE)
    target_link_options(nautilus-cache-shared-image-compatibility PRIVATE
            -pie "LINKER:--build-id=sha1" "LINKER:--enable-new-dtags")

    foreach (image_case core-change provider-change core-missing provider-missing)
        add_test(NAME "cache:SharedImageCompatibility.${image_case}"
                COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/SharedImageCompatibility.py
                --child $<TARGET_FILE:nautilus-cache-shared-image-compatibility>
                --core $<TARGET_FILE:nautilus>
                --core-soname $<TARGET_SONAME_FILE_NAME:nautilus>
                --provider $<TARGET_FILE:nautilus-cache>
                --provider-soname $<TARGET_SONAME_FILE_NAME:nautilus-cache>
                --objcopy ${NAUTILUS_CACHE_TEST_OBJCOPY}
                --case ${image_case})
        set_tests_properties("cache:SharedImageCompatibility.${image_case}" PROPERTIES
                LABELS "cache;plugin;shared-image"
                TIMEOUT 240
                SKIP_RETURN_CODE 77)
    endforeach ()
endif ()
