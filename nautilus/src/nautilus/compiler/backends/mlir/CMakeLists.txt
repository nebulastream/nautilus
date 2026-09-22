
if (ENABLE_MLIR_BACKEND)
    find_package(MLIR REQUIRED CONFIG)
    message(STATUS MLIR_INCLUDE_DIRS=${MLIR_INCLUDE_DIRS})
    target_include_directories(nautilus SYSTEM PRIVATE
            $<BUILD_INTERFACE:${MLIR_INCLUDE_DIRS}>
            $<INSTALL_INTERFACE:include/nautilus/>)
    add_compile_definitions(MLIR_COMPILER)
    get_property(mlir_libs GLOBAL PROPERTY MLIR_ALL_LIBS)
    get_property(dialect_libs GLOBAL PROPERTY MLIR_DIALECT_LIBS)
    get_property(conversion_libs GLOBAL PROPERTY MLIR_CONVERSION_LIBS)
    get_property(extension_libs GLOBAL PROPERTY MLIR_EXTENSION_LIBS)
    message(STATUS "Using MLIRConfig.cmake in: ${MLIR_DIR}")
    message(STATUS "mlir_libs: ${mlir_libs}")
    message(STATUS "dialect_libs: ${dialect_libs}")
    message(STATUS "conversion_libs: ${conversion_libs}")
    message(STATUS "extension_libs: ${extension_libs}")
    # Native codegen libs (host target backend + asm printer); used to be
    # pulled in transitively via MLIRExecutionEngine.
    llvm_map_components_to_libnames(nautilus_llvm_native_libs native)

    target_link_libraries(nautilus
            PRIVATE
            # MLIRExecutionEngine is deliberately NOT linked: Nautilus drives
            # LLJIT directly via our own MLIRJit wrapper (see jit/).
            MLIRTargetLLVMIRExport                  # translateModuleToLLVMIR
            MLIRBuiltinToLLVMIRTranslation          # registerBuiltinDialectTranslation
            MLIRLLVMToLLVMIRTranslation             # registerLLVMDialectTranslation
            MLIRExecutionEngineUtils                # makeOptimizingTransformer (OptUtils)
            MLIRFuncAllExtensions
            # LLVM libs for our direct LLJIT build.
            LLVMOrcJIT
            LLVMExecutionEngine
            LLVMJITLink                            # JITLink-based object linking
            LLVMOrcDebugging                       # GDB JIT interface; DebugInfoPreservationPlugin + createDWARFContext
            LLVMOrcTargetProcess                   # llvm_orc_registerJITLoaderPerf{Start,End,Impl} (perf jitdump)
            ${nautilus_llvm_native_libs}
            # Dialects
            MLIRMathToLLVM
            MLIRFuncToLLVM
            MLIRReconcileUnrealizedCasts
            # Debug info: LocationSnapshot in MLIRTransforms, DIScopeForLLVMFuncOp in MLIRLLVMIRTransforms.
            MLIRTransforms
            MLIRLLVMIRTransforms
    )

    add_subdirectory(debug)
    add_subdirectory(intrinsics)
    add_subdirectory(jit)
    add_source_files(nautilus
            MLIRLoweringProvider.cpp
            MLIRCompilationBackend.cpp
            MLIRExecutable.cpp
            MLIRPassManager.cpp
            LLVMIROptimizer.cpp
            JITCompiler.cpp
            LLVMBackendHooks.cpp
            PropagatePersonalityPass.cpp
    )
    # PropagatePersonalityPass subclasses mlir::PassWrapper; MLIR is built with
    # -fno-rtti and does not export typeinfo for mlir::Pass (see debug/).
    get_filename_component(_nautilus_target_dir
            "${CMAKE_CURRENT_LIST_DIR}/../../../../.."
            ABSOLUTE)
    set_source_files_properties(
            ${CMAKE_CURRENT_SOURCE_DIR}/PropagatePersonalityPass.cpp
            DIRECTORY ${_nautilus_target_dir}
            PROPERTIES COMPILE_OPTIONS -fno-rtti
    )
endif ()

