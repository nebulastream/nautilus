
include(CTest)
include(Catch)

set(TEST_DATA_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/test/data/")

# Object library for sanitizer suppressions, linked into all test executables.
# Uses an OBJECT library to ensure the weak symbol is not discarded by the linker.
add_library(nautilus-sanitizer-suppressions OBJECT
    ${CMAKE_CURRENT_SOURCE_DIR}/common/SanitizerSuppressions.cpp
)

add_subdirectory(benchmark)
add_subdirectory(common)
add_subdirectory(execution-tests)
if (ENABLE_TRACING)
    add_subdirectory(ir-pass-tests)
    add_subdirectory(tracing-tests)
endif ()
add_subdirectory(val-tests)
add_subdirectory(api-compat)
add_subdirectory(nir-tests)
add_subdirectory(extension-tests)

# .nir run tests (docs/plans/nautilus-0.4/16-implementation-plan.md, M4.0, M4.1): modules with `; run` directives that
# nautilus-nir-run compiles with a NIR backend and calls. The tbc backend runs each file as an interpreter in its default
# dispatch mode and in the portable switch mode, and in copy-and-patch mode where the build has it (a skip, status 77,
# where it does not).
if (ENABLE_TBC_BACKEND)
    file(GLOB NIR_RUN_TESTS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/data/nir-run/*.nir)
    foreach (file ${NIR_RUN_TESTS})
        get_filename_component(name ${file} NAME_WE)
        add_test(NAME nir-run-tbc-${name}
                COMMAND nautilus-nir-run --backend=tbc ${file})
        add_test(NAME nir-run-tbc-switch-${name}
                COMMAND nautilus-nir-run --backend=tbc --option=tbc.dispatch=switch ${file})
        if (ENABLE_TBC_JIT)
            add_test(NAME nir-run-tbc-jit-${name}
                    COMMAND nautilus-nir-run --backend=tbc --option=tbc.mode=jit ${file})
            set_tests_properties(nir-run-tbc-jit-${name} PROPERTIES SKIP_RETURN_CODE 77)
        endif ()
    endforeach ()
endif ()

if(ENABLE_LLVM_IR_TEST)
    add_subdirectory(llvm-ir-test)
endif()

