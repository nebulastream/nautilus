; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nofree norecurse nosync nounwind memory(none)
define signext i32 @execute(i32 %0, i32 %1) local_unnamed_addr #0 {
  %.not1 = icmp eq i32 %1, 0
  br i1 %.not1, label %._crit_edge, label %.lr.ph

.lr.ph:                                           ; preds = %2, %.lr.ph
  %3 = phi i32 [ %5, %.lr.ph ], [ %1, %2 ]
  %4 = phi i32 [ %3, %.lr.ph ], [ %0, %2 ]
  %5 = srem i32 %4, %3
  %.not = icmp eq i32 %5, 0
  br i1 %.not, label %._crit_edge, label %.lr.ph

._crit_edge:                                      ; preds = %.lr.ph, %2
  %.lcssa = phi i32 [ %0, %2 ], [ %3, %.lr.ph ]
  ret i32 %.lcssa
}

; Function Attrs: nofree norecurse nosync nounwind memory(none)
define signext i32 @_mlir_ciface_execute(i32 %0, i32 %1) local_unnamed_addr #1 {
  %.not1.i = icmp eq i32 %1, 0
  br i1 %.not1.i, label %execute.exit, label %.lr.ph.i

.lr.ph.i:                                         ; preds = %2, %.lr.ph.i
  %3 = phi i32 [ %5, %.lr.ph.i ], [ %1, %2 ]
  %4 = phi i32 [ %3, %.lr.ph.i ], [ %0, %2 ]
  %5 = srem i32 %4, %3
  %.not.i = icmp eq i32 %5, 0
  br i1 %.not.i, label %execute.exit, label %.lr.ph.i

execute.exit:                                     ; preds = %.lr.ph.i, %2
  %.lcssa.i = phi i32 [ %0, %2 ], [ %3, %.lr.ph.i ]
  ret i32 %.lcssa.i
}

; Function Attrs: nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: none)
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load i32, ptr %5, align 4
  %.not1.i = icmp eq i32 %6, 0
  br i1 %.not1.i, label %execute.exit, label %.lr.ph.i

.lr.ph.i:                                         ; preds = %1, %.lr.ph.i
  %7 = phi i32 [ %9, %.lr.ph.i ], [ %6, %1 ]
  %8 = phi i32 [ %7, %.lr.ph.i ], [ %3, %1 ]
  %9 = srem i32 %8, %7
  %.not.i = icmp eq i32 %9, 0
  br i1 %.not.i, label %execute.exit, label %.lr.ph.i

execute.exit:                                     ; preds = %.lr.ph.i, %1
  %.lcssa.i = phi i32 [ %3, %1 ], [ %7, %.lr.ph.i ]
  %10 = getelementptr i8, ptr %0, i64 16
  %11 = load ptr, ptr %10, align 8
  store i32 %.lcssa.i, ptr %11, align 4
  ret void
}

; Function Attrs: nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: none)
define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load i32, ptr %5, align 4
  %.not1.i.i = icmp eq i32 %6, 0
  br i1 %.not1.i.i, label %_mlir_ciface_execute.exit, label %.lr.ph.i.i

.lr.ph.i.i:                                       ; preds = %1, %.lr.ph.i.i
  %7 = phi i32 [ %9, %.lr.ph.i.i ], [ %6, %1 ]
  %8 = phi i32 [ %7, %.lr.ph.i.i ], [ %3, %1 ]
  %9 = srem i32 %8, %7
  %.not.i.i = icmp eq i32 %9, 0
  br i1 %.not.i.i, label %_mlir_ciface_execute.exit, label %.lr.ph.i.i

_mlir_ciface_execute.exit:                        ; preds = %.lr.ph.i.i, %1
  %.lcssa.i.i = phi i32 [ %3, %1 ], [ %7, %.lr.ph.i.i ]
  %10 = getelementptr i8, ptr %0, i64 16
  %11 = load ptr, ptr %10, align 8
  store i32 %.lcssa.i.i, ptr %11, align 4
  ret void
}

attributes #0 = { nofree norecurse nosync nounwind memory(none) }
attributes #1 = { nofree norecurse nosync nounwind memory(none) }
attributes #2 = { nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
