

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
endif ()
