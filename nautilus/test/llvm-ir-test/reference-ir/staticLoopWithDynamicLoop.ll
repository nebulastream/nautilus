; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nofree norecurse nosync nounwind memory(none)
define signext i32 @execute(i32 %0) local_unnamed_addr #0 {
  %2 = icmp sgt i32 %0, 0
  br i1 %2, label %.lr.ph.preheader, label %._crit_edge

.lr.ph.preheader:                                 ; preds = %1
  %3 = icmp ult i32 %0, 7
  br i1 %3, label %switch.lookup, label %iter.check

iter.check:                                       ; preds = %.lr.ph.preheader
  %4 = add nsw i32 %0, -6
  %min.iters.check = icmp ult i32 %4, 4
  br i1 %min.iters.check, label %.lr.ph.preheader46, label %vector.main.loop.iter.check

vector.main.loop.iter.check:                      ; preds = %iter.check
  %min.iters.check32 = icmp ult i32 %4, 32
  br i1 %min.iters.check32, label %vec.epilog.ph, label %vector.ph

vector.ph:                                        ; preds = %vector.main.loop.iter.check
  %n.vec = and i32 %4, -32
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i32 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %reduced.phi = phi <8 x i32> [ <i32 180, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0>, %vector.ph ], [ %bin.rdx37, %vector.body ]
  %bin.rdx37 = add <8 x i32> %reduced.phi, splat (i32 132)
  %index.next = add nuw i32 %index, 32
  %5 = icmp eq i32 %index.next, %n.vec
  br i1 %5, label %middle.block, label %vector.body, !llvm.loop !1

middle.block:                                     ; preds = %vector.body
  %6 = tail call i32 @llvm.vector.reduce.add.v8i32(<8 x i32> %bin.rdx37)
  %cmp.n = icmp eq i32 %4, %n.vec
  br i1 %cmp.n, label %._crit_edge, label %vec.epilog.iter.check

vec.epilog.iter.check:                            ; preds = %middle.block
  %ind.end = or disjoint i32 %n.vec, 6
  %n.vec.remaining = and i32 %4, 28
  %min.epilog.iters.check = icmp eq i32 %n.vec.remaining, 0
  br i1 %min.epilog.iters.check, label %.lr.ph.preheader46, label %vec.epilog.ph

