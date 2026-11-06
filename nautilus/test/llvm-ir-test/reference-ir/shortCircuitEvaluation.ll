; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: cold noreturn
declare void @nautilus_rt_throw_arith() local_unnamed_addr #0

define signext i32 @execute(i32 %0) local_unnamed_addr #1 {
  %2 = icmp eq i32 %0, 0
  br i1 %2, label %5, label %common.ret, !prof !1

common.ret:                                       ; preds = %1
  %3 = sdiv i32 10, %0
  %4 = icmp sgt i32 %3, 1
  %. = zext i1 %4 to i32
  ret i32 %.

5:                                                ; preds = %1
  tail call void @nautilus_rt_throw_arith()
  unreachable
}

attributes #0 = { cold noreturn }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
!1 = !{!"branch_weights", i32 1, i32 2000}
