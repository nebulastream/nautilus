; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nofree norecurse nosync nounwind memory(none)
define signext i32 @execute(i32 %0) local_unnamed_addr #0 {
  %2 = icmp sgt i32 %0, 1
  br i1 %2, label %.lr.ph.preheader, label %._crit_edge

.lr.ph.preheader:                                 ; preds = %1
  %3 = add nsw i32 %0, -1
  %min.iters.check = icmp ult i32 %0, 9
  br i1 %min.iters.check, label %.lr.ph.preheader5, label %vector.ph

vector.ph:                                        ; preds = %.lr.ph.preheader
  %n.vec = and i32 %3, -8
  %4 = sub i32 %0, %n.vec
  %broadcast.splatinsert = insertelement <4 x i32> poison, i32 %0, i64 0
  %broadcast.splat = shufflevector <4 x i32> %broadcast.splatinsert, <4 x i32> poison, <4 x i32> zeroinitializer
  %induction = add nsw <4 x i32> %broadcast.splat, <i32 0, i32 -1, i32 -2, i32 -3>
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i32 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %vec.ind = phi <4 x i32> [ %induction, %vector.ph ], [ %vec.ind.next, %vector.body ]
  %vec.phi = phi <4 x i32> [ splat (i32 1), %vector.ph ], [ %5, %vector.body ]
  %vec.phi4 = phi <4 x i32> [ splat (i32 1), %vector.ph ], [ %6, %vector.body ]
  %step.add = add <4 x i32> %vec.ind, splat (i32 -4)
  %5 = mul <4 x i32> %vec.ind, %vec.phi
  %6 = mul <4 x i32> %step.add, %vec.phi4
  %index.next = add nuw i32 %index, 8
  %vec.ind.next = add <4 x i32> %vec.ind, splat (i32 -8)
  %7 = icmp eq i32 %index.next, %n.vec
  br i1 %7, label %middle.block, label %vector.body, !llvm.loop !1

middle.block:                                     ; preds = %vector.body
  %bin.rdx = mul <4 x i32> %6, %5
  %8 = tail call i32 @llvm.vector.reduce.mul.v4i32(<4 x i32> %bin.rdx)
  %cmp.n = icmp eq i32 %3, %n.vec
  br i1 %cmp.n, label %._crit_edge, label %.lr.ph.preheader5

.lr.ph.preheader5:                                ; preds = %.lr.ph.preheader, %middle.block
  %.ph = phi i32 [ %0, %.lr.ph.preheader ], [ %4, %middle.block ]
  %.ph6 = phi i32 [ 1, %.lr.ph.preheader ], [ %8, %middle.block ]
  br label %.lr.ph

.lr.ph:                                           ; preds = %.lr.ph.preheader5, %.lr.ph
  %9 = phi i32 [ %12, %.lr.ph ], [ %.ph, %.lr.ph.preheader5 ]
  %10 = phi i32 [ %11, %.lr.ph ], [ %.ph6, %.lr.ph.preheader5 ]
  %11 = mul i32 %9, %10
  %12 = add nsw i32 %9, -1
  %13 = icmp samesign ugt i32 %9, 2
  br i1 %13, label %.lr.ph, label %._crit_edge, !llvm.loop !4

._crit_edge:                                      ; preds = %.lr.ph, %middle.block, %1
  %.lcssa = phi i32 [ 1, %1 ], [ %8, %middle.block ], [ %11, %.lr.ph ]
  ret i32 %.lcssa
}

; Function Attrs: nofree norecurse nosync nounwind memory(none)
define signext i32 @_mlir_ciface_execute(i32 %0) local_unnamed_addr #0 {
  %2 = icmp sgt i32 %0, 1
  br i1 %2, label %.lr.ph.i.preheader, label %execute.exit

.lr.ph.i.preheader:                               ; preds = %1
  %3 = add nsw i32 %0, -1
  %min.iters.check = icmp ult i32 %0, 9
  br i1 %min.iters.check, label %.lr.ph.i.preheader2, label %vector.ph

vector.ph:                                        ; preds = %.lr.ph.i.preheader
  %n.vec = and i32 %3, -8
  %4 = sub i32 %0, %n.vec
  %broadcast.splatinsert = insertelement <4 x i32> poison, i32 %0, i64 0
  %broadcast.splat = shufflevector <4 x i32> %broadcast.splatinsert, <4 x i32> poison, <4 x i32> zeroinitializer
  %induction = add nsw <4 x i32> %broadcast.splat, <i32 0, i32 -1, i32 -2, i32 -3>
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i32 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %vec.ind = phi <4 x i32> [ %induction, %vector.ph ], [ %vec.ind.next, %vector.body ]
  %vec.phi = phi <4 x i32> [ splat (i32 1), %vector.ph ], [ %5, %vector.body ]
  %vec.phi1 = phi <4 x i32> [ splat (i32 1), %vector.ph ], [ %6, %vector.body ]
  %step.add = add <4 x i32> %vec.ind, splat (i32 -4)
  %5 = mul <4 x i32> %vec.phi, %vec.ind
  %6 = mul <4 x i32> %vec.phi1, %step.add
  %index.next = add nuw i32 %index, 8
  %vec.ind.next = add <4 x i32> %vec.ind, splat (i32 -8)
  %7 = icmp eq i32 %index.next, %n.vec
  br i1 %7, label %middle.block, label %vector.body, !llvm.loop !5

