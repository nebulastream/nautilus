; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nofree norecurse nosync nounwind memory(none)
define signext i32 @execute(i32 %0) local_unnamed_addr #0 {
  %.not1 = icmp eq i32 %0, 1
  br i1 %.not1, label %._crit_edge, label %.lr.ph

.lr.ph:                                           ; preds = %1, %.lr.ph
  %2 = phi i32 [ %9, %.lr.ph ], [ %0, %1 ]
  %3 = phi i32 [ %10, %.lr.ph ], [ 0, %1 ]
  %4 = and i32 %2, 1
  %5 = icmp eq i32 %4, 0
  %6 = mul i32 %2, 3
  %7 = add i32 %6, 1
  %8 = ashr exact i32 %2, 1
  %9 = select i1 %5, i32 %8, i32 %7
  %10 = add i32 %3, 1
  %.not = icmp eq i32 %9, 1
  br i1 %.not, label %._crit_edge, label %.lr.ph

._crit_edge:                                      ; preds = %.lr.ph, %1
  %.lcssa = phi i32 [ 0, %1 ], [ %10, %.lr.ph ]
  ret i32 %.lcssa
}

; Function Attrs: nofree norecurse nosync nounwind memory(none)
define signext i32 @_mlir_ciface_execute(i32 %0) local_unnamed_addr #1 {
  %.not1.i = icmp eq i32 %0, 1
  br i1 %.not1.i, label %execute.exit, label %.lr.ph.i

.lr.ph.i:                                         ; preds = %1, %.lr.ph.i
  %2 = phi i32 [ %9, %.lr.ph.i ], [ %0, %1 ]
  %3 = phi i32 [ %10, %.lr.ph.i ], [ 0, %1 ]
  %4 = and i32 %2, 1
  %5 = icmp eq i32 %4, 0
  %6 = mul i32 %2, 3
  %7 = add i32 %6, 1
  %8 = ashr exact i32 %2, 1
  %9 = select i1 %5, i32 %8, i32 %7
  %10 = add i32 %3, 1
  %.not.i = icmp eq i32 %9, 1
  br i1 %.not.i, label %execute.exit, label %.lr.ph.i

execute.exit:                                     ; preds = %.lr.ph.i, %1
  %.lcssa.i = phi i32 [ 0, %1 ], [ %10, %.lr.ph.i ]
  ret i32 %.lcssa.i
}

; Function Attrs: nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: none)
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %.not1.i = icmp eq i32 %3, 1
  br i1 %.not1.i, label %execute.exit, label %.lr.ph.i

.lr.ph.i:                                         ; preds = %1, %.lr.ph.i
  %4 = phi i32 [ %11, %.lr.ph.i ], [ %3, %1 ]
  %5 = phi i32 [ %12, %.lr.ph.i ], [ 0, %1 ]
  %6 = and i32 %4, 1
  %7 = icmp eq i32 %6, 0
  %8 = mul i32 %4, 3
  %9 = add i32 %8, 1
  %10 = ashr exact i32 %4, 1
  %11 = select i1 %7, i32 %10, i32 %9
  %12 = add i32 %5, 1
  %.not.i = icmp eq i32 %11, 1
  br i1 %.not.i, label %execute.exit, label %.lr.ph.i

execute.exit:                                     ; preds = %.lr.ph.i, %1
  %.lcssa.i = phi i32 [ 0, %1 ], [ %12, %.lr.ph.i ]
  %13 = getelementptr i8, ptr %0, i64 8
  %14 = load ptr, ptr %13, align 8
  store i32 %.lcssa.i, ptr %14, align 4
  ret void
}

; Function Attrs: nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: none)
define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %.not1.i.i = icmp eq i32 %3, 1
  br i1 %.not1.i.i, label %_mlir_ciface_execute.exit, label %.lr.ph.i.i

.lr.ph.i.i:                                       ; preds = %1, %.lr.ph.i.i
  %4 = phi i32 [ %11, %.lr.ph.i.i ], [ %3, %1 ]
  %5 = phi i32 [ %12, %.lr.ph.i.i ], [ 0, %1 ]
  %6 = and i32 %4, 1
  %7 = icmp eq i32 %6, 0
  %8 = mul i32 %4, 3
  %9 = add i32 %8, 1
  %10 = ashr exact i32 %4, 1
  %11 = select i1 %7, i32 %10, i32 %9
  %12 = add i32 %5, 1
  %.not.i.i = icmp eq i32 %11, 1
  br i1 %.not.i.i, label %_mlir_ciface_execute.exit, label %.lr.ph.i.i

_mlir_ciface_execute.exit:                        ; preds = %.lr.ph.i.i, %1
  %.lcssa.i.i = phi i32 [ 0, %1 ], [ %12, %.lr.ph.i.i ]
  %13 = getelementptr i8, ptr %0, i64 8
  %14 = load ptr, ptr %13, align 8
  store i32 %.lcssa.i.i, ptr %14, align 4
  ret void
}

attributes #0 = { nofree norecurse nosync nounwind memory(none) }
attributes #1 = { nofree norecurse nosync nounwind memory(none) }
attributes #2 = { nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
