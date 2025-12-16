; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: memory(readwrite)
declare i32 @runtimeFunc0(i32, i32) local_unnamed_addr #0

; Function Attrs: memory(readwrite)
define signext i32 @execute(i32 %0, i32 %1) local_unnamed_addr #1 {
  %3 = icmp sgt i32 %0, 0
  br i1 %3, label %.lr.ph, label %._crit_edge

.lr.ph:                                           ; preds = %2, %.lr.ph
  %4 = phi i32 [ %7, %.lr.ph ], [ 0, %2 ]
  %5 = phi i32 [ %6, %.lr.ph ], [ 0, %2 ]
  %6 = tail call i32 @runtimeFunc0(i32 %5, i32 %1)
  %7 = add nuw nsw i32 %4, 1
  %exitcond.not = icmp eq i32 %7, %0
  br i1 %exitcond.not, label %._crit_edge, label %.lr.ph

._crit_edge:                                      ; preds = %.lr.ph, %2
  %.lcssa = phi i32 [ 0, %2 ], [ %6, %.lr.ph ]
  ret i32 %.lcssa
}

; Function Attrs: memory(readwrite)
define signext i32 @_mlir_ciface_execute(i32 %0, i32 %1) local_unnamed_addr #0 {
  %3 = icmp sgt i32 %0, 0
  br i1 %3, label %.lr.ph.i, label %execute.exit

.lr.ph.i:                                         ; preds = %2, %.lr.ph.i
  %4 = phi i32 [ %7, %.lr.ph.i ], [ 0, %2 ]
  %5 = phi i32 [ %6, %.lr.ph.i ], [ 0, %2 ]
  %6 = tail call i32 @runtimeFunc0(i32 %5, i32 %1)
  %7 = add nuw nsw i32 %4, 1
  %exitcond.not.i = icmp eq i32 %7, %0
  br i1 %exitcond.not.i, label %execute.exit, label %.lr.ph.i

execute.exit:                                     ; preds = %.lr.ph.i, %2
  %.lcssa.i = phi i32 [ 0, %2 ], [ %6, %.lr.ph.i ]
  ret i32 %.lcssa.i
}

define void @_mlir_execute(ptr readonly %0) local_unnamed_addr {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load i32, ptr %5, align 4
  %7 = icmp sgt i32 %3, 0
  br i1 %7, label %.lr.ph.i, label %execute.exit

.lr.ph.i:                                         ; preds = %1, %.lr.ph.i
  %8 = phi i32 [ %11, %.lr.ph.i ], [ 0, %1 ]
  %9 = phi i32 [ %10, %.lr.ph.i ], [ 0, %1 ]
  %10 = tail call i32 @runtimeFunc0(i32 %9, i32 %6)
  %11 = add nuw nsw i32 %8, 1
  %exitcond.not.i = icmp eq i32 %11, %3
  br i1 %exitcond.not.i, label %execute.exit, label %.lr.ph.i

execute.exit:                                     ; preds = %.lr.ph.i, %1
  %.lcssa.i = phi i32 [ 0, %1 ], [ %10, %.lr.ph.i ]
  %12 = getelementptr i8, ptr %0, i64 16
  %13 = load ptr, ptr %12, align 8
  store i32 %.lcssa.i, ptr %13, align 4
  ret void
}

define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load i32, ptr %5, align 4
  %7 = icmp sgt i32 %3, 0
  br i1 %7, label %.lr.ph.i.i, label %_mlir_ciface_execute.exit

.lr.ph.i.i:                                       ; preds = %1, %.lr.ph.i.i
  %8 = phi i32 [ %11, %.lr.ph.i.i ], [ 0, %1 ]
  %9 = phi i32 [ %10, %.lr.ph.i.i ], [ 0, %1 ]
  %10 = tail call i32 @runtimeFunc0(i32 %9, i32 %6)
  %11 = add nuw nsw i32 %8, 1
  %exitcond.not.i.i = icmp eq i32 %11, %3
  br i1 %exitcond.not.i.i, label %_mlir_ciface_execute.exit, label %.lr.ph.i.i

_mlir_ciface_execute.exit:                        ; preds = %.lr.ph.i.i, %1
  %.lcssa.i.i = phi i32 [ 0, %1 ], [ %10, %.lr.ph.i.i ]
  %12 = getelementptr i8, ptr %0, i64 16
  %13 = load ptr, ptr %12, align 8
  store i32 %.lcssa.i.i, ptr %13, align 4
  ret void
}

attributes #0 = { memory(readwrite) }
attributes #1 = { memory(readwrite) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
