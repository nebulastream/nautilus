; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: cold noreturn
declare void @nautilus_rt_throw_arith() local_unnamed_addr #0

define signext i32 @execute(i32 %0, i32 %1) local_unnamed_addr #1 {
  %.not2 = icmp eq i32 %1, 0
  br i1 %.not2, label %._crit_edge, label %.lr.ph

.lr.ph:                                           ; preds = %2, %8
  %3 = phi i32 [ %4, %8 ], [ %0, %2 ]
  %4 = phi i32 [ %9, %8 ], [ %1, %2 ]
  %5 = icmp eq i32 %3, -2147483648
  %6 = icmp eq i32 %4, -1
  %7 = and i1 %6, %5
  br i1 %7, label %10, label %8, !prof !1

8:                                                ; preds = %.lr.ph
  %9 = srem i32 %3, %4
  %.not = icmp eq i32 %9, 0
  br i1 %.not, label %._crit_edge, label %.lr.ph

._crit_edge:                                      ; preds = %8, %2
  %.lcssa = phi i32 [ %0, %2 ], [ %4, %8 ]
  ret i32 %.lcssa

10:                                               ; preds = %.lr.ph
  tail call void @nautilus_rt_throw_arith()
  unreachable
}

attributes #0 = { cold noreturn }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
!1 = !{!"branch_weights", i32 1, i32 2000}
