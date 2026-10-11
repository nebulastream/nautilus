; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nofree norecurse nosync nounwind memory(none)
define noundef zeroext i1 @execute(i32 %0) local_unnamed_addr #0 {
  %2 = icmp slt i32 %0, 2
  br i1 %2, label %.loopexit, label %.preheader

.preheader:                                       ; preds = %1
  %.not1 = icmp samesign ult i32 %0, 4
  br i1 %.not1, label %.loopexit, label %.lr.ph

.loopexit:                                        ; preds = %4, %.lr.ph, %.preheader, %1
  %3 = phi i1 [ false, %1 ], [ true, %.preheader ], [ %.not4.not, %.lr.ph ], [ %.not4.not, %4 ]
  ret i1 %3

4:                                                ; preds = %.lr.ph
  %5 = add i32 %7, 1
  %6 = mul i32 %5, %5
  %.not = icmp sgt i32 %6, %0
  br i1 %.not, label %.loopexit, label %.lr.ph

.lr.ph:                                           ; preds = %.preheader, %4
  %7 = phi i32 [ %5, %4 ], [ 2, %.preheader ]
  %8 = srem i32 %0, %7
  %.not4.not = icmp ne i32 %8, 0
  br i1 %.not4.not, label %4, label %.loopexit
}

attributes #0 = { nofree norecurse nosync nounwind memory(none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
