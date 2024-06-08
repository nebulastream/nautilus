# Nautilus: Demo JIT example

This example project illustrates how Nautilus can be integrated into a C++ project using CMAKE.

## Integrate as sub project

Nautilus is self-contained and requires no external pre-installed dependencies.
In this example, integrate nautilus as a subproject:

```cmake
# add the nautilus source dir as a subdirectory to the main cmake project.
# ${CMAKE_CURRENT_SOURCE_DIR}/.. should be the path to the nautilus source dir
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_BINARY_DIR}/nautilus)
# add the nautilus target to your project
target_link_libraries(myTarget PRIVATE nautilus)
```

## Example

The following [example](./example/) illustrates Nautilus on a simplified aggregation operator.
`conditionalSum` iterates over an array and only aggregates the values for, which mask is true.
Nautilus traces the function and generates efficient code using one of its compilation backend.
These produce either byte code, [mlir](), or[c++]() code.

```c++
val<int32_t> conditionalSum(val<int32_t> size, val<bool*> mask, val<int32_t*> array) {
  val<int32_t> sum = 0;
  for (val<int32_t> i = 0; i < size; i++) {
    // check mask
    if (mask[i]) {
        // load value from array at position i
        val<int32_t> value = array[i];
        // add value to sum
        sum += value;
    }
  }
  return sum;
}

int main(int, char* []) {
    engine::Options options;
    // select the compilation backend, e.g. mlir, cpp, or bc
    options.setOption("engine.backend", "mlir");
    auto engine = engine::NautilusEngine(options);
    // register a function pointer to nautilus.
    auto function = engine.registerFunction(conditionalSum);
    auto mask = new bool[4]{true, true, false, true};
    auto array = new int32_t[4] {1, 2, 3, 4};
    auto result = function(4, mask, array);
    // result is 7
    std::cout << "Result: " << result << std::endl;
    return 0;
}
```

<details open>
<summary>
Nautilus Intermediate Representation
</summary>

```
IR {
execute() {
Block_0($1:i32, $2:ptr, $3:ptr):
    $4 = 0 :i32
    $5 = 0 :i32
    br Block_5($4, $5, $1, $3, $2) :void

Block_5($4:i32, $5:i32, $1:i32, $3:ptr, $2:ptr):
    $6 = $5 < $1 :bool
    if $6 ? Block_1($1, $5, $4, $3, $2) : Block_2($4) :void

Block_1($1:i32, $5:i32, $4:i32, $3:ptr, $2:ptr):
    $10 = 1 :i32
    $11 = $5 * $10 :i32
    $12 = $2 + $11 :ptr
    $13 = load($12) :bool
    if $13 ? Block_3($1, $5, $4, $3, $2) : Block_4($4, $1, $5, $3, $2) :void

Block_3($1:i32, $5:i32, $4:i32, $3:ptr, $2:ptr):
    $17 = 4 :i32
    $18 = $5 * $17 :i32
    $19 = $3 + $18 :ptr
    $20 = load($19) :i32
    $22 = $4 + $20 :i32
    br Block_6($22, $1, $5, $3, $2) :void

Block_6($4:i32, $1:i32, $5:i32, $3:ptr, $2:ptr):
    $24 = 1 :i32
    $25 = $5 + $24 :i32
    br Block_5($4, $25, $1, $3, $2) :void

Block_4($4:i32, $1:i32, $5:i32, $3:ptr, $2:ptr):
    br Block_6($4, $1, $5, $3, $2) :void

Block_2($4:i32):
    return ($4) :i32
}
}
```

</details>

<details open>
<summary>
Generated MLIR
</summary>

```mlir
module {
  func.func @execute(%arg0: i32, %arg1: !llvm.ptr, %arg2: !llvm.ptr) -> i32 attributes {llvm.emit_c_interface} {
    %c0_i32 = arith.constant 0 : i32
    %c0_i32_0 = arith.constant 0 : i32
    cf.br ^bb1(%c0_i32, %c0_i32_0, %arg0, %arg2, %arg1 : i32, i32, i32, !llvm.ptr, !llvm.ptr)
  ^bb1(%0: i32, %1: i32, %2: i32, %3: !llvm.ptr, %4: !llvm.ptr):  // 2 preds: ^bb0, ^bb4
    %5 = arith.cmpi slt, %1, %2 : i32
    cf.cond_br %5, ^bb2(%2, %1, %0, %3, %4 : i32, i32, i32, !llvm.ptr, !llvm.ptr), ^bb6(%0 : i32)
  ^bb2(%6: i32, %7: i32, %8: i32, %9: !llvm.ptr, %10: !llvm.ptr):  // pred: ^bb1
    %c1_i32 = arith.constant 1 : i32
    %11 = llvm.mul %7, %c1_i32  : i32
    %12 = llvm.getelementptr %10[%11] : (!llvm.ptr, i32) -> !llvm.ptr, i8
    %13 = llvm.load %12 : !llvm.ptr -> i1
    cf.cond_br %13, ^bb3(%6, %7, %8, %9, %10 : i32, i32, i32, !llvm.ptr, !llvm.ptr), ^bb5(%8, %6, %7, %9, %10 : i32, i32, i32, !llvm.ptr, !llvm.ptr)
  ^bb3(%14: i32, %15: i32, %16: i32, %17: !llvm.ptr, %18: !llvm.ptr):  // pred: ^bb2
    %c4_i32 = arith.constant 4 : i32
    %19 = llvm.mul %15, %c4_i32  : i32
    %20 = llvm.getelementptr %17[%19] : (!llvm.ptr, i32) -> !llvm.ptr, i8
    %21 = llvm.load %20 : !llvm.ptr -> i32
    %22 = llvm.add %16, %21  : i32
    cf.br ^bb4(%22, %14, %15, %17, %18 : i32, i32, i32, !llvm.ptr, !llvm.ptr)
  ^bb4(%23: i32, %24: i32, %25: i32, %26: !llvm.ptr, %27: !llvm.ptr):  // 2 preds: ^bb3, ^bb5
    %c1_i32_1 = arith.constant 1 : i32
    %28 = llvm.add %25, %c1_i32_1  : i32
    cf.br ^bb1(%23, %28, %24, %26, %27 : i32, i32, i32, !llvm.ptr, !llvm.ptr)
  ^bb5(%29: i32, %30: i32, %31: i32, %32: !llvm.ptr, %33: !llvm.ptr):  // pred: ^bb2
    cf.br ^bb4(%29, %30, %31, %32, %33 : i32, i32, i32, !llvm.ptr, !llvm.ptr)
  ^bb6(%34: i32):  // pred: ^bb1
    llvm.return %34 : i32
  }
}
```
</details>

