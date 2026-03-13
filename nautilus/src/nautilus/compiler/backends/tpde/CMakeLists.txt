
add_source_files(nautilus
	TPDECompilationBackend.cpp
	TPDEExecutable.cpp
	NautilusIRAdaptor.cpp
	NautilusCompiler.cpp
)

target_link_libraries(nautilus PRIVATE $<BUILD_INTERFACE:tpde>)

# FE_MEM (fadec) uses aggregate initialization without nested braces; suppress the warning
set_source_files_properties(NautilusCompiler.cpp PROPERTIES
    COMPILE_OPTIONS "-Wno-missing-braces"
)
