# Nautilus Runtime Options

Nautilus provides various runtime options to customize the compilation, which are passed when initializing the nautilus
engine.:

| Option                                  | Default | Description                                                                                                                                           |
|-----------------------------------------|---------|-------------------------------------------------------------------------------------------------------------------------------------------------------|
| engine.compilation=[true,false]         | true    | Activates the compilation of nautilus functions. If the flag is false, all nautilus functions will be directly invoked, which is handy for debugging. |
| engine.backend=[mlir,bc,cpp]            | mlir    | Sets the specific compilation backend for nautilus functions.                                                                                         |
| dump.all=[true,false]                   | false   | Dumbs intermediate representations all compilation steps.                                                                                             |
| dump.after_tracing=[true,false]         | false   | Dumbs traces directly after trace generation.                                                                                                         |
| dump.after_ssa=[true,false]             | false   | Dumbs traces after SSA generation.                                                                                                                    |
| dump.after_ir_creation=[true,false]     | false   | Dumbs the nautilus ir after generation.                                                                                                               |
| dump.after_mlir_generation=[true,false] | false   | Dumbs the generated mlir if the MLIR backend is used.                                                                                                 |
| dump.after_llvm_generation=[true,false] | false   | Dumbs the generated llvm if the MLIR backend is used.                                                                                                 |
| dump.after_cpp_generation=[true,false]  | false   | Dumbs the generated cpp code if the CPP backend is used.                                                                                              |
| dump.after_bc_generation=[true,false]   | false   | Dumbs the generated byte codes if the Bytecode Interpreter (BC) backend is used.                                                                      |
| dump.toConsole=[true,false]             | false   | Enables the dumping of intermediate compilation steps to the console.                                                                                 |
| dump.toFile=[true,false]                | true    | Enables the dumping of intermediate compilation steps to a temp folder.                                                                               |
| mlir.optimizationLevel=[0,1,2,3]        | 3       | Sets the optimization level for the code-generation if the MLIR backend is used.                                                                      |