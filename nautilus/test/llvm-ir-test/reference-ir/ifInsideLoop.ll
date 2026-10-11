; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nofree norecurse nosync nounwind memory(none)
define signext i32 @execute(i32 %0) local_unnamed_addr #0 {
  %2 = icmp sgt i32 %0, 0
  br i1 %2, label %.lr.ph.preheader, label %._crit_edge

.lr.ph.preheader:                                 ; preds = %1
  %min.iters.check = icmp eq i32 %0, 1
  br i1 %min.iters.check, label %.lr.ph.preheader2, label %vector.ph

vector.ph:                                        ; preds = %.lr.ph.preheader
  %n.vec = and i32 %0, 2147483646
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i32 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %vec.phi = phi i32 [ 0, %vector.ph ], [ %3, %vector.body ]
  %3 = add i32 %vec.phi, 1
  %index.next = add nuw i32 %index, 2
  %4 = icmp eq i32 %index.next, %n.vec
  br i1 %4, label %middle.block, label %vector.body, !llvm.loop !1

middle.block:                                     ; preds = %vector.body
  %cmp.n = icmp eq i32 %0, %n.vec
  br i1 %cmp.n, label %._crit_edge, label %.lr.ph.preheader2

.lr.ph.preheader2:                                ; preds = %.lr.ph.preheader, %middle.block
  %.ph = phi i32 [ 0, %.lr.ph.preheader ], [ %n.vec, %middle.block ]
  %.ph3 = phi i32 [ 0, %.lr.ph.preheader ], [ %3, %middle.block ]
  br label %.lr.ph

.lr.ph:                                           ; preds = %.lr.ph.preheader2, %.lr.ph
  %5 = phi i32 [ %9, %.lr.ph ], [ %.ph, %.lr.ph.preheader2 ]
  %6 = phi i32 [ %spec.select, %.lr.ph ], [ %.ph3, %.lr.ph.preheader2 ]
  %7 = and i32 %5, 1
  %8 = xor i32 %7, 1
  %spec.select = add nuw nsw i32 %8, %6
  %9 = add nuw nsw i32 %5, 1
  %exitcond.not = icmp eq i32 %9, %0
  br i1 %exitcond.not, label %._crit_edge, label %.lr.ph, !llvm.loop !4

._crit_edge:                                      ; preds = %.lr.ph, %middle.block, %1
  %.lcssa = phi i32 [ 0, %1 ], [ %3, %middle.block ], [ %spec.select, %.lr.ph ]
  ret i32 %.lcssa
}

attributes #0 = { nofree norecurse nosync nounwind memory(none) }
attributes #1 = { nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
!1 = distinct !{!1, !2, !3}
!2 = !{!"llvm.loop.isvectorized", i32 1}
!3 = !{!"llvm.loop.unroll.runtime.disable"}
!4 = distinct !{!4, !2}
!5 = distinct !{!5, !2, !3}
!6 = distinct !{!6, !2}
!7 = distinct !{!7, !2, !3}
!8 = distinct !{!8, !2}
!9 = distinct !{!9, !2, !3}
!10 = distinct !{!10, !2}
