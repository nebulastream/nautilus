set(NAUTILUS_INLINE_SUPPORTED FALSE)


function(does_clang_support_inline_pass result_var)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang"
            AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "19.0.0"
            AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "20")
        set(${result_var} TRUE PARENT_SCOPE)
    else()
        set(${result_var} FALSE PARENT_SCOPE)
    endif()
endfunction()


does_clang_support_inline_pass(NAUTILUS_INLINE_SUPPORTED)
if(NAUTILUS_INLINE_SUPPORTED)
    # Match llvm to the clang version
    find_package(LLVM ${CMAKE_CXX_COMPILER_VERSION} CONFIG REQUIRED)
    add_library(InliningPass SHARED FunctionInliningPass.cpp
            InliningUtils.cpp InliningUtils.h)

    target_include_directories(InliningPass PRIVATE ${LLVM_INCLUDE_DIRS})
    target_compile_definitions(InliningPass PRIVATE ${LLVM_DEFINITIONS})
    set_target_properties(InliningPass PROPERTIES
            COMPILE_OPTIONS "-Wno-unused-parameter;-Wno-unused-variable;-Wno-extra-semi;-Wno-deprecated-copy-with-dtor"
            LINK_OPTIONS ""
    ) #override global compile and link flags for the pass

    target_include_directories(InliningPass PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include/nautilus/>)
    target_include_directories(InliningPass PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
            $<INSTALL_INTERFACE:include/>)
    target_include_directories(InliningPass PRIVATE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
            $<INSTALL_INTERFACE:src/nautilus/>)

    target_link_libraries(InliningPass
            "$<$<PLATFORM_ID:Darwin>:-undefined dynamic_lookup>")

    set_target_properties(InliningPass PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    )

    add_custom_target(InliningPassDependency
            DEPENDS $<TARGET_FILE:InliningPass>
    )
endif ()

function(nautilus_inline target)
    does_clang_support_inline_pass(NAUTILUS_INLINE_SUPPORTED)
    if (NAUTILUS_INLINE_SUPPORTED)
        add_dependencies(${target} InliningPassDependency)
        target_compile_options(${target} PRIVATE
                "-fpass-plugin=$<TARGET_FILE:InliningPass>"
        )
    else()
        message(WARNING "Function inlining requires clang 19 during compilation. Probably also works with other clang versions. Adjust the version-check in CMake and find out")
    endif ()
endfunction()
