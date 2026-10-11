; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nounwind
define ptr @execute(ptr %0, i32 %1, i64 %2) local_unnamed_addr #0 {
  %4 = tail call ptr @runtimeFunc0(ptr %0, i32 %1, i64 %2)
  ret ptr %4
}

; Function Attrs: nounwind memory(readwrite)
declare ptr @runtimeFunc0(ptr, i32, i64) local_unnamed_addr #1

attributes #0 = { nounwind }
attributes #1 = { nounwind memory(readwrite) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
