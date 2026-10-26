# NIR: the Nautilus IR of 0.4 (docs/plans/nautilus-0.4). Built next to the 0.3 pipeline; the engine does not use it yet.
add_library(nautilus-nir STATIC
        Opcodes.cpp
        ConstPool.cpp
        Module.cpp
        Builder.cpp
        Rewriter.cpp
        Uses.cpp
        Diagnostics.cpp
        Verifier.cpp
        VerifierFunction.cpp
        Printer.cpp
        Parser.cpp
        Canonicalize.cpp
        Image.cpp
        ImageLoader.cpp
        Inspect.cpp
        Hash.cpp
        ../common/OptionRegistry.cpp
        ../extensions/ExtensionRegistry.cpp
        ../extensions/CoreIntrinsics.cpp)
target_include_directories(nautilus-nir PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>)
# the public headers: the capture thunk template of intrinsic registration (ExceptionTransport.hpp) is header-only
target_include_directories(nautilus-nir PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../include>)
target_compile_features(nautilus-nir PUBLIC cxx_std_20)
set_target_properties(nautilus-nir PROPERTIES POSITION_INDEPENDENT_CODE ON)
# the vendored xxHash is used header-only; it is a system include so that its code is not built with our warnings
target_include_directories(nautilus-nir SYSTEM PRIVATE ${PROJECT_SOURCE_DIR}/third_party/xxhash)
