; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: memory(readwrite)
declare float @runtimeFunc0(float) local_unnamed_addr #0

; Function Attrs: memory(readwrite)
declare float @runtimeFunc1(float) local_unnamed_addr #0

; Function Attrs: memory(readwrite)
declare float @runtimeFunc2(float) local_unnamed_addr #0

; Function Attrs: memory(readwrite)
define float @execute(float %0, float %1) local_unnamed_addr #1 {
  %3 = tail call float @runtimeFunc0(float %0)
  %4 = tail call float @runtimeFunc1(float %1)
  %5 = fmul fast float %4, %4
  %6 = fmul fast float %3, %3
  %7 = fadd fast float %5, %6
  %8 = tail call float @runtimeFunc2(float %7)
  ret float %8
}

; Function Attrs: memory(readwrite)
define float @_mlir_ciface_execute(float %0, float %1) local_unnamed_addr #0 {
  %3 = tail call float @runtimeFunc0(float %0)
  %4 = tail call float @runtimeFunc1(float %1)
  %5 = fmul fast float %4, %4
  %6 = fmul fast float %3, %3
  %7 = fadd fast float %5, %6
  %8 = tail call float @runtimeFunc2(float %7)
  ret float %8
}

define void @_mlir_execute(ptr readonly %0) local_unnamed_addr {
  %2 = load ptr, ptr %0, align 8
  %3 = load float, ptr %2, align 4
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load float, ptr %5, align 4
  %7 = tail call float @runtimeFunc0(float %3)
  %8 = tail call float @runtimeFunc1(float %6)
  %9 = fmul fast float %8, %8
  %10 = fmul fast float %7, %7
  %11 = fadd fast float %9, %10
  %12 = tail call float @runtimeFunc2(float %11)
  %13 = getelementptr i8, ptr %0, i64 16
  %14 = load ptr, ptr %13, align 8
  store float %12, ptr %14, align 4
  ret void
}

define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr {
  %2 = load ptr, ptr %0, align 8
  %3 = load float, ptr %2, align 4
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load float, ptr %5, align 4
  %7 = tail call float @runtimeFunc0(float %3)
  %8 = tail call float @runtimeFunc1(float %6)
  %9 = fmul fast float %8, %8
  %10 = fmul fast float %7, %7
  %11 = fadd fast float %9, %10
  %12 = tail call float @runtimeFunc2(float %11)
  %13 = getelementptr i8, ptr %0, i64 16
  %14 = load ptr, ptr %13, align 8
  store float %12, ptr %14, align 4
  ret void
}

attributes #0 = { memory(readwrite) }
attributes #1 = { memory(readwrite) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
