
# DWARF-based variable-name resolver. Sits on top of main's
# SourceLocationResolver: that one symbolises return addresses to
# function-level frames; this one turns a (file, line, column) into a
# DW_TAG_variable name. Used as an opt-in extension to IRGraph's
# source-location dump trailer.
add_source_files(nautilus DwarfVariableResolver.cpp)

if (ENABLE_MLIR_BACKEND)
	# LLVM's DWARF/Object/BinaryFormat/MC/Support libraries are pulled
	# in transitively by the MLIR backend's MLIRTargetLLVMIRExport
	# dependency, so we don't need to re-list them here.
	#
	# Enable the DWARF code path inside DwarfVariableResolver.cpp. The
	# define is attached to the nautilus target so it's visible regardless
	# of where in the source tree the .cpp lives (the MLIR backend's own
	# add_compile_definitions call is directory-scoped).
	target_compile_definitions(nautilus PRIVATE NAUTILUS_DEBUG_HAS_DWARF)

	# LLVM is built without RTTI, so any TU that touches LLVM's class
	# hierarchy (here: llvm::ErrorInfo, DWARFDie, ...) must match or the
	# linker fails looking for typeinfo symbols LLVM never exports. Scope
	# -fno-rtti to this TU only — the rest of nautilus needs RTTI for
	# val<T> typeid lookups.
	set_source_files_properties(
			${CMAKE_CURRENT_SOURCE_DIR}/DwarfVariableResolver.cpp
			TARGET_DIRECTORY nautilus
			PROPERTIES COMPILE_OPTIONS -fno-rtti)
endif ()
