

if (ENABLE_TBC_BACKEND)
    # Outgoing external calls are marshaled through dyncall (no dyncallback:
    # the tbc backend never allocates executable memory) or, when dyncall is
    # disabled or tbc.externalCall=thunks, through the built-in
    # zero-dependency typed thunks (TBCThunkCall.cpp).
    if (ENABLE_TBC_DYNCALL)
        target_link_libraries(nautilus PRIVATE dyncall_s)
    endif ()

    add_source_files(nautilus
            TBCBackend.cpp
            TBCCode.cpp
            TBCExecutable.cpp
            TBCInterpreter.cpp
            TBCLoweringProvider.cpp
            TBCThunkCall.cpp
            TBCTrampoline.cpp
    )
endif ()
