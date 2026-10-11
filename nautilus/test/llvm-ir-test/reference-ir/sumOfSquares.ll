; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i32 @execute(i32 %0) local_unnamed_addr #0 {
  %.not1 = icmp slt i32 %0, 1
  br i1 %.not1, label %._crit_edge, label %.lr.ph.preheader

.lr.ph.preheader:                                 ; preds = %1
  %2 = add nsw i32 %0, -1
  %3 = zext nneg i32 %2 to i33
  %4 = add nsw i32 %0, -2
  %5 = zext i32 %4 to i33
  %6 = mul i33 %3, %5
  %7 = add nsw i32 %0, -3
  %8 = zext i32 %7 to i33
  %9 = mul i33 %6, %8
  %10 = lshr i33 %9, 1
  %11 = trunc nuw i33 %10 to i32
  %12 = mul i32 %11, 1431655766
  %13 = lshr i33 %6, 1
  %14 = trunc nuw i33 %13 to i32
  %15 = mul i32 %14, 5
  %16 = add i32 %12, %15
  %17 = shl i32 %0, 2
  %18 = add i32 %16, %17
  %19 = add i32 %18, -3
  br label %._crit_edge

._crit_edge:                                      ; preds = %.lr.ph.preheader, %1
  %.lcssa = phi i32 [ 0, %1 ], [ %19, %.lr.ph.preheader ]
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
!4 = distinct !{!4, !2}
