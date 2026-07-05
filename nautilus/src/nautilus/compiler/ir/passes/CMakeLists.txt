
add_source_files(nautilus
        IRStatistics.cpp
        IRVerifier.cpp
        IRPassManager.cpp
        Dominators.cpp
        FunctionRewriter.cpp
        EmptyBlockEliminationPass.cpp
        ConstantFoldingAndCopyPropagationPass.cpp
        StrengthReductionPass.cpp
        DeadCodeEliminationPass.cpp
        AlgebraicSimplificationPass.cpp
        ConstantBranchFoldingPass.cpp
        BlockMergingPass.cpp
        BlockArgumentPruningPass.cpp
)
