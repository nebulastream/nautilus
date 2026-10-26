# NIR: the Nautilus IR of 0.4 (docs/plans/nautilus-0.4). Built next to the 0.3 pipeline; the engine does not use it yet.
add_library(nautilus-nir STATIC
        Opcodes.cpp
        ConstPool.cpp
        Module.cpp
        Builder.cpp
        Rewriter.cpp
        Uses.cpp
        Diagnostics.cpp
        Verifier.cpp)
target_include_directories(nautilus-nir PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../..)
target_compile_features(nautilus-nir PUBLIC cxx_std_20)
set_target_properties(nautilus-nir PROPERTIES POSITION_INDEPENDENT_CODE ON)
