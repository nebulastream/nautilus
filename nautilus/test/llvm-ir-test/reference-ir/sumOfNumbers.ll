; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nofree norecurse nosync nounwind memory(none)
define signext i32 @execute(i32 %0) local_unnamed_addr #0 {
  %.not1 = icmp slt i32 %0, 1
  br i1 %.not1, label %._crit_edge, label %.lr.ph.preheader

.lr.ph.preheader:                                 ; preds = %1
  %2 = add nsw i32 %0, -2147483647
  %or.cond = icmp ult i32 %2, -2147483627
  br i1 %or.cond, label %.lr.ph.preheader7, label %vector.ph

vector.ph:                                        ; preds = %.lr.ph.preheader
  %n.vec = and i32 %0, 2147483640
  %3 = or disjoint i32 %n.vec, 1
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i32 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %vec.ind = phi <4 x i32> [ <i32 1, i32 2, i32 3, i32 4>, %vector.ph ], [ %vec.ind.next, %vector.body ]
  %vec.phi = phi <4 x i32> [ zeroinitializer, %vector.ph ], [ %4, %vector.body ]
  %vec.phi5 = phi <4 x i32> [ zeroinitializer, %vector.ph ], [ %5, %vector.body ]
  %step.add = add <4 x i32> %vec.ind, splat (i32 4)
  %4 = add <4 x i32> %vec.ind, %vec.phi
  %5 = add <4 x i32> %step.add, %vec.phi5
  %index.next = add nuw i32 %index, 8
  %vec.ind.next = add <4 x i32> %vec.ind, splat (i32 8)
  %6 = icmp eq i32 %index.next, %n.vec
  br i1 %6, label %middle.block, label %vector.body, !llvm.loop !1

middle.block:                                     ; preds = %vector.body
  %bin.rdx = add <4 x i32> %5, %4
  %7 = tail call i32 @llvm.vector.reduce.add.v4i32(<4 x i32> %bin.rdx)
  %cmp.n = icmp eq i32 %0, %n.vec
  br i1 %cmp.n, label %._crit_edge, label %.lr.ph.preheader7

.lr.ph.preheader7:                                ; preds = %.lr.ph.preheader, %middle.block
  %.ph = phi i32 [ 1, %.lr.ph.preheader ], [ %3, %middle.block ]
  %.ph8 = phi i32 [ 0, %.lr.ph.preheader ], [ %7, %middle.block ]
  br label %.lr.ph

.lr.ph:                                           ; preds = %.lr.ph.preheader7, %.lr.ph
  %8 = phi i32 [ %11, %.lr.ph ], [ %.ph, %.lr.ph.preheader7 ]
  %9 = phi i32 [ %10, %.lr.ph ], [ %.ph8, %.lr.ph.preheader7 ]
  %10 = add i32 %8, %9
  %11 = add i32 %8, 1
  %.not = icmp sgt i32 %11, %0
  br i1 %.not, label %._crit_edge, label %.lr.ph, !llvm.loop !4

._crit_edge:                                      ; preds = %.lr.ph, %middle.block, %1
  %.lcssa = phi i32 [ 0, %1 ], [ %7, %middle.block ], [ %10, %.lr.ph ]
  ret i32 %.lcssa
}

; Function Attrs: nofree norecurse nosync nounwind memory(none)
define signext i32 @_mlir_ciface_execute(i32 %0) local_unnamed_addr #0 {
  %.not1.i = icmp slt i32 %0, 1
  br i1 %.not1.i, label %execute.exit, label %.lr.ph.i.preheader

.lr.ph.i.preheader:                               ; preds = %1
  %2 = add nsw i32 %0, -2147483647
  %or.cond = icmp ult i32 %2, -2147483627
  br i1 %or.cond, label %.lr.ph.i.preheader3, label %vector.ph

vector.ph:                                        ; preds = %.lr.ph.i.preheader
  %n.vec = and i32 %0, 2147483640
  %3 = or disjoint i32 %n.vec, 1
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i32 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %vec.ind = phi <4 x i32> [ <i32 1, i32 2, i32 3, i32 4>, %vector.ph ], [ %vec.ind.next, %vector.body ]
  %vec.phi = phi <4 x i32> [ zeroinitializer, %vector.ph ], [ %4, %vector.body ]
  %vec.phi1 = phi <4 x i32> [ zeroinitializer, %vector.ph ], [ %5, %vector.body ]
  %step.add = add <4 x i32> %vec.ind, splat (i32 4)
  %4 = add <4 x i32> %vec.phi, %vec.ind
  %5 = add <4 x i32> %vec.phi1, %step.add
  %index.next = add nuw i32 %index, 8
  %vec.ind.next = add <4 x i32> %vec.ind, splat (i32 8)
  %6 = icmp eq i32 %index.next, %n.vec
  br i1 %6, label %middle.block, label %vector.body, !llvm.loop !5

