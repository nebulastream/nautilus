; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

define float @execute(ptr %0) local_unnamed_addr #0 {
  %2 = tail call ptr @runtimeFunc0(ptr %0)
  %3 = tail call float @runtimeFunc1(ptr %2)
  ret float %3
}

define float @_mlir_ciface_execute(ptr %0) local_unnamed_addr #0 {
  %2 = tail call ptr @runtimeFunc0(ptr %0)
  %3 = tail call float @runtimeFunc1(ptr %2)
  ret float %3
}

; Function Attrs: memory(readwrite)
declare ptr @runtimeFunc0(ptr) local_unnamed_addr #1

; Function Attrs: memory(readwrite)
declare float @runtimeFunc1(ptr) local_unnamed_addr #1

define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #0 {
  %2 = load ptr, ptr %0, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = tail call ptr @runtimeFunc0(ptr %3)
  %5 = tail call float @runtimeFunc1(ptr %4)
  %6 = getelementptr i8, ptr %0, i64 8
  %7 = load ptr, ptr %6, align 8
  store float %5, ptr %7, align 4
  ret void
}

define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #0 {
  %2 = load ptr, ptr %0, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = tail call ptr @runtimeFunc0(ptr %3)
  %5 = tail call float @runtimeFunc1(ptr %4)
  %6 = getelementptr i8, ptr %0, i64 8
  %7 = load ptr, ptr %6, align 8
  store float %5, ptr %7, align 4
  ret void
}

attributes #1 = { memory(readwrite) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
