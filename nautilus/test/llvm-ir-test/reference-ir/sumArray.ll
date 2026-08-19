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
  %min.iters.check = icmp ult i32 %1, 8
  br i1 %min.iters.check, label %.lr.ph.preheader7, label %vector.ph

vector.ph:                                        ; preds = %.lr.ph.preheader
  %n.vec = and i64 %wide.trip.count, 2147483640
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i64 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %vec.phi = phi <4 x i32> [ zeroinitializer, %vector.ph ], [ %7, %vector.body ]
  %vec.phi5 = phi <4 x i32> [ zeroinitializer, %vector.ph ], [ %8, %vector.body ]
  %4 = shl nuw nsw i64 %index, 2
  %5 = getelementptr i8, ptr %0, i64 %4
  %6 = getelementptr i8, ptr %5, i64 16
  %wide.load = load <4 x i32>, ptr %5, align 4
  %wide.load6 = load <4 x i32>, ptr %6, align 4
  %7 = add <4 x i32> %wide.load, %vec.phi
  %8 = add <4 x i32> %wide.load6, %vec.phi5
  %index.next = add nuw i64 %index, 8
  %9 = icmp eq i64 %index.next, %n.vec
  br i1 %9, label %middle.block, label %vector.body, !llvm.loop !1

middle.block:                                     ; preds = %vector.body
  %bin.rdx = add <4 x i32> %8, %7
  %10 = tail call i32 @llvm.vector.reduce.add.v4i32(<4 x i32> %bin.rdx)
  %cmp.n = icmp eq i64 %n.vec, %wide.trip.count
  br i1 %cmp.n, label %._crit_edge, label %.lr.ph.preheader7

.lr.ph.preheader7:                                ; preds = %.lr.ph.preheader, %middle.block
  %indvars.iv.ph = phi i64 [ 0, %.lr.ph.preheader ], [ %n.vec, %middle.block ]
  %.ph = phi i32 [ 0, %.lr.ph.preheader ], [ %10, %middle.block ]
  br label %.lr.ph

.lr.ph:                                           ; preds = %.lr.ph.preheader7, %.lr.ph
  %indvars.iv = phi i64 [ %indvars.iv.next, %.lr.ph ], [ %indvars.iv.ph, %.lr.ph.preheader7 ]
  %11 = phi i32 [ %15, %.lr.ph ], [ %.ph, %.lr.ph.preheader7 ]
  %12 = shl nuw nsw i64 %indvars.iv, 2
  %13 = getelementptr i8, ptr %0, i64 %12
  %14 = load i32, ptr %13, align 4
  %15 = add i32 %14, %11
  %indvars.iv.next = add nuw nsw i64 %indvars.iv, 1
  %exitcond.not = icmp eq i64 %indvars.iv.next, %wide.trip.count
  br i1 %exitcond.not, label %._crit_edge, label %.lr.ph, !llvm.loop !4

._crit_edge:                                      ; preds = %.lr.ph, %middle.block, %2
  %.lcssa = phi i32 [ 0, %2 ], [ %10, %middle.block ], [ %15, %.lr.ph ]
  ret i32 %.lcssa
}

; Function Attrs: nofree norecurse nosync nounwind memory(argmem: read)
define signext i32 @_mlir_ciface_execute(ptr readonly %0, i32 %1) local_unnamed_addr #0 {
  %3 = icmp sgt i32 %1, 0
  br i1 %3, label %.lr.ph.preheader.i, label %execute.exit

.lr.ph.preheader.i:                               ; preds = %2
  %wide.trip.count.i = zext nneg i32 %1 to i64
  %min.iters.check = icmp ult i32 %1, 8
  br i1 %min.iters.check, label %.lr.ph.i.preheader, label %vector.ph

vector.ph:                                        ; preds = %.lr.ph.preheader.i
  %n.vec = and i64 %wide.trip.count.i, 2147483640
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i64 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %vec.phi = phi <4 x i32> [ zeroinitializer, %vector.ph ], [ %7, %vector.body ]
  %vec.phi1 = phi <4 x i32> [ zeroinitializer, %vector.ph ], [ %8, %vector.body ]
  %4 = shl nuw nsw i64 %index, 2
  %5 = getelementptr i8, ptr %0, i64 %4
  %6 = getelementptr i8, ptr %5, i64 16
  %wide.load = load <4 x i32>, ptr %5, align 4
  %wide.load2 = load <4 x i32>, ptr %6, align 4
  %7 = add <4 x i32> %wide.load, %vec.phi
  %8 = add <4 x i32> %wide.load2, %vec.phi1
  %index.next = add nuw i64 %index, 8
  %9 = icmp eq i64 %index.next, %n.vec
  br i1 %9, label %middle.block, label %vector.body, !llvm.loop !5