middle.block:                                     ; preds = %vector.body
  %bin.rdx = add <4 x i32> %5, %4
  %7 = tail call i32 @llvm.vector.reduce.add.v4i32(<4 x i32> %bin.rdx)
  %cmp.n = icmp eq i32 %0, %n.vec
  br i1 %cmp.n, label %execute.exit, label %.lr.ph.i.preheader3

.lr.ph.i.preheader3:                              ; preds = %.lr.ph.i.preheader, %middle.block
  %.ph = phi i32 [ 1, %.lr.ph.i.preheader ], [ %3, %middle.block ]
  %.ph4 = phi i32 [ 0, %.lr.ph.i.preheader ], [ %7, %middle.block ]
  br label %.lr.ph.i

.lr.ph.i:                                         ; preds = %.lr.ph.i.preheader3, %.lr.ph.i
  %8 = phi i32 [ %11, %.lr.ph.i ], [ %.ph, %.lr.ph.i.preheader3 ]
  %9 = phi i32 [ %10, %.lr.ph.i ], [ %.ph4, %.lr.ph.i.preheader3 ]
  %10 = add i32 %9, %8
  %11 = add i32 %8, 1
  %.not.i = icmp sgt i32 %11, %0
  br i1 %.not.i, label %execute.exit, label %.lr.ph.i, !llvm.loop !6

execute.exit:                                     ; preds = %.lr.ph.i, %middle.block, %1
  %.lcssa.i = phi i32 [ 0, %1 ], [ %7, %middle.block ], [ %10, %.lr.ph.i ]
  ret i32 %.lcssa.i
}

; Function Attrs: nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: none)
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #1 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %.not1.i = icmp slt i32 %3, 1
  br i1 %.not1.i, label %execute.exit, label %.lr.ph.i.preheader

.lr.ph.i.preheader:                               ; preds = %1
  %4 = add nsw i32 %3, -2147483647
  %or.cond = icmp ult i32 %4, -2147483627
  br i1 %or.cond, label %.lr.ph.i.preheader3, label %vector.ph

vector.ph:                                        ; preds = %.lr.ph.i.preheader
  %n.vec = and i32 %3, 2147483640
  %5 = or disjoint i32 %n.vec, 1
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i32 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %vec.ind = phi <4 x i32> [ <i32 1, i32 2, i32 3, i32 4>, %vector.ph ], [ %vec.ind.next, %vector.body ]
  %vec.phi = phi <4 x i32> [ zeroinitializer, %vector.ph ], [ %6, %vector.body ]
  %vec.phi1 = phi <4 x i32> [ zeroinitializer, %vector.ph ], [ %7, %vector.body ]
  %step.add = add <4 x i32> %vec.ind, splat (i32 4)
  %6 = add <4 x i32> %vec.phi, %vec.ind
  %7 = add <4 x i32> %vec.phi1, %step.add
  %index.next = add nuw i32 %index, 8
  %vec.ind.next = add <4 x i32> %vec.ind, splat (i32 8)
  %8 = icmp eq i32 %index.next, %n.vec
  br i1 %8, label %middle.block, label %vector.body, !llvm.loop !7

middle.block:                                     ; preds = %vector.body
  %bin.rdx = add <4 x i32> %7, %6
  %9 = tail call i32 @llvm.vector.reduce.add.v4i32(<4 x i32> %bin.rdx)
  %cmp.n = icmp eq i32 %3, %n.vec
  br i1 %cmp.n, label %execute.exit, label %.lr.ph.i.preheader3

.lr.ph.i.preheader3:                              ; preds = %.lr.ph.i.preheader, %middle.block
  %.ph = phi i32 [ 1, %.lr.ph.i.preheader ], [ %5, %middle.block ]
  %.ph4 = phi i32 [ 0, %.lr.ph.i.preheader ], [ %9, %middle.block ]
  br label %.lr.ph.i

