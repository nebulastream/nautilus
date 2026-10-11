; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nounwind
define signext i32 @execute(i32 %0) local_unnamed_addr #0 {
  %2 = icmp eq i32 %0, 1
  br i1 %2, label %3, label %5, !prof !1

3:                                                ; preds = %5, %1
  %4 = phi i32 [ %6, %5 ], [ 42, %1 ]
  ret i32 %4

5:                                                ; preds = %1
  %6 = tail call i32 @runtimeFunc0(i32 %0)
  br label %3
}

; Function Attrs: nounwind memory(readwrite)
declare i32 @runtimeFunc0(i32) local_unnamed_addr #1

attributes #0 = { nounwind }
attributes #1 = { nounwind memory(readwrite) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
!1 = !{!"branch_weights", i32 900, i32 99}
