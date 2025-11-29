; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: memory(readwrite)
declare i32 @runtimeFunc0(i32) local_unnamed_addr #0

; Function Attrs: memory(readwrite)
define signext i32 @execute(i32 %0) local_unnamed_addr #1 {
  %2 = icmp eq i32 %0, 1
  br i1 %2, label %3, label %5, !prof !1

3:                                                ; preds = %5, %1
  %4 = phi i32 [ %6, %5 ], [ 42, %1 ]
  ret i32 %4

5:                                                ; preds = %1
  %6 = tail call i32 @runtimeFunc0(i32 %0)
  br label %3
}

; Function Attrs: memory(readwrite)
define signext i32 @_mlir_ciface_execute(i32 %0) local_unnamed_addr #0 {
  %2 = icmp eq i32 %0, 1
  br i1 %2, label %execute.exit, label %3, !prof !1

3:                                                ; preds = %1
  %4 = tail call i32 @runtimeFunc0(i32 %0)
  br label %execute.exit

execute.exit:                                     ; preds = %1, %3
  %5 = phi i32 [ %4, %3 ], [ 42, %1 ]
  ret i32 %5
}

define void @_mlir_execute(ptr readonly %0) local_unnamed_addr {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = icmp eq i32 %3, 1
  br i1 %4, label %execute.exit, label %5, !prof !1

5:                                                ; preds = %1
  %6 = tail call i32 @runtimeFunc0(i32 %3)
  br label %execute.exit

execute.exit:                                     ; preds = %1, %5
  %7 = phi i32 [ %6, %5 ], [ 42, %1 ]
  %8 = getelementptr i8, ptr %0, i64 8
  %9 = load ptr, ptr %8, align 8
  store i32 %7, ptr %9, align 4
  ret void
}

define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = icmp eq i32 %3, 1
  br i1 %4, label %_mlir_ciface_execute.exit, label %5, !prof !1

5:                                                ; preds = %1
  %6 = tail call i32 @runtimeFunc0(i32 %3)
  br label %_mlir_ciface_execute.exit

_mlir_ciface_execute.exit:                        ; preds = %1, %5
  %7 = phi i32 [ %6, %5 ], [ 42, %1 ]
  %8 = getelementptr i8, ptr %0, i64 8
  %9 = load ptr, ptr %8, align 8
  store i32 %7, ptr %9, align 4
  ret void
}

attributes #0 = { memory(readwrite) }
attributes #1 = { memory(readwrite) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
!1 = !{!"branch_weights", i32 900, i32 99}
