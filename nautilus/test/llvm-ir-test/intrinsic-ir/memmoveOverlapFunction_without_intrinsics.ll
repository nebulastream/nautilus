; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: memory(readwrite)
declare ptr @runtimeFunc0(ptr, ptr, i64) local_unnamed_addr #0

; Function Attrs: memory(readwrite)
define ptr @execute(ptr %0, ptr %1, i64 %2) local_unnamed_addr #1 {
  %4 = tail call ptr @runtimeFunc0(ptr %0, ptr %1, i64 %2)
  ret ptr %4
}

; Function Attrs: memory(readwrite)
define ptr @_mlir_ciface_execute(ptr %0, ptr %1, i64 %2) local_unnamed_addr #0 {
  %4 = tail call ptr @runtimeFunc0(ptr %0, ptr %1, i64 %2)
  ret ptr %4
}

define void @_mlir_execute(ptr readonly %0) local_unnamed_addr {
  %2 = load ptr, ptr %0, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load ptr, ptr %5, align 8
  %7 = getelementptr i8, ptr %0, i64 16
  %8 = load ptr, ptr %7, align 8
  %9 = load i64, ptr %8, align 8
  %10 = tail call ptr @runtimeFunc0(ptr %3, ptr %6, i64 %9)
  %11 = getelementptr i8, ptr %0, i64 24
  %12 = load ptr, ptr %11, align 8
  store ptr %10, ptr %12, align 8
  ret void
}

define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr {
  %2 = load ptr, ptr %0, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load ptr, ptr %5, align 8
  %7 = getelementptr i8, ptr %0, i64 16
  %8 = load ptr, ptr %7, align 8
  %9 = load i64, ptr %8, align 8
  %10 = tail call ptr @runtimeFunc0(ptr %3, ptr %6, i64 %9)
  %11 = getelementptr i8, ptr %0, i64 24
  %12 = load ptr, ptr %11, align 8
  store ptr %10, ptr %12, align 8
  ret void
}

attributes #0 = { memory(readwrite) }
attributes #1 = { memory(readwrite) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
