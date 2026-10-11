; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nofree norecurse nosync nounwind memory(none)
define signext i32 @execute(i32 %0) local_unnamed_addr #0 {
  %.not1 = icmp eq i32 %0, 0
  br i1 %.not1, label %._crit_edge, label %.lr.ph

.lr.ph:                                           ; preds = %1, %.lr.ph
  %2 = phi i32 [ %4, %.lr.ph ], [ %0, %1 ]
  %3 = phi i32 [ %5, %.lr.ph ], [ 0, %1 ]
  %4 = sdiv i32 %2, 10
  %5 = add i32 %3, 1
  %.off = add i32 %2, 9
  %.not = icmp ult i32 %.off, 19
  br i1 %.not, label %._crit_edge, label %.lr.ph

._crit_edge:                                      ; preds = %.lr.ph, %1
  %.lcssa = phi i32 [ 0, %1 ], [ %5, %.lr.ph ]
  ret i32 %.lcssa
}

attributes #0 = { nofree norecurse nosync nounwind memory(none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
