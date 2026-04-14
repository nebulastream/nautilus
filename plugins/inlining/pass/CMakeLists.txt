
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
            COMPILE_OPTIONS "-Wno-unused-parameter;-Wno-unused-variable;-Wno-extra-semi;-Wno-deprecated-copy-with-dtor;-stdlib=libstdc++;-fno-sanitize=all"
            LINK_OPTIONS "-stdlib=libstdc++;-fno-sanitize=all"
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
