; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nofree norecurse nosync nounwind memory(none)
define signext i32 @execute(i32 %0) local_unnamed_addr #0 {
  br label %2

2:                                                ; preds = %2, %1
  %3 = phi i32 [ %7, %2 ], [ 0, %1 ]
  %4 = phi i32 [ %8, %2 ], [ %0, %1 ]
  %5 = mul i32 %3, 10
  %6 = srem i32 %4, 10
  %7 = add i32 %6, %5
  %8 = udiv i32 %4, 10
  %9 = icmp sgt i32 %4, 9
  br i1 %9, label %2, label %10

10:                                               ; preds = %2
  ret i32 %7
}

attributes #0 = { nofree norecurse nosync nounwind memory(none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
