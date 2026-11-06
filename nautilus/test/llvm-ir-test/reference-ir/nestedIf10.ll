; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i32 @execute(i32 %0) local_unnamed_addr #0 {
  %2 = icmp sgt i32 %0, 0
  br i1 %2, label %3, label %common.ret

common.ret:                                       ; preds = %15, %13, %12, %10, %9, %7, %6, %4, %3, %1
  %common.ret.op = phi i32 [ 8, %13 ], [ 0, %1 ], [ 1, %3 ], [ 2, %4 ], [ 3, %6 ], [ 4, %7 ], [ 5, %9 ], [ 6, %10 ], [ 7, %12 ], [ %., %15 ]
  ret i32 %common.ret.op

3:                                                ; preds = %1
  %.not = icmp eq i32 %0, 1
  br i1 %.not, label %common.ret, label %4

4:                                                ; preds = %3
  %5 = icmp samesign ugt i32 %0, 2
  br i1 %5, label %6, label %common.ret

6:                                                ; preds = %4
  %.not1 = icmp eq i32 %0, 3
  br i1 %.not1, label %common.ret, label %7

7:                                                ; preds = %6
  %8 = icmp samesign ugt i32 %0, 4
  br i1 %8, label %9, label %common.ret

9:                                                ; preds = %7
  %.not2 = icmp eq i32 %0, 5
  br i1 %.not2, label %common.ret, label %10

10:                                               ; preds = %9
  %11 = icmp samesign ugt i32 %0, 6
  br i1 %11, label %12, label %common.ret

12:                                               ; preds = %10
  %.not3 = icmp eq i32 %0, 7
  br i1 %.not3, label %common.ret, label %13

13:                                               ; preds = %12
  %14 = icmp samesign ugt i32 %0, 8
  br i1 %14, label %15, label %common.ret

15:                                               ; preds = %13
  %.not4 = icmp eq i32 %0, 9
  %. = select i1 %.not4, i32 9, i32 10
  br label %common.ret
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