<details open>

<summary>
Generated C++ code
</summary>

```C++
#include <cstdint>
extern "C" int32_t execute(int32_t var_$1, uint8_t* var_$2, uint8_t* var_$3) {
  // variable declarations
  int32_t var_$4;
  int32_t var_$5;
  bool var_$6;
  int32_t var_$10;
  int32_t var_$11;
  uint8_t* var_$12;
  bool var_$13;
  int32_t var_$17;
  int32_t var_$18;
  uint8_t* var_$19;
  int32_t var_$20;
  int32_t var_$22;
  int32_t var_$24;
  int32_t var_$25;
  // basic blocks
  Block_0:
    var_$4 = (int32_t) 0;
    var_$5 = (int32_t) 0;
    // prepare block arguments
    {
        int32_t temp_0 = var_$4;
        int32_t temp_1 = var_$5;
        int32_t temp_2 = var_$1;
        uint8_t* temp_3 = var_$3;
        uint8_t* temp_4 = var_$2;
        var_$4 = temp_0;
        var_$5 = temp_1;
        var_$1 = temp_2;
        var_$3 = temp_3;
        var_$2 = temp_4;
    }
    goto Block_5;
Block_5:
    var_$6 = var_$5 < var_$1;
    if (var_$6) {
      // prepare block arguments
      {
        int32_t temp_0 = var_$1;
        int32_t temp_1 = var_$5;
        int32_t temp_2 = var_$4;
        uint8_t* temp_3 = var_$3;
        uint8_t* temp_4 = var_$2;
        var_$1 = temp_0;
        var_$5 = temp_1;
        var_$4 = temp_2;
        var_$3 = temp_3;
        var_$2 = temp_4;
      }
      goto Block_1;
    } else {
        // prepare block arguments
        {
            int32_t temp_0 = var_$4;
            var_$4 = temp_0;
        }
        goto Block_2;
    }

Block_1:
    var_$10 = (int32_t) 1;
    var_$11 = var_$5 * var_$10;
    var_$12 = var_$2 + var_$11;
    var_$13 = *reinterpret_cast<bool*>(var_$12);
    if (var_$13) {
        // prepare block arguments
        {
            int32_t temp_0 = var_$1;
            int32_t temp_1 = var_$5;
            int32_t temp_2 = var_$4;
            uint8_t* temp_3 = var_$3;
            uint8_t* temp_4 = var_$2;
            var_$1 = temp_0;
            var_$5 = temp_1;
            var_$4 = temp_2;
            var_$3 = temp_3;
            var_$2 = temp_4;
        }
        goto Block_3;
    } else {
        // prepare block arguments
        {
            int32_t temp_0 = var_$4;
            int32_t temp_1 = var_$1;
            int32_t temp_2 = var_$5;
            uint8_t* temp_3 = var_$3;
            uint8_t* temp_4 = var_$2;
            var_$4 = temp_0;
            var_$1 = temp_1;
            var_$5 = temp_2;
            var_$3 = temp_3;
            var_$2 = temp_4;
        }
        goto Block_4;
    }

Block_3:
    var_$17 = (int32_t) 4;
    var_$18 = var_$5 * var_$17;
    var_$19 = var_$3 + var_$18;
    var_$20 = *reinterpret_cast<int32_t*>(var_$19);
    var_$22 = var_$4 + var_$20;
    // prepare block arguments
    {
        int32_t temp_0 = var_$22;
        int32_t temp_1 = var_$1;
        int32_t temp_2 = var_$5;
        uint8_t* temp_3 = var_$3;
        uint8_t* temp_4 = var_$2;
        var_$4 = temp_0;
        var_$1 = temp_1;
        var_$5 = temp_2;
        var_$3 = temp_3;
        var_$2 = temp_4;
    }
    goto Block_6;

Block_6:
    var_$24 = (int32_t) 1;
    var_$25 = var_$5 + var_$24;
    // prepare block arguments
    {
        int32_t temp_0 = var_$4;
        int32_t temp_1 = var_$25;
        int32_t temp_2 = var_$1;
        uint8_t* temp_3 = var_$3;
        uint8_t* temp_4 = var_$2;
        var_$4 = temp_0;
        var_$5 = temp_1;
        var_$1 = temp_2;
        var_$3 = temp_3;
        var_$2 = temp_4;
    }
    goto Block_5;

Block_4:
    // prepare block arguments
    {
        int32_t temp_0 = var_$4;
        int32_t temp_1 = var_$1;
        int32_t temp_2 = var_$5;
        uint8_t* temp_3 = var_$3;
        uint8_t* temp_4 = var_$2;
        var_$4 = temp_0;
        var_$1 = temp_1;
        var_$5 = temp_2;
        var_$3 = temp_3;
        var_$2 = temp_4;
    }
    goto Block_6;
    
    Block_2:
        return var_$4;
}```

</details>