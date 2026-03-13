
add_subdirectory(ArithmeticOperations)
add_subdirectory(LogicalOperations)
add_subdirectory(BinaryOperations)

add_source_files(nautilus
        Operation.cpp
        AllocaOperation.cpp
        LoadOperation.cpp
        StoreOperation.cpp
        ConstIntOperation.cpp
        ConstFloatOperation.cpp
        FunctionOperation.cpp
        IfOperation.cpp
        ProxyCallOperation.cpp
        IndirectCallOperation.cpp
        FunctionAddressOfOperation.cpp
        ReturnOperation.cpp
        CastOperation.cpp
        BranchOperation.cpp
        ConstBooleanOperation.cpp
        ConstPtrOperation.cpp
        SelectOperation.cpp
)
