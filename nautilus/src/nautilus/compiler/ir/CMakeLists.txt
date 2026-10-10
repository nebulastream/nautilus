
add_subdirectory(blocks)
add_subdirectory(operations)
add_subdirectory(passes)
add_subdirectory(util)
#add_subdirectory(Types)

add_source_files(nautilus IRGraph.cpp IRLocationMap.cpp Usages.cpp FunctionName.cpp FunctionTable.cpp OperationEffects.cpp
        IntrinsicRegistry.cpp)
