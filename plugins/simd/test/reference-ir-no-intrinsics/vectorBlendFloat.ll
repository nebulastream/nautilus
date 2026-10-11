; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

define void @execute(ptr %0, ptr %1, ptr %2, ptr %3) local_unnamed_addr #0 {
  %5 = tail call ptr @runtimeFunc0(ptr %0)
  %6 = tail call ptr @runtimeFunc0(ptr %1)
  %7 = tail call ptr @runtimeFunc0(ptr %2)
  %8 = tail call ptr @runtimeFunc1(ptr %7, ptr %5, ptr %6)
  tail call void @runtimeFunc2(ptr %3, ptr %8)
  ret void
}

; Function Attrs: memory(readwrite)
declare ptr @runtimeFunc0(ptr) local_unnamed_addr #1

; Function Attrs: memory(readwrite)
declare ptr @runtimeFunc1(ptr, ptr, ptr) local_unnamed_addr #1

; Function Attrs: memory(readwrite)
declare void @runtimeFunc2(ptr, ptr) local_unnamed_addr #1

attributes #1 = { memory(readwrite) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
