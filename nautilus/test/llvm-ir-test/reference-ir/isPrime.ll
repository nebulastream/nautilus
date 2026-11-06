; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: cold noreturn
declare void @nautilus_rt_throw_arith() local_unnamed_addr #0

define noundef zeroext i1 @execute(i32 %0) local_unnamed_addr #1 {
  %2 = icmp slt i32 %0, 2
  br i1 %2, label %common.ret, label %.preheader

.preheader:                                       ; preds = %1
  %.not2 = icmp samesign ult i32 %0, 4
  br i1 %.not2, label %common.ret, label %.lr.ph

3:                                                ; preds = %12
  %4 = or disjoint i32 %10, 1
  %5 = mul i32 %4, %4
  %.not = icmp sgt i32 %5, %0
  br i1 %.not, label %common.ret, label %.lr.ph.1

.lr.ph.1:                                         ; preds = %3
  %6 = srem i32 %0, %4
  %.not6.not.1 = icmp eq i32 %6, 0
  br i1 %.not6.not.1, label %common.ret, label %7

7:                                                ; preds = %.lr.ph.1
  %8 = add i32 %10, 2
  %9 = mul i32 %8, %8
  %.not.1 = icmp sgt i32 %9, %0
  br i1 %.not.1, label %common.ret, label %.lr.ph

.lr.ph:                                           ; preds = %.preheader, %7
  %10 = phi i32 [ %8, %7 ], [ 2, %.preheader ]
  %11 = icmp eq i32 %10, 0
  br i1 %11, label %14, label %12, !prof !1

12:                                               ; preds = %.lr.ph
  %13 = srem i32 %0, %10
  %.not6.not = icmp eq i32 %13, 0
  br i1 %.not6.not, label %common.ret, label %3

common.ret:                                       ; preds = %3, %12, %.lr.ph.1, %7, %.preheader, %1
  %common.ret.op = phi i1 [ false, %1 ], [ true, %.preheader ], [ true, %3 ], [ false, %12 ], [ false, %.lr.ph.1 ], [ true, %7 ]
  ret i1 %common.ret.op

14:                                               ; preds = %.lr.ph
  tail call void @nautilus_rt_throw_arith()
  unreachable
}

attributes #0 = { cold noreturn }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
!1 = !{!"branch_weights", i32 1, i32 2000}
