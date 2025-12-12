; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nofree norecurse nosync nounwind memory(none)
define signext i32 @execute(i32 %0) local_unnamed_addr #0 {
  %.not2 = icmp slt i32 %0, 2
  br i1 %.not2, label %._crit_edge, label %.lr.ph

.lr.ph:                                           ; preds = %1, %.lr.ph
  %2 = phi i32 [ %4, %.lr.ph ], [ 0, %1 ]
  %3 = phi i32 [ %6, %.lr.ph ], [ 2, %1 ]
  %4 = phi i32 [ %5, %.lr.ph ], [ 1, %1 ]
  %5 = add i32 %2, %4
  %6 = add i32 %3, 1
  %.not = icmp sgt i32 %6, %0
  br i1 %.not, label %._crit_edge, label %.lr.ph

._crit_edge:                                      ; preds = %.lr.ph, %1
  %.lcssa = phi i32 [ 1, %1 ], [ %5, %.lr.ph ]
  ret i32 %.lcssa
}

; Function Attrs: nofree norecurse nosync nounwind memory(none)
define signext i32 @_mlir_ciface_execute(i32 %0) local_unnamed_addr #1 {
  %.not2.i = icmp slt i32 %0, 2
  br i1 %.not2.i, label %execute.exit, label %.lr.ph.i

.lr.ph.i:                                         ; preds = %1, %.lr.ph.i
  %2 = phi i32 [ %4, %.lr.ph.i ], [ 0, %1 ]
  %3 = phi i32 [ %6, %.lr.ph.i ], [ 2, %1 ]
  %4 = phi i32 [ %5, %.lr.ph.i ], [ 1, %1 ]
  %5 = add i32 %4, %2
  %6 = add i32 %3, 1
  %.not.i = icmp sgt i32 %6, %0
  br i1 %.not.i, label %execute.exit, label %.lr.ph.i

execute.exit:                                     ; preds = %.lr.ph.i, %1
  %.lcssa.i = phi i32 [ 1, %1 ], [ %5, %.lr.ph.i ]
  ret i32 %.lcssa.i
}

; Function Attrs: nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: none)
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %.not2.i = icmp slt i32 %3, 2
  br i1 %.not2.i, label %execute.exit, label %.lr.ph.i

.lr.ph.i:                                         ; preds = %1, %.lr.ph.i
  %4 = phi i32 [ %6, %.lr.ph.i ], [ 0, %1 ]
  %5 = phi i32 [ %8, %.lr.ph.i ], [ 2, %1 ]
  %6 = phi i32 [ %7, %.lr.ph.i ], [ 1, %1 ]
  %7 = add i32 %6, %4
  %8 = add i32 %5, 1
  %.not.i = icmp sgt i32 %8, %3
  br i1 %.not.i, label %execute.exit, label %.lr.ph.i

execute.exit:                                     ; preds = %.lr.ph.i, %1
  %.lcssa.i = phi i32 [ 1, %1 ], [ %7, %.lr.ph.i ]
  %9 = getelementptr i8, ptr %0, i64 8
  %10 = load ptr, ptr %9, align 8
  store i32 %.lcssa.i, ptr %10, align 4
  ret void
}

; Function Attrs: nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: none)
define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %.not2.i.i = icmp slt i32 %3, 2
  br i1 %.not2.i.i, label %_mlir_ciface_execute.exit, label %.lr.ph.i.i

.lr.ph.i.i:                                       ; preds = %1, %.lr.ph.i.i
  %4 = phi i32 [ %6, %.lr.ph.i.i ], [ 0, %1 ]
  %5 = phi i32 [ %8, %.lr.ph.i.i ], [ 2, %1 ]
  %6 = phi i32 [ %7, %.lr.ph.i.i ], [ 1, %1 ]
  %7 = add i32 %6, %4
  %8 = add i32 %5, 1
  %.not.i.i = icmp sgt i32 %8, %3
  br i1 %.not.i.i, label %_mlir_ciface_execute.exit, label %.lr.ph.i.i

_mlir_ciface_execute.exit:                        ; preds = %.lr.ph.i.i, %1
  %.lcssa.i.i = phi i32 [ 1, %1 ], [ %7, %.lr.ph.i.i ]
  %9 = getelementptr i8, ptr %0, i64 8
  %10 = load ptr, ptr %9, align 8
  store i32 %.lcssa.i.i, ptr %10, align 4
  ret void
}

attributes #0 = { nofree norecurse nosync nounwind memory(none) }
attributes #1 = { nofree norecurse nosync nounwind memory(none) }
attributes #2 = { nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