.lr.ph.i:                                         ; preds = %.lr.ph.i.preheader3, %.lr.ph.i
  %10 = phi i32 [ %13, %.lr.ph.i ], [ %.ph, %.lr.ph.i.preheader3 ]
  %11 = phi i32 [ %12, %.lr.ph.i ], [ %.ph4, %.lr.ph.i.preheader3 ]
  %12 = add i32 %11, %10
  %13 = add i32 %10, 1
  %.not.i = icmp sgt i32 %13, %3
  br i1 %.not.i, label %execute.exit, label %.lr.ph.i, !llvm.loop !8

execute.exit:                                     ; preds = %.lr.ph.i, %middle.block, %1
  %.lcssa.i = phi i32 [ 0, %1 ], [ %9, %middle.block ], [ %12, %.lr.ph.i ]
  %14 = getelementptr i8, ptr %0, i64 8
  %15 = load ptr, ptr %14, align 8
  store i32 %.lcssa.i, ptr %15, align 4
  ret void
}

; Function Attrs: nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: none)
define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #1 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %.not1.i.i = icmp slt i32 %3, 1
  br i1 %.not1.i.i, label %_mlir_ciface_execute.exit, label %.lr.ph.i.i.preheader

.lr.ph.i.i.preheader:                             ; preds = %1
  %4 = add nsw i32 %3, -2147483647
  %or.cond = icmp ult i32 %4, -2147483627
  br i1 %or.cond, label %.lr.ph.i.i.preheader3, label %vector.ph

vector.ph:                                        ; preds = %.lr.ph.i.i.preheader
  %n.vec = and i32 %3, 2147483640
  %5 = or disjoint i32 %n.vec, 1
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i32 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %vec.ind = phi <4 x i32> [ <i32 1, i32 2, i32 3, i32 4>, %vector.ph ], [ %vec.ind.next, %vector.body ]
  %vec.phi = phi <4 x i32> [ zeroinitializer, %vector.ph ], [ %6, %vector.body ]
  %vec.phi1 = phi <4 x i32> [ zeroinitializer, %vector.ph ], [ %7, %vector.body ]
  %step.add = add <4 x i32> %vec.ind, splat (i32 4)
  %6 = add <4 x i32> %vec.phi, %vec.ind
  %7 = add <4 x i32> %vec.phi1, %step.add
  %index.next = add nuw i32 %index, 8
  %vec.ind.next = add <4 x i32> %vec.ind, splat (i32 8)
  %8 = icmp eq i32 %index.next, %n.vec
  br i1 %8, label %middle.block, label %vector.body, !llvm.loop !9

middle.block:                                     ; preds = %vector.body
  %bin.rdx = add <4 x i32> %7, %6
  %9 = tail call i32 @llvm.vector.reduce.add.v4i32(<4 x i32> %bin.rdx)
  %cmp.n = icmp eq i32 %3, %n.vec
  br i1 %cmp.n, label %_mlir_ciface_execute.exit, label %.lr.ph.i.i.preheader3

.lr.ph.i.i.preheader3:                            ; preds = %.lr.ph.i.i.preheader, %middle.block
  %.ph = phi i32 [ 1, %.lr.ph.i.i.preheader ], [ %5, %middle.block ]
  %.ph4 = phi i32 [ 0, %.lr.ph.i.i.preheader ], [ %9, %middle.block ]
  br label %.lr.ph.i.i

.lr.ph.i.i:                                       ; preds = %.lr.ph.i.i.preheader3, %.lr.ph.i.i
  %10 = phi i32 [ %13, %.lr.ph.i.i ], [ %.ph, %.lr.ph.i.i.preheader3 ]
  %11 = phi i32 [ %12, %.lr.ph.i.i ], [ %.ph4, %.lr.ph.i.i.preheader3 ]
  %12 = add i32 %11, %10
  %13 = add i32 %10, 1
  %.not.i.i = icmp sgt i32 %13, %3
  br i1 %.not.i.i, label %_mlir_ciface_execute.exit, label %.lr.ph.i.i, !llvm.loop !10

_mlir_ciface_execute.exit:                        ; preds = %.lr.ph.i.i, %middle.block, %1
  %.lcssa.i.i = phi i32 [ 0, %1 ], [ %9, %middle.block ], [ %12, %.lr.ph.i.i ]
  %14 = getelementptr i8, ptr %0, i64 8
  %15 = load ptr, ptr %14, align 8
  store i32 %.lcssa.i.i, ptr %15, align 4
  ret void
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i32 @llvm.vector.reduce.add.v4i32(<4 x i32>) #2

attributes #0 = { nofree norecurse nosync nounwind memory(none) }
attributes #1 = { nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: none) }
attributes #2 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }

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
