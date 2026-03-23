; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:o-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "arm64-apple-darwin25.2.0"

define float @execute(ptr %0, ptr %1) local_unnamed_addr #0 {
  %3 = tail call ptr @runtimeFunc0(ptr %0)
  %4 = tail call ptr @runtimeFunc0(ptr %1)
  %5 = tail call ptr @runtimeFunc1(ptr %3, ptr %4)
  %6 = tail call float @runtimeFunc2(ptr %5)
  ret float %6
}

define float @_mlir_ciface_execute(ptr %0, ptr %1) local_unnamed_addr #0 {
  %3 = tail call ptr @runtimeFunc0(ptr %0)
  %4 = tail call ptr @runtimeFunc0(ptr %1)
  %5 = tail call ptr @runtimeFunc1(ptr %3, ptr %4)
  %6 = tail call float @runtimeFunc2(ptr %5)
  ret float %6
}

; Function Attrs: memory(readwrite)
declare ptr @runtimeFunc0(ptr) local_unnamed_addr #1

; Function Attrs: memory(readwrite)
declare ptr @runtimeFunc1(ptr, ptr) local_unnamed_addr #1

; Function Attrs: memory(readwrite)
declare float @runtimeFunc2(ptr) local_unnamed_addr #1

define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #0 {
  %2 = load ptr, ptr %0, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load ptr, ptr %5, align 8
  %7 = tail call ptr @runtimeFunc0(ptr %3)
  %8 = tail call ptr @runtimeFunc0(ptr %6)
  %9 = tail call ptr @runtimeFunc1(ptr %7, ptr %8)
  %10 = tail call float @runtimeFunc2(ptr %9)
  %11 = getelementptr i8, ptr %0, i64 16
  %12 = load ptr, ptr %11, align 8
  store float %10, ptr %12, align 4
  ret void
}

define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #0 {
  %2 = load ptr, ptr %0, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load ptr, ptr %5, align 8
  %7 = tail call ptr @runtimeFunc0(ptr %3)
  %8 = tail call ptr @runtimeFunc0(ptr %6)
  %9 = tail call ptr @runtimeFunc1(ptr %7, ptr %8)
  %10 = tail call float @runtimeFunc2(ptr %9)
  %11 = getelementptr i8, ptr %0, i64 16
  %12 = load ptr, ptr %11, align 8
  store float %10, ptr %12, align 4
  ret void
}

attributes #0 = { "target-features" }
attributes #1 = { memory(readwrite) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
