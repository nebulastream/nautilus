; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: memory(readwrite)
declare void @runtimeFunc0() local_unnamed_addr #0

define noundef signext i32 @execute() local_unnamed_addr #1 {
  tail call void @runtimeFunc0()
  ret i32 42
}

attributes #0 = { memory(readwrite) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
