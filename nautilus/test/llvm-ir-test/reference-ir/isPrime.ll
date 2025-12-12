; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nofree norecurse nosync nounwind memory(none)
define noundef i1 @execute(i32 %0) local_unnamed_addr #0 {
  %2 = icmp slt i32 %0, 2
  br i1 %2, label %.loopexit, label %.preheader

.preheader:                                       ; preds = %1
  %.not2 = icmp samesign ult i32 %0, 4
  br i1 %.not2, label %.loopexit, label %.lr.ph

.loopexit:                                        ; preds = %4, %.lr.ph, %.preheader, %1
  %3 = phi i1 [ false, %1 ], [ true, %.preheader ], [ %.not5.not, %.lr.ph ], [ %.not5.not, %4 ]
  ret i1 %3

4:                                                ; preds = %.lr.ph
  %5 = add i32 %7, 1
  %6 = mul i32 %5, %5
  %.not = icmp sgt i32 %6, %0
  br i1 %.not, label %.loopexit, label %.lr.ph

.lr.ph:                                           ; preds = %.preheader, %4
  %7 = phi i32 [ %5, %4 ], [ 2, %.preheader ]
  %8 = srem i32 %0, %7
  %.not5.not = icmp ne i32 %8, 0
  br i1 %.not5.not, label %4, label %.loopexit
}

; Function Attrs: nofree norecurse nosync nounwind memory(none)
define noundef i1 @_mlir_ciface_execute(i32 %0) local_unnamed_addr #1 {
  %2 = icmp slt i32 %0, 2
  br i1 %2, label %execute.exit, label %.preheader.i

.preheader.i:                                     ; preds = %1
  %.not2.i = icmp samesign ult i32 %0, 4
  br i1 %.not2.i, label %execute.exit, label %.lr.ph.i

3:                                                ; preds = %.lr.ph.i
  %4 = add i32 %6, 1
  %5 = mul i32 %4, %4
  %.not.i = icmp sgt i32 %5, %0
  br i1 %.not.i, label %execute.exit, label %.lr.ph.i

.lr.ph.i:                                         ; preds = %.preheader.i, %3
  %6 = phi i32 [ %4, %3 ], [ 2, %.preheader.i ]
  %7 = srem i32 %0, %6
  %.not5.i.not.not = icmp ne i32 %7, 0
  br i1 %.not5.i.not.not, label %3, label %execute.exit

execute.exit:                                     ; preds = %3, %.lr.ph.i, %1, %.preheader.i
  %8 = phi i1 [ false, %1 ], [ true, %.preheader.i ], [ %.not5.i.not.not, %.lr.ph.i ], [ %.not5.i.not.not, %3 ]
  ret i1 %8
}

; Function Attrs: nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: none)
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = icmp slt i32 %3, 2
  br i1 %4, label %execute.exit, label %.preheader.i

.preheader.i:                                     ; preds = %1
  %.not2.i = icmp samesign ult i32 %3, 4
  br i1 %.not2.i, label %execute.exit, label %.lr.ph.i

5:                                                ; preds = %.lr.ph.i
  %6 = add i32 %8, 1
  %7 = mul i32 %6, %6
  %.not.i = icmp sgt i32 %7, %3
  br i1 %.not.i, label %execute.exit, label %.lr.ph.i

.lr.ph.i:                                         ; preds = %.preheader.i, %5
  %8 = phi i32 [ %6, %5 ], [ 2, %.preheader.i ]
  %9 = srem i32 %3, %8
  %.not5.i.not.not = icmp ne i32 %9, 0
  br i1 %.not5.i.not.not, label %5, label %execute.exit

execute.exit:                                     ; preds = %5, %.lr.ph.i, %1, %.preheader.i
  %10 = phi i1 [ false, %1 ], [ true, %.preheader.i ], [ %.not5.i.not.not, %.lr.ph.i ], [ %.not5.i.not.not, %5 ]
  %11 = getelementptr i8, ptr %0, i64 8
  %12 = load ptr, ptr %11, align 8
  store i1 %10, ptr %12, align 1
  ret void
}

; Function Attrs: nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: none)
define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = icmp slt i32 %3, 2
  br i1 %4, label %_mlir_ciface_execute.exit, label %.preheader.i.i

.preheader.i.i:                                   ; preds = %1
  %.not2.i.i = icmp samesign ult i32 %3, 4
  br i1 %.not2.i.i, label %_mlir_ciface_execute.exit, label %.lr.ph.i.i

5:                                                ; preds = %.lr.ph.i.i
  %6 = add i32 %8, 1
  %7 = mul i32 %6, %6
  %.not.i.i = icmp sgt i32 %7, %3
  br i1 %.not.i.i, label %_mlir_ciface_execute.exit, label %.lr.ph.i.i

.lr.ph.i.i:                                       ; preds = %.preheader.i.i, %5
  %8 = phi i32 [ %6, %5 ], [ 2, %.preheader.i.i ]
  %9 = srem i32 %3, %8
  %.not5.i.not.i.not.not = icmp ne i32 %9, 0
  br i1 %.not5.i.not.i.not.not, label %5, label %_mlir_ciface_execute.exit

_mlir_ciface_execute.exit:                        ; preds = %5, %.lr.ph.i.i, %1, %.preheader.i.i
  %10 = phi i1 [ false, %1 ], [ true, %.preheader.i.i ], [ %.not5.i.not.i.not.not, %.lr.ph.i.i ], [ %.not5.i.not.i.not.not, %5 ]
  %11 = getelementptr i8, ptr %0, i64 8
  %12 = load ptr, ptr %11, align 8
  store i1 %10, ptr %12, align 1
  ret void
}

attributes #0 = { nofree norecurse nosync nounwind memory(none) }
attributes #1 = { nofree norecurse nosync nounwind memory(none) }
attributes #2 = { nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
