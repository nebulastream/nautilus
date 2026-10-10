; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i32 @execute(i32 %0) local_unnamed_addr #0 {
  %2 = icmp sgt i32 %0, 0
  br i1 %2, label %3, label %16

3:                                                ; preds = %1
  %.not = icmp eq i32 %0, 1
  br i1 %.not, label %16, label %4

4:                                                ; preds = %3
  %5 = icmp samesign ugt i32 %0, 2
  br i1 %5, label %6, label %16

6:                                                ; preds = %4
  %.not1 = icmp eq i32 %0, 3
  br i1 %.not1, label %16, label %7

7:                                                ; preds = %6
  %8 = icmp samesign ugt i32 %0, 4
  br i1 %8, label %9, label %16

9:                                                ; preds = %7
  %.not2 = icmp eq i32 %0, 5
  br i1 %.not2, label %16, label %10

10:                                               ; preds = %9
  %11 = icmp samesign ugt i32 %0, 6
  br i1 %11, label %12, label %16

12:                                               ; preds = %10
  %.not3 = icmp eq i32 %0, 7
  br i1 %.not3, label %16, label %13

13:                                               ; preds = %12
  %14 = icmp samesign ugt i32 %0, 8
  br i1 %14, label %15, label %16

15:                                               ; preds = %13
  %.not4 = icmp eq i32 %0, 9
  %spec.select = select i1 %.not4, i32 9, i32 10
  br label %16

16:                                               ; preds = %15, %13, %12, %10, %9, %7, %6, %4, %3, %1
  %17 = phi i32 [ 0, %1 ], [ %spec.select, %15 ], [ 8, %13 ], [ 7, %12 ], [ 6, %10 ], [ 5, %9 ], [ 4, %7 ], [ 3, %6 ], [ 2, %4 ], [ 1, %3 ]
  ret i32 %17
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
