; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nofree norecurse nosync nounwind memory(none)
define signext i32 @execute(i32 %0) local_unnamed_addr #0 {
  br label %2

2:                                                ; preds = %2, %1
  %3 = phi i32 [ %7, %2 ], [ %0, %1 ]
  %4 = phi i32 [ %6, %2 ], [ 0, %1 ]
  %5 = srem i32 %3, 10
  %6 = add i32 %4, %5
  %7 = udiv i32 %3, 10
  %8 = icmp sgt i32 %3, 9
  br i1 %8, label %2, label %9

9:                                                ; preds = %2
  ret i32 %6
}

attributes #0 = { nofree norecurse nosync nounwind memory(none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
