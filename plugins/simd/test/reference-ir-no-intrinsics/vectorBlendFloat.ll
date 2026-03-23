; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:o-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "arm64-apple-darwin25.2.0"

define void @execute(ptr %0, ptr %1, ptr %2, ptr %3) local_unnamed_addr #0 {
  %5 = tail call ptr @runtimeFunc0(ptr %0)
  %6 = tail call ptr @runtimeFunc0(ptr %1)
  %7 = tail call ptr @runtimeFunc0(ptr %2)
  %8 = tail call ptr @runtimeFunc1(ptr %7, ptr %5, ptr %6)
  tail call void @runtimeFunc2(ptr %3, ptr %8)
  ret void
}

define void @_mlir_ciface_execute(ptr %0, ptr %1, ptr %2, ptr %3) local_unnamed_addr #0 {
  %5 = tail call ptr @runtimeFunc0(ptr %0)
  %6 = tail call ptr @runtimeFunc0(ptr %1)
  %7 = tail call ptr @runtimeFunc0(ptr %2)
  %8 = tail call ptr @runtimeFunc1(ptr %7, ptr %5, ptr %6)
  tail call void @runtimeFunc2(ptr %3, ptr %8)
  ret void
}

; Function Attrs: memory(readwrite)
declare ptr @runtimeFunc0(ptr) local_unnamed_addr #1

; Function Attrs: memory(readwrite)
declare ptr @runtimeFunc1(ptr, ptr, ptr) local_unnamed_addr #1

; Function Attrs: memory(readwrite)
declare void @runtimeFunc2(ptr, ptr) local_unnamed_addr #1

define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #0 {
  %2 = load ptr, ptr %0, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load ptr, ptr %5, align 8
  %7 = getelementptr i8, ptr %0, i64 16
  %8 = load ptr, ptr %7, align 8
  %9 = load ptr, ptr %8, align 8
  %10 = getelementptr i8, ptr %0, i64 24
  %11 = load ptr, ptr %10, align 8
  %12 = load ptr, ptr %11, align 8
  %13 = tail call ptr @runtimeFunc0(ptr %3)
  %14 = tail call ptr @runtimeFunc0(ptr %6)
  %15 = tail call ptr @runtimeFunc0(ptr %9)
  %16 = tail call ptr @runtimeFunc1(ptr %15, ptr %13, ptr %14)
  tail call void @runtimeFunc2(ptr %12, ptr %16)
  ret void
}

define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #0 {
  %2 = load ptr, ptr %0, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load ptr, ptr %5, align 8
  %7 = getelementptr i8, ptr %0, i64 16
  %8 = load ptr, ptr %7, align 8
  %9 = load ptr, ptr %8, align 8
  %10 = getelementptr i8, ptr %0, i64 24
  %11 = load ptr, ptr %10, align 8
  %12 = load ptr, ptr %11, align 8
  %13 = tail call ptr @runtimeFunc0(ptr %3)
  %14 = tail call ptr @runtimeFunc0(ptr %6)
  %15 = tail call ptr @runtimeFunc0(ptr %9)
  %16 = tail call ptr @runtimeFunc1(ptr %15, ptr %13, ptr %14)
  tail call void @runtimeFunc2(ptr %12, ptr %16)
  ret void
}

attributes #0 = { "target-features" }
attributes #1 = { memory(readwrite) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
