; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: memory(readwrite, errnomem: none)
declare i32 @runtimeFunc0(i1) local_unnamed_addr #0

; Function Attrs: memory(readwrite, errnomem: none)
define signext i32 @execute(i1 %0) local_unnamed_addr #1 {
  %2 = tail call i32 @runtimeFunc0(i1 %0)
  %3 = tail call i32 @runtimeFunc0(i1 false)
  %4 = tail call i32 @runtimeFunc0(i1 false)
  %5 = tail call i32 @runtimeFunc0(i1 false)
  %6 = tail call i32 @runtimeFunc0(i1 false)
  ret i32 %6
}

; Function Attrs: memory(readwrite, errnomem: none)
define signext i32 @_mlir_ciface_execute(i1 %0) local_unnamed_addr #0 {
  %2 = tail call i32 @runtimeFunc0(i1 %0)
  %3 = tail call i32 @runtimeFunc0(i1 false)
  %4 = tail call i32 @runtimeFunc0(i1 false)
  %5 = tail call i32 @runtimeFunc0(i1 false)
  %6 = tail call i32 @runtimeFunc0(i1 false)
  ret i32 %6
}

define void @_mlir_execute(ptr readonly %0) local_unnamed_addr {
  %2 = load ptr, ptr %0, align 8
  %3 = load i1, ptr %2, align 1
  %4 = tail call i32 @runtimeFunc0(i1 %3)
  %5 = tail call i32 @runtimeFunc0(i1 false)
  %6 = tail call i32 @runtimeFunc0(i1 false)
  %7 = tail call i32 @runtimeFunc0(i1 false)
  %8 = tail call i32 @runtimeFunc0(i1 false)
  %9 = getelementptr i8, ptr %0, i64 8
  %10 = load ptr, ptr %9, align 8
  store i32 %8, ptr %10, align 4
  ret void
}

define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr {
  %2 = load ptr, ptr %0, align 8
  %3 = load i1, ptr %2, align 1
  %4 = tail call i32 @runtimeFunc0(i1 %3)
  %5 = tail call i32 @runtimeFunc0(i1 false)
  %6 = tail call i32 @runtimeFunc0(i1 false)
  %7 = tail call i32 @runtimeFunc0(i1 false)
  %8 = tail call i32 @runtimeFunc0(i1 false)
  %9 = getelementptr i8, ptr %0, i64 8
  %10 = load ptr, ptr %9, align 8
  store i32 %8, ptr %10, align 4
  ret void
}

attributes #0 = { memory(readwrite, errnomem: none) }
attributes #1 = { memory(readwrite, errnomem: none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
