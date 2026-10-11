; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

define float @execute(float %0) local_unnamed_addr #0 {
  %2 = tail call float @runtimeFunc0(float %0)
  ret float %2
}

; Function Attrs: memory(readwrite)
declare float @runtimeFunc0(float) local_unnamed_addr #1

attributes #1 = { memory(readwrite) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
