; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: memory(readwrite)
declare i32 @runtimeFunc0() local_unnamed_addr #0

; Function Attrs: memory(readwrite)
declare i32 @runtimeFunc1(i32, i32) local_unnamed_addr #0

; Function Attrs: memory(readwrite)
define signext i32 @execute(i32 %0) local_unnamed_addr #1 {
  %2 = tail call i32 @runtimeFunc0()
  %3 = sext i32 %2 to i64
  %.not = icmp eq i32 %2, 0
  br i1 %.not, label %._crit_edge, label %.lr.ph

.lr.ph:                                           ; preds = %1, %.lr.ph
  %4 = phi i64 [ %7, %.lr.ph ], [ 0, %1 ]
  %5 = phi i32 [ %6, %.lr.ph ], [ 0, %1 ]
  %6 = tail call i32 @runtimeFunc1(i32 %5, i32 %0)
  %7 = add nuw i64 %4, 1
  %exitcond.not = icmp eq i64 %7, %3
  br i1 %exitcond.not, label %._crit_edge, label %.lr.ph

._crit_edge:                                      ; preds = %.lr.ph, %1
  %.lcssa = phi i32 [ 0, %1 ], [ %6, %.lr.ph ]
  ret i32 %.lcssa
}

; Function Attrs: memory(readwrite)
define signext i32 @_mlir_ciface_execute(i32 %0) local_unnamed_addr #0 {
  %2 = tail call i32 @runtimeFunc0()
  %3 = sext i32 %2 to i64
  %.not.i = icmp eq i32 %2, 0
  br i1 %.not.i, label %execute.exit, label %.lr.ph.i

.lr.ph.i:                                         ; preds = %1, %.lr.ph.i
  %4 = phi i64 [ %7, %.lr.ph.i ], [ 0, %1 ]
  %5 = phi i32 [ %6, %.lr.ph.i ], [ 0, %1 ]
  %6 = tail call i32 @runtimeFunc1(i32 %5, i32 %0)
  %7 = add nuw i64 %4, 1
  %exitcond.not.i = icmp eq i64 %7, %3
  br i1 %exitcond.not.i, label %execute.exit, label %.lr.ph.i

execute.exit:                                     ; preds = %.lr.ph.i, %1
  %.lcssa.i = phi i32 [ 0, %1 ], [ %6, %.lr.ph.i ]
  ret i32 %.lcssa.i
}

define void @_mlir_execute(ptr readonly %0) local_unnamed_addr {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = tail call i32 @runtimeFunc0()
  %5 = sext i32 %4 to i64
  %.not.i = icmp eq i32 %4, 0
  br i1 %.not.i, label %execute.exit, label %.lr.ph.i

.lr.ph.i:                                         ; preds = %1, %.lr.ph.i
  %6 = phi i64 [ %9, %.lr.ph.i ], [ 0, %1 ]
  %7 = phi i32 [ %8, %.lr.ph.i ], [ 0, %1 ]
  %8 = tail call i32 @runtimeFunc1(i32 %7, i32 %3)
  %9 = add nuw i64 %6, 1
  %exitcond.not.i = icmp eq i64 %9, %5
  br i1 %exitcond.not.i, label %execute.exit, label %.lr.ph.i

execute.exit:                                     ; preds = %.lr.ph.i, %1
  %.lcssa.i = phi i32 [ 0, %1 ], [ %8, %.lr.ph.i ]
  %10 = getelementptr i8, ptr %0, i64 8
  %11 = load ptr, ptr %10, align 8
  store i32 %.lcssa.i, ptr %11, align 4
  ret void
}

define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = tail call i32 @runtimeFunc0()
  %5 = sext i32 %4 to i64
  %.not.i.i = icmp eq i32 %4, 0
  br i1 %.not.i.i, label %_mlir_ciface_execute.exit, label %.lr.ph.i.i

.lr.ph.i.i:                                       ; preds = %1, %.lr.ph.i.i
  %6 = phi i64 [ %9, %.lr.ph.i.i ], [ 0, %1 ]
  %7 = phi i32 [ %8, %.lr.ph.i.i ], [ 0, %1 ]
  %8 = tail call i32 @runtimeFunc1(i32 %7, i32 %3)
  %9 = add nuw i64 %6, 1
  %exitcond.not.i.i = icmp eq i64 %9, %5
  br i1 %exitcond.not.i.i, label %_mlir_ciface_execute.exit, label %.lr.ph.i.i

_mlir_ciface_execute.exit:                        ; preds = %.lr.ph.i.i, %1
  %.lcssa.i.i = phi i32 [ 0, %1 ], [ %8, %.lr.ph.i.i ]
  %10 = getelementptr i8, ptr %0, i64 8
  %11 = load ptr, ptr %10, align 8
  store i32 %.lcssa.i.i, ptr %11, align 4
  ret void
}

attributes #0 = { memory(readwrite) }
attributes #1 = { memory(readwrite) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
