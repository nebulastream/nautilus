
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64|i[3-6]86")
    set(ASMJIT_LOWERING_SRC X64LoweringProvider.cpp)
else()
    set(ASMJIT_LOWERING_SRC A64LoweringProvider.cpp)
endif()

add_source_files(nautilus
        AsmJitCompilationBackend.cpp
        ${ASMJIT_LOWERING_SRC}
        AsmJitExecutable.cpp
)

target_link_libraries(nautilus PRIVATE asmjit)
