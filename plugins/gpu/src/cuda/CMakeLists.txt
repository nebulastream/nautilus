target_sources(nautilus-gpu PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/CUDACompilationBackend.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/CUDALoweringProvider.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/CUDACompiler.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/CUDAExecutable.cpp
)
