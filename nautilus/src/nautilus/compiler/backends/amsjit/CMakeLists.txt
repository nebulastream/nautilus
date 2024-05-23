
add_source_files(nautilus
        AsmJitCompilationBackend.cpp
        A64LoweringProvider.cpp
)

target_link_libraries(nautilus PRIVATE asmjit)