vec.epilog.ph:                                    ; preds = %vec.epilog.iter.check, %vector.main.loop.iter.check
  %vec.epilog.resume.val = phi i32 [ %n.vec, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %bc.merge.rdx = phi i32 [ %6, %vec.epilog.iter.check ], [ 180, %vector.main.loop.iter.check ]
  %n.vec39 = and i32 %4, -4
  %7 = add i32 %n.vec39, 6
  %8 = insertelement <4 x i32> <i32 poison, i32 0, i32 0, i32 0>, i32 %bc.merge.rdx, i64 0
  br label %vec.epilog.vector.body

vec.epilog.vector.body:                           ; preds = %vec.epilog.vector.body, %vec.epilog.ph
  %index40 = phi i32 [ %vec.epilog.resume.val, %vec.epilog.ph ], [ %index.next42, %vec.epilog.vector.body ]
  %vec.phi41 = phi <4 x i32> [ %8, %vec.epilog.ph ], [ %9, %vec.epilog.vector.body ]
  %9 = add <4 x i32> %vec.phi41, splat (i32 33)
  %index.next42 = add nuw i32 %index40, 4
  %10 = icmp eq i32 %index.next42, %n.vec39
  br i1 %10, label %vec.epilog.middle.block, label %vec.epilog.vector.body, !llvm.loop !5

vec.epilog.middle.block:                          ; preds = %vec.epilog.vector.body
  %11 = tail call i32 @llvm.vector.reduce.add.v4i32(<4 x i32> %9)
  %cmp.n43 = icmp eq i32 %4, %n.vec39
  br i1 %cmp.n43, label %._crit_edge, label %.lr.ph.preheader46

.lr.ph.preheader46:                               ; preds = %vec.epilog.iter.check, %vec.epilog.middle.block, %iter.check
  %.ph = phi i32 [ 6, %iter.check ], [ %ind.end, %vec.epilog.iter.check ], [ %7, %vec.epilog.middle.block ]
  %.ph47 = phi i32 [ 180, %iter.check ], [ %6, %vec.epilog.iter.check ], [ %11, %vec.epilog.middle.block ]
  br label %.lr.ph

.lr.ph:                                           ; preds = %.lr.ph.preheader46, %.lr.ph
  %12 = phi i32 [ %15, %.lr.ph ], [ %.ph, %.lr.ph.preheader46 ]
  %13 = phi i32 [ %14, %.lr.ph ], [ %.ph47, %.lr.ph.preheader46 ]
  %14 = add i32 %13, 33
  %15 = add nuw nsw i32 %12, 1
  %exitcond.not = icmp eq i32 %15, %0
  br i1 %exitcond.not, label %._crit_edge, label %.lr.ph, !llvm.loop !6

switch.lookup:                                    ; preds = %.lr.ph.preheader
  %16 = mul nuw nsw i32 %0, 30
  br label %._crit_edge

._crit_edge:                                      ; preds = %.lr.ph, %middle.block, %vec.epilog.middle.block, %switch.lookup, %1
  %.lcssa = phi i32 [ 0, %1 ], [ %16, %switch.lookup ], [ %6, %middle.block ], [ %11, %vec.epilog.middle.block ], [ %14, %.lr.ph ]
  ret i32 %.lcssa
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i32 @_mlir_ciface_execute(i32 %0) local_unnamed_addr #1 {
  %2 = icmp sgt i32 %0, 0
  br i1 %2, label %.lr.ph.preheader.i, label %execute.exit

.lr.ph.preheader.i:                               ; preds = %1
  %3 = icmp ult i32 %0, 7
  br i1 %3, label %switch.lookup, label %.lr.ph.i.preheader

.lr.ph.i.preheader:                               ; preds = %.lr.ph.preheader.i
  %4 = mul i32 %0, 33
  %5 = add i32 %4, -18
  br label %execute.exit

switch.lookup:                                    ; preds = %.lr.ph.preheader.i
  %6 = mul nuw nsw i32 %0, 30
  br label %execute.exit

execute.exit:                                     ; preds = %switch.lookup, %.lr.ph.i.preheader, %1
  %.lcssa.i = phi i32 [ 0, %1 ], [ %5, %.lr.ph.i.preheader ], [ %6, %switch.lookup ]
  ret i32 %.lcssa.i
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = icmp sgt i32 %3, 0
  br i1 %4, label %.lr.ph.preheader.i, label %execute.exit

.lr.ph.preheader.i:                               ; preds = %1
  %5 = icmp ult i32 %3, 7
  br i1 %5, label %switch.lookup, label %.lr.ph.i.preheader

.lr.ph.i.preheader:                               ; preds = %.lr.ph.preheader.i
  %6 = mul i32 %3, 33
  %7 = add i32 %6, -18
  br label %execute.exit

switch.lookup:                                    ; preds = %.lr.ph.preheader.i
  %8 = mul nuw nsw i32 %3, 30
  br label %execute.exit

execute.exit:                                     ; preds = %switch.lookup, %.lr.ph.i.preheader, %1
  %.lcssa.i = phi i32 [ 0, %1 ], [ %7, %.lr.ph.i.preheader ], [ %8, %switch.lookup ]
  %9 = getelementptr i8, ptr %0, i64 8
  %10 = load ptr, ptr %9, align 8
  store i32 %.lcssa.i, ptr %10, align 4
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = icmp sgt i32 %3, 0
  br i1 %4, label %.lr.ph.preheader.i.i, label %_mlir_ciface_execute.exit

.lr.ph.preheader.i.i:                             ; preds = %1
  %5 = icmp ult i32 %3, 7
  br i1 %5, label %switch.lookup, label %.lr.ph.i.preheader.i

.lr.ph.i.preheader.i:                             ; preds = %.lr.ph.preheader.i.i
  %6 = mul i32 %3, 33
  %7 = add i32 %6, -18
  br label %_mlir_ciface_execute.exit

switch.lookup:                                    ; preds = %.lr.ph.preheader.i.i
  %8 = mul nuw nsw i32 %3, 30
  br label %_mlir_ciface_execute.exit

_mlir_ciface_execute.exit:                        ; preds = %switch.lookup, %1, %.lr.ph.i.preheader.i
  %.lcssa.i.i = phi i32 [ 0, %1 ], [ %7, %.lr.ph.i.preheader.i ], [ %8, %switch.lookup ]
  %9 = getelementptr i8, ptr %0, i64 8
  %10 = load ptr, ptr %9, align 8
  store i32 %.lcssa.i.i, ptr %10, align 4
  ret void
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i32 @llvm.vector.reduce.add.v8i32(<8 x i32>) #3

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i32 @llvm.vector.reduce.add.v4i32(<4 x i32>) #3

attributes #0 = { nofree norecurse nosync nounwind memory(none) }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
attributes #2 = { mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none) }
attributes #3 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
!1 = distinct !{!1, !2, !3, !4}
!2 = !{!"llvm.loop.peeled.count", i32 6}
!3 = !{!"llvm.loop.isvectorized", i32 1}
!4 = !{!"llvm.loop.unroll.runtime.disable"}
!5 = distinct !{!5, !2, !3, !4}
!6 = distinct !{!6, !2, !4, !3}