middle.block:                                     ; preds = %vector.body
  %bin.rdx = mul <4 x i32> %6, %5
  %8 = tail call i32 @llvm.vector.reduce.mul.v4i32(<4 x i32> %bin.rdx)
  %cmp.n = icmp eq i32 %3, %n.vec
  br i1 %cmp.n, label %execute.exit, label %.lr.ph.i.preheader2

.lr.ph.i.preheader2:                              ; preds = %.lr.ph.i.preheader, %middle.block
  %.ph = phi i32 [ %0, %.lr.ph.i.preheader ], [ %4, %middle.block ]
  %.ph3 = phi i32 [ 1, %.lr.ph.i.preheader ], [ %8, %middle.block ]
  br label %.lr.ph.i

.lr.ph.i:                                         ; preds = %.lr.ph.i.preheader2, %.lr.ph.i
  %9 = phi i32 [ %12, %.lr.ph.i ], [ %.ph, %.lr.ph.i.preheader2 ]
  %10 = phi i32 [ %11, %.lr.ph.i ], [ %.ph3, %.lr.ph.i.preheader2 ]
  %11 = mul i32 %10, %9
  %12 = add nsw i32 %9, -1
  %13 = icmp samesign ugt i32 %9, 2
  br i1 %13, label %.lr.ph.i, label %execute.exit, !llvm.loop !6

execute.exit:                                     ; preds = %.lr.ph.i, %middle.block, %1
  %.lcssa.i = phi i32 [ 1, %1 ], [ %8, %middle.block ], [ %11, %.lr.ph.i ]
  ret i32 %.lcssa.i
}

; Function Attrs: nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: none)
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #1 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = icmp sgt i32 %3, 1
  br i1 %4, label %.lr.ph.i.preheader, label %execute.exit

.lr.ph.i.preheader:                               ; preds = %1
  %5 = add nsw i32 %3, -1
  %min.iters.check = icmp ult i32 %3, 9
  br i1 %min.iters.check, label %.lr.ph.i.preheader2, label %vector.ph

vector.ph:                                        ; preds = %.lr.ph.i.preheader
  %n.vec = and i32 %5, -8
  %6 = sub i32 %3, %n.vec
  %broadcast.splatinsert = insertelement <4 x i32> poison, i32 %3, i64 0
  %broadcast.splat = shufflevector <4 x i32> %broadcast.splatinsert, <4 x i32> poison, <4 x i32> zeroinitializer
  %induction = add nsw <4 x i32> %broadcast.splat, <i32 0, i32 -1, i32 -2, i32 -3>
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i32 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %vec.ind = phi <4 x i32> [ %induction, %vector.ph ], [ %vec.ind.next, %vector.body ]
  %vec.phi = phi <4 x i32> [ splat (i32 1), %vector.ph ], [ %7, %vector.body ]
  %vec.phi1 = phi <4 x i32> [ splat (i32 1), %vector.ph ], [ %8, %vector.body ]
  %step.add = add <4 x i32> %vec.ind, splat (i32 -4)
  %7 = mul <4 x i32> %vec.phi, %vec.ind
  %8 = mul <4 x i32> %vec.phi1, %step.add
  %index.next = add nuw i32 %index, 8
  %vec.ind.next = add <4 x i32> %vec.ind, splat (i32 -8)
  %9 = icmp eq i32 %index.next, %n.vec
  br i1 %9, label %middle.block, label %vector.body, !llvm.loop !7

middle.block:                                     ; preds = %vector.body
  %bin.rdx = mul <4 x i32> %8, %7
  %10 = tail call i32 @llvm.vector.reduce.mul.v4i32(<4 x i32> %bin.rdx)
  %cmp.n = icmp eq i32 %5, %n.vec
  br i1 %cmp.n, label %execute.exit, label %.lr.ph.i.preheader2

.lr.ph.i.preheader2:                              ; preds = %.lr.ph.i.preheader, %middle.block
  %.ph = phi i32 [ %3, %.lr.ph.i.preheader ], [ %6, %middle.block ]
  %.ph3 = phi i32 [ 1, %.lr.ph.i.preheader ], [ %10, %middle.block ]
  br label %.lr.ph.i

.lr.ph.i:                                         ; preds = %.lr.ph.i.preheader2, %.lr.ph.i
  %11 = phi i32 [ %14, %.lr.ph.i ], [ %.ph, %.lr.ph.i.preheader2 ]
  %12 = phi i32 [ %13, %.lr.ph.i ], [ %.ph3, %.lr.ph.i.preheader2 ]
  %13 = mul i32 %12, %11
  %14 = add nsw i32 %11, -1
  %15 = icmp samesign ugt i32 %11, 2
  br i1 %15, label %.lr.ph.i, label %execute.exit, !llvm.loop !8

