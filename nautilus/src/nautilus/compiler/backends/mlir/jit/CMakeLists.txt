add_source_files(nautilus
        MLIRJit.cpp
        PackFunctionArguments.cpp
)

# MLIRJit drives llvm::orc::LLJIT directly; ORC's error types (e.g.
# llvm::ErrorInfo<llvm::orc::JITDylibDefunct, llvm::ErrorInfoBase>) reference
# the typeinfo of llvm::ErrorInfoBase. MLIR itself is built with -fno-rtti,
# so its libraries do not export that typeinfo. Match the flag for this one TU
# so the emitted vtable does not reference the missing typeinfo.
# The nautilus target's sources are aggregated globally via add_source_files
# and compiled from nautilus/CMakeLists.txt, so the DIRECTORY scope must
# be the one that owns the target.  Compute that path relative to this
# file so the property also resolves when the example sub-project is
# the top-level CMake project (in which case CMAKE_SOURCE_DIR is
# example/, not the nautilus repo root).
get_filename_component(_nautilus_target_dir
        "${CMAKE_CURRENT_LIST_DIR}/../../../../../.."
        ABSOLUTE)
set_source_files_properties(
        ${CMAKE_CURRENT_SOURCE_DIR}/MLIRJit.cpp
        DIRECTORY ${_nautilus_target_dir}
        PROPERTIES COMPILE_OPTIONS -fno-rtti
)
