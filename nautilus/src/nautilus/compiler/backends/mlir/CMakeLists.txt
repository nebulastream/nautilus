
if (ENABLE_MLIR_BACKEND)
    find_package(MLIR REQUIRED CONFIG)
    message(STATUS MLIR_INCLUDE_DIRS=${MLIR_INCLUDE_DIRS})
    target_include_directories(nautilus SYSTEM PRIVATE
            $<BUILD_INTERFACE:${MLIR_INCLUDE_DIRS}>
            $<INSTALL_INTERFACE:include/nebulastream/>)
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
    target_link_libraries(nautilus
            PRIVATE
            MLIRExecutionEngine
            MLIRFuncAllExtensions
            # Dialects
            MLIRFuncToLLVM
    )
    
    add_subdirectory(intrinsics)    
    add_source_files(nautilus
            MLIRLoweringProvider.cpp
            MLIRCompilationBackend.cpp
            MLIRExecutable.cpp
            MLIRPassManager.cpp
            LLVMIROptimizer.cpp
            JITCompiler.cpp
            LLVMInliningUtils.cpp
    )
endif ()

