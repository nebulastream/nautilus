; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nofree norecurse nosync nounwind memory(none)
define signext i32 @execute(i32 %0) local_unnamed_addr #0 {
  %2 = icmp sgt i32 %0, 0
  br i1 %2, label %.lr.ph.preheader, label %._crit_edge

.lr.ph.preheader:                                 ; preds = %1
  %min.iters.check = icmp ult i32 %0, 8
  br i1 %min.iters.check, label %.lr.ph.preheader2, label %vector.ph

vector.ph:                                        ; preds = %.lr.ph.preheader
  %n.vec = and i32 %0, 2147483640
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i32 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %vec.ind = phi <4 x i32> [ <i32 0, i32 1, i32 2, i32 3>, %vector.ph ], [ %vec.ind.next, %vector.body ]
  %vec.phi = phi <4 x i32> [ zeroinitializer, %vector.ph ], [ %7, %vector.body ]
  %vec.phi1 = phi <4 x i32> [ zeroinitializer, %vector.ph ], [ %8, %vector.body ]
  %3 = and <4 x i32> %vec.ind, splat (i32 1)
  %4 = and <4 x i32> %vec.ind, splat (i32 1)
  %5 = xor <4 x i32> %3, splat (i32 1)
  %6 = xor <4 x i32> %4, splat (i32 1)
  %7 = add <4 x i32> %5, %vec.phi
  %8 = add <4 x i32> %6, %vec.phi1
  %index.next = add nuw i32 %index, 8
  %vec.ind.next = add <4 x i32> %vec.ind, splat (i32 8)
  %9 = icmp eq i32 %index.next, %n.vec
  br i1 %9, label %middle.block, label %vector.body, !llvm.loop !1

middle.block:                                     ; preds = %vector.body
  %bin.rdx = add <4 x i32> %8, %7
  %10 = tail call i32 @llvm.vector.reduce.add.v4i32(<4 x i32> %bin.rdx)
  %cmp.n = icmp eq i32 %0, %n.vec
  br i1 %cmp.n, label %._crit_edge, label %.lr.ph.preheader2

.lr.ph.preheader2:                                ; preds = %.lr.ph.preheader, %middle.block
  %.ph = phi i32 [ 0, %.lr.ph.preheader ], [ %n.vec, %middle.block ]
  %.ph3 = phi i32 [ 0, %.lr.ph.preheader ], [ %10, %middle.block ]
  br label %.lr.ph

.lr.ph:                                           ; preds = %.lr.ph.preheader2, %.lr.ph
  %11 = phi i32 [ %15, %.lr.ph ], [ %.ph, %.lr.ph.preheader2 ]
  %12 = phi i32 [ %spec.select, %.lr.ph ], [ %.ph3, %.lr.ph.preheader2 ]
  %13 = and i32 %11, 1
  %14 = xor i32 %13, 1
  %spec.select = add i32 %14, %12
  %15 = add nuw nsw i32 %11, 1
  %exitcond.not = icmp eq i32 %15, %0
  br i1 %exitcond.not, label %._crit_edge, label %.lr.ph, !llvm.loop !4

._crit_edge:                                      ; preds = %.lr.ph, %middle.block, %1
  %.lcssa = phi i32 [ 0, %1 ], [ %10, %middle.block ], [ %spec.select, %.lr.ph ]
  ret i32 %.lcssa
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i32 @llvm.vector.reduce.add.v4i32(<4 x i32>) #1

attributes #0 = { nofree norecurse nosync nounwind memory(none) }
attributes #1 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
!1 = distinct !{!1, !2, !3}
!2 = !{!"llvm.loop.isvectorized", i32 1}
!3 = !{!"llvm.loop.unroll.runtime.disable"}
!4 = distinct !{!4, !3, !2}
