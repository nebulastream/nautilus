# Debug Symbols Generation Infrastructure

## Overview

The Nautilus MLIR backend now includes optional debug symbol generation infrastructure that enables debuggers to display meaningful variable names and source locations for compiled functions.

## Architecture

### Components

1. **DebugInfoBuilder** (`nautilus/src/nautilus/compiler/backends/mlir/DebugInfoBuilder.{hpp,cpp}`)
   - Utility class for managing debug information during MLIR generation
   - Annotates operations with semantic names and source location information
   - Creates fused locations combining FileLineColLoc (file/line/column) with NameLoc (semantic names)

2. **MLIRLoweringProvider Integration**
   - Instantiates DebugInfoBuilder when the `mlir.emit_debug_symbols` option is enabled
   - Maintains clean separation: debug symbol generation is fully optional

3. **Test Infrastructure**
   - LLVMIRTestUtil enables debug symbols for all LLVM IR tests
   - Variable names tests (`VariableNamesTest.cpp`) run with debug information enabled

## Usage

### Enabling Debug Symbols

To enable debug symbol generation in your application:

```cpp
nautilus::engine::Options options;
options.setOption("mlir.emit_debug_symbols", true);
auto engine = nautilus::engine::NautilusEngine(options);
```

### How It Works

When debug symbols are enabled:

1. **Module Initialization**: The DebugInfoBuilder initializes debug metadata for the module
2. **Operation Annotation**: Each MLIR operation created during lowering can be annotated with:
   - **Semantic Name**: The meaningful variable name (e.g., "sum", "temp1", "radius")
   - **Source Location**: File name, line number, and column information
3. **Location Preservation**: MLIR's NameLoc and FileLineColLoc are preserved through MLIR→LLVM IR lowering
4. **Debug Info Flag**: LLVM IR includes the Debug Info Version metadata flag

## Implementation Details

### NameLoc + FileLineColLoc Fusion

The infrastructure creates "fused locations" that combine:
- **FileLineColLoc**: Structured source location (file, line, column)
- **NameLoc**: Semantic variable name

This allows debuggers to:
- Display meaningful variable names instead of generic names like `var_$0`
- Navigate to source locations when available
- Track values through the computation

### Example

For a function like:
```cpp
val<int32_t> sum = (a + b);
```

With debug symbols enabled, the operation is annotated with:
- **Semantic Name**: "sum"
- **Location**: Source file, line number, column

## Infrastructure for Future Enhancement

The design is extensible for future support of:
- **DICompileUnit**: Top-level compilation unit metadata
- **DISubprogram**: Function-level metadata
- **DILocalVariable**: Local variable metadata
- **DILocation**: Precise debug locations

These would enable full DWARF debug information generation once MLIR provides better support for debug metadata attributes.

## Performance Impact

- **Disabled (default)**: Zero overhead - no debug infrastructure is instantiated
- **Enabled**: Minimal overhead - only location assignment operations are added during MLIR generation
- **No runtime cost**: Debug information is only used during compilation

## Testing

Run the variable names tests with debug symbols:

```bash
./build/nautilus/test/llvm-ir-test/nautilus-llvm-ir-test [variable-names]
```

The test utility automatically enables debug symbols for all tests.

## Files Modified

1. **nautilus/src/nautilus/compiler/backends/mlir/CMakeLists.txt**
   - Added DebugInfoBuilder.cpp to build

2. **nautilus/src/nautilus/compiler/backends/mlir/MLIRLoweringProvider.hpp**
   - Added DebugInfoBuilder include
   - Added debugInfoBuilder member variable
   - Added emitDebugSymbols flag

3. **nautilus/src/nautilus/compiler/backends/mlir/MLIRLoweringProvider.cpp**
   - Initialize DebugInfoBuilder in constructor when option is enabled

4. **nautilus/test/llvm-ir-test/LLVMIRTestUtil.hpp**
   - Enable `mlir.emit_debug_symbols` option for all LLVM IR tests

## Future Work

1. **DWARF Generation**: Once MLIR improves debug metadata support, generate full DWARF debug info
2. **Call Stack Integration**: Leverage existing TagRecorder infrastructure to include call stack context
3. **Variable Tracking**: Track variable lifetime and scope through the computation
4. **Debugger Integration**: Enhance integration with LLVM debuggers (lldb, gdb) for better visualization

## References

- MLIR Location Documentation
- LLVM Debug Info
- DWARF Specification
