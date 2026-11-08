#pragma once

namespace nautilus::compiler::cpp {

/// The first lines of the generated source: the fixed-width integer types, defined from the compiler's own macros
/// rather than by `#include <cstdint>`. The header pulls in the C library's headers and costs about 3 ms of the 30
/// that compiling a small function takes, and the generated code uses nothing else from it. Where a later include
/// (`<exception>`) declares the same typedefs again, they name the same types.
inline constexpr const char* GENERATED_SOURCE_PREAMBLE =
    "\ntypedef __INT8_TYPE__ int8_t;\ntypedef __INT16_TYPE__ int16_t;\ntypedef __INT32_TYPE__ int32_t;\n"
    "typedef __INT64_TYPE__ int64_t;\ntypedef __UINT8_TYPE__ uint8_t;\ntypedef __UINT16_TYPE__ uint16_t;\n"
    "typedef __UINT32_TYPE__ uint32_t;\ntypedef __UINT64_TYPE__ uint64_t;\ntypedef __INTPTR_TYPE__ intptr_t;\n"
    "typedef __UINTPTR_TYPE__ uintptr_t;\n";

} // namespace nautilus::compiler::cpp
