; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: memory(readwrite, errnomem: none)
declare void @runtimeFunc0(i32, i32) local_unnamed_addr #0

; Function Attrs: memory(readwrite, errnomem: none)
define signext i32 @execute(i32 returned %0, i32 %1) local_unnamed_addr #1 {
  tail call void @runtimeFunc0(i32 %0, i32 %1)
  ret i32 %0
}

; Function Attrs: memory(readwrite, errnomem: none)
define signext i32 @_mlir_ciface_execute(i32 returned %0, i32 %1) local_unnamed_addr #0 {
  tail call void @runtimeFunc0(i32 %0, i32 %1)
  ret i32 %0
}

define void @_mlir_execute(ptr readonly %0) local_unnamed_addr {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load i32, ptr %5, align 4
  tail call void @runtimeFunc0(i32 %3, i32 %6)
  %7 = getelementptr i8, ptr %0, i64 16
  %8 = load ptr, ptr %7, align 8
  store i32 %3, ptr %8, align 4
  ret void
}

define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load i32, ptr %5, align 4
  tail call void @runtimeFunc0(i32 %3, i32 %6)
  %7 = getelementptr i8, ptr %0, i64 16
  %8 = load ptr, ptr %7, align 8
  store i32 %3, ptr %8, align 4
  ret void
}

attributes #0 = { memory(readwrite, errnomem: none) }
attributes #1 = { memory(readwrite, errnomem: none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
