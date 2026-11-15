# Compile-fail suite for the public headers (docs/plans/nautilus-0.4/15-agent-experience.md §4.2, plan M3.10): code that
# misuses val<T> must be rejected with a message that says what is wrong. Each file names its expectation in its first
# line; check_compile_fail.cmake compiles it with the include paths of the nautilus library.
if (ENABLE_TRACING)
    file(GLOB COMPILE_FAIL_CASES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
    foreach (file ${COMPILE_FAIL_CASES})
        get_filename_component(name ${file} NAME_WE)
        add_test(NAME compile-fail-${name}
                COMMAND ${CMAKE_COMMAND} -DCXX=${CMAKE_CXX_COMPILER}
                "-DINCLUDES=$<TARGET_PROPERTY:nautilus,INTERFACE_INCLUDE_DIRECTORIES>"
                -DSOURCE=${file} -P ${CMAKE_CURRENT_SOURCE_DIR}/check_compile_fail.cmake)
    endforeach ()
endif ()
