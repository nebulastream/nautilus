; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nofree norecurse nosync nounwind memory(argmem: read)
define signext i32 @execute(ptr readonly %0, i32 %1) local_unnamed_addr #0 {
  %3 = icmp sgt i32 %1, 0
  br i1 %3, label %.lr.ph.preheader, label %._crit_edge

.lr.ph.preheader:                                 ; preds = %2
  %wide.trip.count = zext nneg i32 %1 to i64
  %min.iters.check = icmp eq i32 %1, 1
  br i1 %min.iters.check, label %.lr.ph.preheader6, label %vector.ph

vector.ph:                                        ; preds = %.lr.ph.preheader
  %n.vec = and i64 %wide.trip.count, 2147483646
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i64 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %vec.phi = phi i32 [ 0, %vector.ph ], [ %11, %vector.body ]
  %vec.phi5 = phi i32 [ 0, %vector.ph ], [ %12, %vector.body ]
  %4 = shl nuw nsw i64 %index, 2
  %5 = shl i64 %index, 2
  %6 = getelementptr i8, ptr %0, i64 %4
  %7 = getelementptr i8, ptr %0, i64 %5
  %8 = getelementptr i8, ptr %7, i64 4
  %9 = load i32, ptr %6, align 4
  %10 = load i32, ptr %8, align 4
  %11 = add i32 %9, %vec.phi
  %12 = add i32 %10, %vec.phi5
  %index.next = add nuw i64 %index, 2
  %13 = icmp eq i64 %index.next, %n.vec
  br i1 %13, label %middle.block, label %vector.body, !llvm.loop !1

middle.block:                                     ; preds = %vector.body
  %bin.rdx = add i32 %12, %11
  %cmp.n = icmp eq i64 %n.vec, %wide.trip.count
  br i1 %cmp.n, label %._crit_edge, label %.lr.ph.preheader6

.lr.ph.preheader6:                                ; preds = %.lr.ph.preheader, %middle.block
  %indvars.iv.ph = phi i64 [ 0, %.lr.ph.preheader ], [ %n.vec, %middle.block ]
  %.ph = phi i32 [ 0, %.lr.ph.preheader ], [ %bin.rdx, %middle.block ]
  br label %.lr.ph

.lr.ph:                                           ; preds = %.lr.ph.preheader6, %.lr.ph
  %indvars.iv = phi i64 [ %indvars.iv.next, %.lr.ph ], [ %indvars.iv.ph, %.lr.ph.preheader6 ]
  %14 = phi i32 [ %18, %.lr.ph ], [ %.ph, %.lr.ph.preheader6 ]
  %15 = shl nuw nsw i64 %indvars.iv, 2
  %16 = getelementptr i8, ptr %0, i64 %15
  %17 = load i32, ptr %16, align 4
  %18 = add nsw i32 %17, %14
  %indvars.iv.next = add nuw nsw i64 %indvars.iv, 1
  %exitcond.not = icmp eq i64 %indvars.iv.next, %wide.trip.count
  br i1 %exitcond.not, label %._crit_edge, label %.lr.ph, !llvm.loop !4

._crit_edge:                                      ; preds = %.lr.ph, %middle.block, %2
  %.lcssa = phi i32 [ 0, %2 ], [ %bin.rdx, %middle.block ], [ %18, %.lr.ph ]
  ret i32 %.lcssa
}

attributes #0 = { nofree norecurse nosync nounwind memory(argmem: read) }
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
