
if (ENABLE_MLIR_BACKEND)

    include(${CMAKE_SOURCE_DIR}/cmake/AddMLIR.cmake)

    add_source_files(nautilus
            MLIRLoweringProvider.cpp
            MLIRCompilationBackend.cpp
            MLIRExecutable.cpp
            MLIRPassManager.cpp
            LLVMIROptimizer.cpp
            JITCompiler.cpp
    )
endif ()

