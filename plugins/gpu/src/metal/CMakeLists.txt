target_sources(nautilus-gpu PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/MetalCompilationBackend.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/MetalLoweringProvider.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/MetalCompiler.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/MetalHostCompiler.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/MetalExecutable.cpp
)
