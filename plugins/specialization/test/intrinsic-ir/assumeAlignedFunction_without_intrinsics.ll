; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

define signext i32 @execute(ptr %0) local_unnamed_addr #0 {
  tail call void @runtimeFunc0(ptr %0, i8 16)
  %2 = load i32, ptr %0, align 4
  ret i32 %2
}

; Function Attrs: memory(readwrite)
declare void @runtimeFunc0(ptr, i8 signext) local_unnamed_addr #1

attributes #1 = { memory(readwrite) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
