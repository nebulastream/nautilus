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
  br i1 %3, label %switch.lookup, label %.lr.ph.preheader34

.lr.ph.preheader34:                               ; preds = %.lr.ph.preheader
  %4 = add nsw i32 %0, -6
  %min.iters.check = icmp ult i32 %4, 8
  br i1 %min.iters.check, label %.lr.ph.preheader37, label %vector.ph

vector.ph:                                        ; preds = %.lr.ph.preheader34
  %n.vec = and i32 %4, -8
  %5 = or disjoint i32 %n.vec, 6
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i32 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %reduced.phi = phi <4 x i32> [ <i32 180, i32 0, i32 0, i32 0>, %vector.ph ], [ %bin.rdx, %vector.body ]
  %bin.rdx = add <4 x i32> %reduced.phi, splat (i32 66)
  %index.next = add nuw i32 %index, 8
  %6 = icmp eq i32 %index.next, %n.vec
  br i1 %6, label %middle.block, label %vector.body, !llvm.loop !1

middle.block:                                     ; preds = %vector.body
  %7 = tail call i32 @llvm.vector.reduce.add.v4i32(<4 x i32> %bin.rdx)
  %cmp.n = icmp eq i32 %4, %n.vec
  br i1 %cmp.n, label %._crit_edge, label %.lr.ph.preheader37

.lr.ph.preheader37:                               ; preds = %.lr.ph.preheader34, %middle.block
  %.ph = phi i32 [ 6, %.lr.ph.preheader34 ], [ %5, %middle.block ]
  %.ph38 = phi i32 [ 180, %.lr.ph.preheader34 ], [ %7, %middle.block ]
  br label %.lr.ph

.lr.ph:                                           ; preds = %.lr.ph.preheader37, %.lr.ph
  %8 = phi i32 [ %11, %.lr.ph ], [ %.ph, %.lr.ph.preheader37 ]
  %9 = phi i32 [ %10, %.lr.ph ], [ %.ph38, %.lr.ph.preheader37 ]
  %10 = add i32 %9, 33
  %11 = add nuw nsw i32 %8, 1
  %exitcond.not = icmp eq i32 %11, %0
  br i1 %exitcond.not, label %._crit_edge, label %.lr.ph, !llvm.loop !5

switch.lookup:                                    ; preds = %.lr.ph.preheader
  %12 = mul nuw nsw i32 %0, 30
  br label %._crit_edge

._crit_edge:                                      ; preds = %.lr.ph, %middle.block, %switch.lookup, %1
  %.lcssa = phi i32 [ 0, %1 ], [ %12, %switch.lookup ], [ %7, %middle.block ], [ %10, %.lr.ph ]
  ret i32 %.lcssa
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i32 @_mlir_ciface_execute(i32 %0) local_unnamed_addr #1 {
  %2 = icmp sgt i32 %0, 0
  br i1 %2, label %.lr.ph.preheader.i, label %execute.exit

.lr.ph.preheader.i:                               ; preds = %1
  %3 = icmp samesign ult i32 %0, 7
  br i1 %3, label %switch.lookup.i, label %.lr.ph.i.preheader

.lr.ph.i.preheader:                               ; preds = %.lr.ph.preheader.i
  %4 = mul i32 %0, 33
  %5 = add i32 %4, -18
  br label %execute.exit

switch.lookup.i:                                  ; preds = %.lr.ph.preheader.i
  %6 = mul nuw nsw i32 %0, 30
  br label %execute.exit

execute.exit:                                     ; preds = %.lr.ph.i.preheader, %1, %switch.lookup.i
  %.lcssa.i = phi i32 [ 0, %1 ], [ %6, %switch.lookup.i ], [ %5, %.lr.ph.i.preheader ]
  ret i32 %.lcssa.i
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = icmp sgt i32 %3, 0
  br i1 %4, label %.lr.ph.preheader.i, label %execute.exit

.lr.ph.preheader.i:                               ; preds = %1
  %5 = icmp samesign ult i32 %3, 7
  br i1 %5, label %switch.lookup.i, label %.lr.ph.i.preheader

.lr.ph.i.preheader:                               ; preds = %.lr.ph.preheader.i
  %6 = mul i32 %3, 33
  %7 = add i32 %6, -18
  br label %execute.exit

switch.lookup.i:                                  ; preds = %.lr.ph.preheader.i
  %8 = mul nuw nsw i32 %3, 30
  br label %execute.exit

execute.exit:                                     ; preds = %.lr.ph.i.preheader, %1, %switch.lookup.i
  %.lcssa.i = phi i32 [ 0, %1 ], [ %8, %switch.lookup.i ], [ %7, %.lr.ph.i.preheader ]
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
  %5 = icmp samesign ult i32 %3, 7
  br i1 %5, label %switch.lookup.i.i, label %.lr.ph.i.preheader.i

.lr.ph.i.preheader.i:                             ; preds = %.lr.ph.preheader.i.i
  %6 = mul i32 %3, 33
  %7 = add i32 %6, -18
  br label %_mlir_ciface_execute.exit

switch.lookup.i.i:                                ; preds = %.lr.ph.preheader.i.i
  %8 = mul nuw nsw i32 %3, 30
  br label %_mlir_ciface_execute.exit

_mlir_ciface_execute.exit:                        ; preds = %1, %.lr.ph.i.preheader.i, %switch.lookup.i.i
  %.lcssa.i.i = phi i32 [ 0, %1 ], [ %8, %switch.lookup.i.i ], [ %7, %.lr.ph.i.preheader.i ]
  %9 = getelementptr i8, ptr %0, i64 8
  %10 = load ptr, ptr %9, align 8
  store i32 %.lcssa.i.i, ptr %10, align 4
  ret void
}

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
!5 = distinct !{!5, !2, !4, !3}
