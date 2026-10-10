; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nounwind
define zeroext i64 @execute(i64 %0, i64 %1) local_unnamed_addr #0 {
  %3 = tail call i64 @runtimeFunc0(i64 %0, i64 %1)
  ret i64 %3
}

; Function Attrs: nounwind memory(readwrite)
declare i64 @runtimeFunc0(i64, i64) local_unnamed_addr #1

attributes #0 = { nounwind }
attributes #1 = { nounwind memory(readwrite) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
