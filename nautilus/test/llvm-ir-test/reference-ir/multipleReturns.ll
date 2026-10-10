; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i32 @execute(i32 %0) local_unnamed_addr #0 {
  %2 = icmp eq i32 %0, 1
  br i1 %2, label %3, label %5

3:                                                ; preds = %7, %5, %1
  %4 = phi i32 [ %9, %7 ], [ 42, %5 ], [ 1, %1 ]
  ret i32 %4

5:                                                ; preds = %1
  %6 = icmp slt i32 %0, 10
  br i1 %6, label %3, label %7

7:                                                ; preds = %5
  %8 = shl nuw i32 %0, 1
  %9 = or disjoint i32 %8, 1
  br label %3
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
