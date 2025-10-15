set(NAUTILUS_INLINE_SUPPORTED FALSE)

function(is_inlining_supported result_var)
    message(STATUS "C++ compiler ID: ${CMAKE_CXX_COMPILER_ID}")

    # Check clang version
    # right now, inlining is restricted to clang-19, but it can probably also support other clang versions without changes to the source code
    # however, this should be validated first, as the llvm pass API changes sometimes
    if (NOT (CMAKE_CXX_COMPILER_ID MATCHES "Clang"
            AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "19.0.0"
            AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "20"))
        set(${result_var} FALSE PARENT_SCOPE)
        return()
    endif ()

    set(${result_var} TRUE PARENT_SCOPE)
endfunction()


is_inlining_supported(NAUTILUS_INLINE_SUPPORTED)
if (NAUTILUS_INLINE_SUPPORTED)
    # Match llvm to the clang version
    find_package(LLVM ${CMAKE_CXX_COMPILER_VERSION} CONFIG REQUIRED)
    add_library(InliningPass SHARED FunctionInliningPass.cpp InliningUtils.cpp)

    target_include_directories(InliningPass PRIVATE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<INSTALL_INTERFACE:src/nautilus/>)

    target_include_directories(InliningPass PRIVATE ${LLVM_INCLUDE_DIRS})
    target_compile_definitions(InliningPass PRIVATE ${LLVM_DEFINITIONS})
    set_target_properties(InliningPass PROPERTIES
            COMPILE_OPTIONS "-Wno-unused-parameter;-Wno-unused-variable;-Wno-extra-semi;-Wno-deprecated-copy-with-dtor;-stdlib=libstdc++"
            LINK_OPTIONS "-stdlib=libstdc++"
    ) #override global compile and link flags for the pass

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
    is_inlining_supported(NAUTILUS_INLINE_SUPPORTED)
    if (NAUTILUS_INLINE_SUPPORTED)
        add_dependencies(${target} InliningPassDependency)
        target_compile_options(${target} PRIVATE
                "-fpass-plugin=$<TARGET_FILE:InliningPass>"
        )
    else ()
        message(WARNING "Function inlining requires clang 19 during compilation. Probably also works with other clang versions. Adjust the version-check in CMake and find out")
    endif ()
endfunction()
