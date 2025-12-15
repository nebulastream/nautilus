; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: memory(readwrite)
declare i32 @runtimeFunc0(i32) local_unnamed_addr #0

; Function Attrs: memory(readwrite)
declare i32 @runtimeFunc1(i32) local_unnamed_addr #0

; Function Attrs: memory(readwrite)
define signext i32 @execute(i32 %0) local_unnamed_addr #1 {
  %2 = tail call i32 @runtimeFunc0(i32 %0)
  %3 = tail call i32 @runtimeFunc1(i32 %2)
  ret i32 %3
}

; Function Attrs: memory(readwrite)
define signext i32 @_mlir_ciface_execute(i32 %0) local_unnamed_addr #0 {
  %2 = tail call i32 @runtimeFunc0(i32 %0)
  %3 = tail call i32 @runtimeFunc1(i32 %2)
  ret i32 %3
}

define void @_mlir_execute(ptr readonly %0) local_unnamed_addr {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = tail call i32 @runtimeFunc0(i32 %3)
  %5 = tail call i32 @runtimeFunc1(i32 %4)
  %6 = getelementptr i8, ptr %0, i64 8
  %7 = load ptr, ptr %6, align 8
  store i32 %5, ptr %7, align 4
  ret void
}

define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = tail call i32 @runtimeFunc0(i32 %3)
  %5 = tail call i32 @runtimeFunc1(i32 %4)
  %6 = getelementptr i8, ptr %0, i64 8
  %7 = load ptr, ptr %6, align 8
  store i32 %5, ptr %7, align 4
  ret void
}

attributes #0 = { memory(readwrite) }
attributes #1 = { memory(readwrite) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
