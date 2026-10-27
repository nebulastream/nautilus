add_source_files(nautilus
        NirBackend.cpp
        NirRuntime.cpp
)

# the pseudo-backend that compiles the NIR attached to the IR graph (tracing.nirChain); removed at the cut-over
if (ENABLE_TBC_BACKEND)
    add_source_files(nautilus NirChainBackend.cpp)
endif ()
