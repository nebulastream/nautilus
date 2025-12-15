; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: memory(readwrite)
declare void @runtimeFunc0(ptr, i8) local_unnamed_addr #0

; Function Attrs: memory(readwrite)
define signext i32 @execute(ptr %0) local_unnamed_addr #1 {
  tail call void @runtimeFunc0(ptr %0, i8 16)
  %2 = load i32, ptr %0, align 4
  ret i32 %2
}

; Function Attrs: memory(readwrite)
define signext i32 @_mlir_ciface_execute(ptr %0) local_unnamed_addr #0 {
  tail call void @runtimeFunc0(ptr %0, i8 16)
  %2 = load i32, ptr %0, align 4
  ret i32 %2
}

define void @_mlir_execute(ptr readonly %0) local_unnamed_addr {
  %2 = load ptr, ptr %0, align 8
  %3 = load ptr, ptr %2, align 8
  tail call void @runtimeFunc0(ptr %3, i8 16)
  %4 = load i32, ptr %3, align 4
  %5 = getelementptr i8, ptr %0, i64 8
  %6 = load ptr, ptr %5, align 8
  store i32 %4, ptr %6, align 4
  ret void
}

define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr {
  %2 = load ptr, ptr %0, align 8
  %3 = load ptr, ptr %2, align 8
  tail call void @runtimeFunc0(ptr %3, i8 16)
  %4 = load i32, ptr %3, align 4
  %5 = getelementptr i8, ptr %0, i64 8
  %6 = load ptr, ptr %5, align 8
  store i32 %4, ptr %6, align 4
  ret void
}

attributes #0 = { memory(readwrite) }
attributes #1 = { memory(readwrite) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
