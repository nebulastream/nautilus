; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nofree norecurse nosync nounwind memory(none)
define signext i32 @execute(i32 %0) local_unnamed_addr #0 {
  br label %2

2:                                                ; preds = %2, %1
  %3 = phi i32 [ %10, %2 ], [ %0, %1 ]
  %4 = phi i32 [ %9, %2 ], [ 1, %1 ]
  %5 = phi i32 [ %8, %2 ], [ 0, %1 ]
  %6 = srem i32 %3, 2
  %7 = mul i32 %6, %4
  %8 = add i32 %5, %7
  %9 = mul i32 %4, 10
  %10 = lshr i32 %3, 1
  %11 = icmp sgt i32 %3, 1
  br i1 %11, label %2, label %12

12:                                               ; preds = %2
  ret i32 %8
}

attributes #0 = { nofree norecurse nosync nounwind memory(none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