middle.block:                                     ; preds = %vector.body
  %bin.rdx = add <4 x i32> %8, %7
  %10 = tail call i32 @llvm.vector.reduce.add.v4i32(<4 x i32> %bin.rdx)
  %cmp.n = icmp eq i64 %n.vec, %wide.trip.count.i
  br i1 %cmp.n, label %execute.exit, label %.lr.ph.i.preheader

.lr.ph.i.preheader:                               ; preds = %.lr.ph.preheader.i, %middle.block
  %indvars.iv.i.ph = phi i64 [ 0, %.lr.ph.preheader.i ], [ %n.vec, %middle.block ]
  %.ph = phi i32 [ 0, %.lr.ph.preheader.i ], [ %10, %middle.block ]
  br label %.lr.ph.i

.lr.ph.i:                                         ; preds = %.lr.ph.i.preheader, %.lr.ph.i
  %indvars.iv.i = phi i64 [ %indvars.iv.next.i, %.lr.ph.i ], [ %indvars.iv.i.ph, %.lr.ph.i.preheader ]
  %11 = phi i32 [ %15, %.lr.ph.i ], [ %.ph, %.lr.ph.i.preheader ]
  %12 = shl nuw nsw i64 %indvars.iv.i, 2
  %13 = getelementptr i8, ptr %0, i64 %12
  %14 = load i32, ptr %13, align 4
  %15 = add i32 %14, %11
  %indvars.iv.next.i = add nuw nsw i64 %indvars.iv.i, 1
  %exitcond.not.i = icmp eq i64 %indvars.iv.next.i, %wide.trip.count.i
  br i1 %exitcond.not.i, label %execute.exit, label %.lr.ph.i, !llvm.loop !6

execute.exit:                                     ; preds = %.lr.ph.i, %middle.block, %2
  %.lcssa.i = phi i32 [ 0, %2 ], [ %10, %middle.block ], [ %15, %.lr.ph.i ]
  ret i32 %.lcssa.i
}

; Function Attrs: nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: none)
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #1 {
  %2 = load ptr, ptr %0, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load i32, ptr %5, align 4
  %7 = icmp sgt i32 %6, 0
  br i1 %7, label %.lr.ph.preheader.i, label %execute.exit

.lr.ph.preheader.i:                               ; preds = %1
  %wide.trip.count.i = zext nneg i32 %6 to i64
  %min.iters.check = icmp ult i32 %6, 8
  br i1 %min.iters.check, label %.lr.ph.i.preheader, label %vector.ph

vector.ph:                                        ; preds = %.lr.ph.preheader.i
  %n.vec = and i64 %wide.trip.count.i, 2147483640
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i64 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %vec.phi = phi <4 x i32> [ zeroinitializer, %vector.ph ], [ %11, %vector.body ]
  %vec.phi1 = phi <4 x i32> [ zeroinitializer, %vector.ph ], [ %12, %vector.body ]
  %8 = shl nuw nsw i64 %index, 2
  %9 = getelementptr i8, ptr %3, i64 %8
  %10 = getelementptr i8, ptr %9, i64 16
  %wide.load = load <4 x i32>, ptr %9, align 4
  %wide.load2 = load <4 x i32>, ptr %10, align 4
  %11 = add <4 x i32> %wide.load, %vec.phi
  %12 = add <4 x i32> %wide.load2, %vec.phi1
  %index.next = add nuw i64 %index, 8
  %13 = icmp eq i64 %index.next, %n.vec
  br i1 %13, label %middle.block, label %vector.body, !llvm.loop !7

middle.block:                                     ; preds = %vector.body
  %bin.rdx = add <4 x i32> %12, %11
  %14 = tail call i32 @llvm.vector.reduce.add.v4i32(<4 x i32> %bin.rdx)
  %cmp.n = icmp eq i64 %n.vec, %wide.trip.count.i
  br i1 %cmp.n, label %execute.exit, label %.lr.ph.i.preheader

.lr.ph.i.preheader:                               ; preds = %.lr.ph.preheader.i, %middle.block
  %indvars.iv.i.ph = phi i64 [ 0, %.lr.ph.preheader.i ], [ %n.vec, %middle.block ]
  %.ph = phi i32 [ 0, %.lr.ph.preheader.i ], [ %14, %middle.block ]
  br label %.lr.ph.i

.lr.ph.i:                                         ; preds = %.lr.ph.i.preheader, %.lr.ph.i
  %indvars.iv.i = phi i64 [ %indvars.iv.next.i, %.lr.ph.i ], [ %indvars.iv.i.ph, %.lr.ph.i.preheader ]
  %15 = phi i32 [ %19, %.lr.ph.i ], [ %.ph, %.lr.ph.i.preheader ]
  %16 = shl nuw nsw i64 %indvars.iv.i, 2
  %17 = getelementptr i8, ptr %3, i64 %16
  %18 = load i32, ptr %17, align 4
  %19 = add i32 %18, %15
  %indvars.iv.next.i = add nuw nsw i64 %indvars.iv.i, 1
  %exitcond.not.i = icmp eq i64 %indvars.iv.next.i, %wide.trip.count.i
  br i1 %exitcond.not.i, label %execute.exit, label %.lr.ph.i, !llvm.loop !8

