; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

define signext i32 @execute(i32 %0, i32 %1) local_unnamed_addr #0 {
  %3 = icmp sgt i32 %0, 0
  %4 = icmp sgt i32 %1, 0
  %5 = and i1 %3, %4
  tail call void @runtimeFunc0(i1 %5)
  %6 = add i32 %1, %0
  ret i32 %6
}

; Function Attrs: memory(readwrite)
declare void @runtimeFunc0(i1 zeroext) local_unnamed_addr #1

attributes #1 = { memory(readwrite) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
