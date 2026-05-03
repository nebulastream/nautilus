
# Tests for the DWARF variable-name resolver and its integration with
# IRPrintOptions. Only meaningful when (a) tracing is enabled (we need
# val<T> to produce IR) and (b) the MLIR backend is on (that's the
# config that links LLVM's DWARF libraries into the main nautilus lib).
if (ENABLE_TRACING AND ENABLE_MLIR_BACKEND)
    include(CTest)
    include(Catch)

    add_executable(nautilus-debug-info-tests
            DwarfVariableResolverTest.cpp
            VarNameMetadataTest.cpp)

    # Force full DWARF 5 with column info regardless of the outer build
    # config; the tests assert on specific (file, line, column) lookups
    # that only resolve when DW_AT_decl_column is emitted.
    target_compile_options(nautilus-debug-info-tests PRIVATE
            -g -gdwarf-5 -gcolumn-info -O0)

    target_link_libraries(nautilus-debug-info-tests PRIVATE
            nautilus Catch2::Catch2WithMain nautilus-sanitizer-suppressions)

    # Private src/ include path so the test can see LegacyCompiler.hpp
    # and other internal headers used to drive compileToIR directly.
    target_include_directories(nautilus-debug-info-tests PRIVATE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../common>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../src>)

    list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
    catch_discover_tests(nautilus-debug-info-tests EXTRA_ARGS --allow-running-no-tests)
endif ()
