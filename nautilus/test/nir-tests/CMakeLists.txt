add_executable(nautilus-nir-tests
        RecordTest.cpp
        ConstPoolTest.cpp
        BuilderTest.cpp
        RewriterTest.cpp
        VerifierModuleTest.cpp
        VerifierFunctionTest.cpp
        VerifierFlowTest.cpp
        PrinterTest.cpp
        ParserTest.cpp
        ConformanceTest.cpp
)

target_link_libraries(nautilus-nir-tests PRIVATE nautilus-nir Catch2::Catch2WithMain)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
catch_discover_tests(nautilus-nir-tests EXTRA_ARGS --allow-running-no-tests)

# .nir tests: a text file in the conformance corpus, run through nautilus-nir-opt by ctest
# (docs/plans/nautilus-0.4/16-implementation-plan.md, M1.3).
target_compile_definitions(nautilus-nir-tests PRIVATE NIR_CONFORMANCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../data/nir-conformance")
file(GLOB NIR_TEXT_TESTS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../data/nir-conformance/text/*.nir)
file(GLOB NIR_INVALID_TESTS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../data/nir-conformance/text/invalid/*.nir)
foreach (file ${NIR_TEXT_TESTS})
    get_filename_component(name ${file} NAME_WE)
    add_test(NAME nir-text-${name}
            COMMAND ${CMAKE_COMMAND} -DOPT=$<TARGET_FILE:nautilus-nir-opt> -DINPUT=${file} -P ${CMAKE_CURRENT_SOURCE_DIR}/check_roundtrip.cmake)
endforeach ()
foreach (file ${NIR_INVALID_TESTS})
    get_filename_component(name ${file} NAME_WE)
    add_test(NAME nir-invalid-${name}
            COMMAND ${CMAKE_COMMAND} -DOPT=$<TARGET_FILE:nautilus-nir-opt> -DINPUT=${file} -P ${CMAKE_CURRENT_SOURCE_DIR}/check_invalid.cmake)
endforeach ()
