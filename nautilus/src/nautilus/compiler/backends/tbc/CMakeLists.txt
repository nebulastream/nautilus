

if (ENABLE_TBC_BACKEND)
    # Outgoing external calls are marshaled through dyncall (no dyncallback:
    # the tbc backend never allocates executable memory).
    target_link_libraries(nautilus PRIVATE dyncall_s)

    add_source_files(nautilus
            TBCBackend.cpp
            TBCCode.cpp
            TBCExecutable.cpp
            TBCInterpreter.cpp
            TBCLoweringProvider.cpp
            TBCTrampoline.cpp
            TBCVm.cpp
    )

    if (ENABLE_TBC_JIT)
        # Copy-and-patch JIT execution mode (tbc.mode=jit): stitches
        # pre-compiled stencils (jit/generated/*.inc, regenerated via
        # tools/stencils/build_stencils.py) into executable memory managed by
        # asmjit's JitAllocator. TBCStencilSource.cpp is deliberately NOT
        # listed: it is the offline input to the stencil extractor.
        target_link_libraries(nautilus PRIVATE asmjit)
        add_source_files(nautilus
                jit/TBCJit.cpp
                jit/TBCStencilHelpers.cpp
                jit/TBCStencils.cpp
                jit/TBCStitcher.cpp
        )

        # Regenerate the checked-in stencil tables (requires clang >= 19 and
        # llvm-readobj; normal builds never need these).
        add_custom_target(tbc-regen-stencils
                COMMAND ${CMAKE_COMMAND} -E env python3
                ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../tools/stencils/build_stencils.py
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                COMMENT "Regenerating tbc copy-and-patch stencil tables"
                VERBATIM)
    endif ()
endif ()