execute.exit:                                     ; preds = %.lr.ph.i, %middle.block, %1
  %.lcssa.i = phi i32 [ 0, %1 ], [ %14, %middle.block ], [ %19, %.lr.ph.i ]
  %20 = getelementptr i8, ptr %0, i64 16
  %21 = load ptr, ptr %20, align 8
  store i32 %.lcssa.i, ptr %21, align 4
  ret void
}

; Function Attrs: nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: none)
define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #1 {
  %2 = load ptr, ptr %0, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load i32, ptr %5, align 4
  %7 = icmp sgt i32 %6, 0
  br i1 %7, label %.lr.ph.preheader.i.i, label %_mlir_ciface_execute.exit

.lr.ph.preheader.i.i:                             ; preds = %1
  %wide.trip.count.i.i = zext nneg i32 %6 to i64
  %min.iters.check = icmp ult i32 %6, 8
  br i1 %min.iters.check, label %.lr.ph.i.i.preheader, label %vector.ph

vector.ph:                                        ; preds = %.lr.ph.preheader.i.i
  %n.vec = and i64 %wide.trip.count.i.i, 2147483640
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i64 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %vec.phi = phi <4 x i32> [ zeroinitializer, %vector.ph ], [ %11, %vector.body ]
  %vec.phi1 = phi <4 x i32> [ zeroinitializer, %vector.ph ], [ %12, %vector.body ]
  %8 = shl nuw nsw i64 %index, 2
  %9 = getelementptr i8, ptr %3, i64 %8
  %10 = getelementptr i8, ptr %9, i64 16
  %wide.load = load <4 x i32>, ptr %9, align 4
  %wide.load2 = load <4 x i32>, ptr %10, align 4
  %11 = add <4 x i32> %wide.load, %vec.phi
  %12 = add <4 x i32> %wide.load2, %vec.phi1
  %index.next = add nuw i64 %index, 8
  %13 = icmp eq i64 %index.next, %n.vec
  br i1 %13, label %middle.block, label %vector.body, !llvm.loop !9

middle.block:                                     ; preds = %vector.body
  %bin.rdx = add <4 x i32> %12, %11
  %14 = tail call i32 @llvm.vector.reduce.add.v4i32(<4 x i32> %bin.rdx)
  %cmp.n = icmp eq i64 %n.vec, %wide.trip.count.i.i
  br i1 %cmp.n, label %_mlir_ciface_execute.exit, label %.lr.ph.i.i.preheader

.lr.ph.i.i.preheader:                             ; preds = %.lr.ph.preheader.i.i, %middle.block
  %indvars.iv.i.i.ph = phi i64 [ 0, %.lr.ph.preheader.i.i ], [ %n.vec, %middle.block ]
  %.ph = phi i32 [ 0, %.lr.ph.preheader.i.i ], [ %14, %middle.block ]
  br label %.lr.ph.i.i

.lr.ph.i.i:                                       ; preds = %.lr.ph.i.i.preheader, %.lr.ph.i.i
  %indvars.iv.i.i = phi i64 [ %indvars.iv.next.i.i, %.lr.ph.i.i ], [ %indvars.iv.i.i.ph, %.lr.ph.i.i.preheader ]
  %15 = phi i32 [ %19, %.lr.ph.i.i ], [ %.ph, %.lr.ph.i.i.preheader ]
  %16 = shl nuw nsw i64 %indvars.iv.i.i, 2
  %17 = getelementptr i8, ptr %3, i64 %16
  %18 = load i32, ptr %17, align 4
  %19 = add i32 %18, %15
  %indvars.iv.next.i.i = add nuw nsw i64 %indvars.iv.i.i, 1
  %exitcond.not.i.i = icmp eq i64 %indvars.iv.next.i.i, %wide.trip.count.i.i
  br i1 %exitcond.not.i.i, label %_mlir_ciface_execute.exit, label %.lr.ph.i.i, !llvm.loop !10

_mlir_ciface_execute.exit:                        ; preds = %.lr.ph.i.i, %middle.block, %1
  %.lcssa.i.i = phi i32 [ 0, %1 ], [ %14, %middle.block ], [ %19, %.lr.ph.i.i ]
  %20 = getelementptr i8, ptr %0, i64 16
  %21 = load ptr, ptr %20, align 8
  store i32 %.lcssa.i.i, ptr %21, align 4
  ret void
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i32 @llvm.vector.reduce.add.v4i32(<4 x i32>) #2

attributes #0 = { nofree norecurse nosync nounwind memory(argmem: read) }
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
