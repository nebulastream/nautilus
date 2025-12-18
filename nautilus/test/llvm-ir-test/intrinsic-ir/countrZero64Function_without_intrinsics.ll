; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: memory(readwrite)
declare i64 @runtimeFunc0(i64) local_unnamed_addr #0

; Function Attrs: memory(readwrite)
define zeroext i64 @execute(i64 %0) local_unnamed_addr #1 {
  %2 = tail call i64 @runtimeFunc0(i64 %0)
  ret i64 %2
}

; Function Attrs: memory(readwrite)
define zeroext i64 @_mlir_ciface_execute(i64 %0) local_unnamed_addr #0 {
  %2 = tail call i64 @runtimeFunc0(i64 %0)
  ret i64 %2
}

define void @_mlir_execute(ptr readonly %0) local_unnamed_addr {
  %2 = load ptr, ptr %0, align 8
  %3 = load i64, ptr %2, align 8
  %4 = tail call i64 @runtimeFunc0(i64 %3)
  %5 = getelementptr i8, ptr %0, i64 8
  %6 = load ptr, ptr %5, align 8
  store i64 %4, ptr %6, align 8
  ret void
}

define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr {
  %2 = load ptr, ptr %0, align 8
  %3 = load i64, ptr %2, align 8
  %4 = tail call i64 @runtimeFunc0(i64 %3)
  %5 = getelementptr i8, ptr %0, i64 8
  %6 = load ptr, ptr %5, align 8
  store i64 %4, ptr %6, align 8
  ret void
}

attributes #0 = { memory(readwrite) }
attributes #1 = { memory(readwrite) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