execute.exit:                                     ; preds = %.lr.ph.i, %middle.block, %1
  %.lcssa.i = phi i32 [ 1, %1 ], [ %10, %middle.block ], [ %13, %.lr.ph.i ]
  %16 = getelementptr i8, ptr %0, i64 8
  %17 = load ptr, ptr %16, align 8
  store i32 %.lcssa.i, ptr %17, align 4
  ret void
}

; Function Attrs: nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: none)
define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #1 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = icmp sgt i32 %3, 1
  br i1 %4, label %.lr.ph.i.i.preheader, label %_mlir_ciface_execute.exit

.lr.ph.i.i.preheader:                             ; preds = %1
  %5 = add nsw i32 %3, -1
  %min.iters.check = icmp ult i32 %3, 9
  br i1 %min.iters.check, label %.lr.ph.i.i.preheader2, label %vector.ph

vector.ph:                                        ; preds = %.lr.ph.i.i.preheader
  %n.vec = and i32 %5, -8
  %6 = sub i32 %3, %n.vec
  %broadcast.splatinsert = insertelement <4 x i32> poison, i32 %3, i64 0
  %broadcast.splat = shufflevector <4 x i32> %broadcast.splatinsert, <4 x i32> poison, <4 x i32> zeroinitializer
  %induction = add nsw <4 x i32> %broadcast.splat, <i32 0, i32 -1, i32 -2, i32 -3>
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i32 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %vec.ind = phi <4 x i32> [ %induction, %vector.ph ], [ %vec.ind.next, %vector.body ]
  %vec.phi = phi <4 x i32> [ splat (i32 1), %vector.ph ], [ %7, %vector.body ]
  %vec.phi1 = phi <4 x i32> [ splat (i32 1), %vector.ph ], [ %8, %vector.body ]
  %step.add = add <4 x i32> %vec.ind, splat (i32 -4)
  %7 = mul <4 x i32> %vec.phi, %vec.ind
  %8 = mul <4 x i32> %vec.phi1, %step.add
  %index.next = add nuw i32 %index, 8
  %vec.ind.next = add <4 x i32> %vec.ind, splat (i32 -8)
  %9 = icmp eq i32 %index.next, %n.vec
  br i1 %9, label %middle.block, label %vector.body, !llvm.loop !9

middle.block:                                     ; preds = %vector.body
  %bin.rdx = mul <4 x i32> %8, %7
  %10 = tail call i32 @llvm.vector.reduce.mul.v4i32(<4 x i32> %bin.rdx)
  %cmp.n = icmp eq i32 %5, %n.vec
  br i1 %cmp.n, label %_mlir_ciface_execute.exit, label %.lr.ph.i.i.preheader2

.lr.ph.i.i.preheader2:                            ; preds = %.lr.ph.i.i.preheader, %middle.block
  %.ph = phi i32 [ %3, %.lr.ph.i.i.preheader ], [ %6, %middle.block ]
  %.ph3 = phi i32 [ 1, %.lr.ph.i.i.preheader ], [ %10, %middle.block ]
  br label %.lr.ph.i.i

.lr.ph.i.i:                                       ; preds = %.lr.ph.i.i.preheader2, %.lr.ph.i.i
  %11 = phi i32 [ %14, %.lr.ph.i.i ], [ %.ph, %.lr.ph.i.i.preheader2 ]
  %12 = phi i32 [ %13, %.lr.ph.i.i ], [ %.ph3, %.lr.ph.i.i.preheader2 ]
  %13 = mul i32 %12, %11
  %14 = add nsw i32 %11, -1
  %15 = icmp samesign ugt i32 %11, 2
  br i1 %15, label %.lr.ph.i.i, label %_mlir_ciface_execute.exit, !llvm.loop !10

_mlir_ciface_execute.exit:                        ; preds = %.lr.ph.i.i, %middle.block, %1
  %.lcssa.i.i = phi i32 [ 1, %1 ], [ %10, %middle.block ], [ %13, %.lr.ph.i.i ]
  %16 = getelementptr i8, ptr %0, i64 8
  %17 = load ptr, ptr %16, align 8
  store i32 %.lcssa.i.i, ptr %17, align 4
  ret void
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i32 @llvm.vector.reduce.mul.v4i32(<4 x i32>) #2

attributes #0 = { nofree norecurse nosync nounwind memory(none) }
attributes #1 = { nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: none) }
attributes #2 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
!1 = distinct !{!1, !2, !3}
!2 = !{!"llvm.loop.isvectorized", i32 1}
!3 = !{!"llvm.loop.unroll.runtime.disable"}
!4 = distinct !{!4, !3, !2}
!5 = distinct !{!5, !2, !3}
!6 = distinct !{!6, !3, !2}
!7 = distinct !{!7, !2, !3}
!8 = distinct !{!8, !3, !2}
!9 = distinct !{!9, !2, !3}
!10 = distinct !{!10, !3, !2}
