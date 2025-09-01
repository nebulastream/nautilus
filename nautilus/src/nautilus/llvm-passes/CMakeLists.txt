find_package(LLVM 20.1.7 CONFIG)

add_library(InliningPass SHARED FunctionInliningPass.cpp
        InliningUtils.cpp InliningUtils.h)
llvm_map_components_to_libnames(llvm_libs
        support
        core
        irreader
        passes
        analysis
        transformutils
        scalaropts
)
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

function(nautilus_inline target)
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_dependencies(${target} InliningPass)
        target_compile_options(${target} PRIVATE
                "-fpass-plugin=$<TARGET_FILE:InliningPass>"
        )
    else ()
        message(WARNING "Cannot apply inlining pass when not compiling with clang")
    endif ()
endfunction()