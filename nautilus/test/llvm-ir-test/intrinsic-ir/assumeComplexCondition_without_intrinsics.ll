; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: memory(readwrite)
declare void @runtimeFunc0(i1) local_unnamed_addr #0

; Function Attrs: memory(readwrite)
define signext i32 @execute(i32 %0, i32 %1) local_unnamed_addr #1 {
  %3 = icmp sgt i32 %0, 0
  %4 = icmp sgt i32 %1, 0
  %5 = and i1 %3, %4
  tail call void @runtimeFunc0(i1 %5)
  %6 = add i32 %1, %0
  ret i32 %6
}

; Function Attrs: memory(readwrite)
define signext i32 @_mlir_ciface_execute(i32 %0, i32 %1) local_unnamed_addr #0 {
  %3 = icmp sgt i32 %0, 0
  %4 = icmp sgt i32 %1, 0
  %5 = and i1 %3, %4
  tail call void @runtimeFunc0(i1 %5)
  %6 = add i32 %1, %0
  ret i32 %6
}

define void @_mlir_execute(ptr readonly %0) local_unnamed_addr {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load i32, ptr %5, align 4
  %7 = icmp sgt i32 %3, 0
  %8 = icmp sgt i32 %6, 0
  %9 = and i1 %7, %8
  tail call void @runtimeFunc0(i1 %9)
  %10 = add i32 %6, %3
  %11 = getelementptr i8, ptr %0, i64 16
  %12 = load ptr, ptr %11, align 8
  store i32 %10, ptr %12, align 4
  ret void
}

define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load i32, ptr %5, align 4
  %7 = icmp sgt i32 %3, 0
  %8 = icmp sgt i32 %6, 0
  %9 = and i1 %7, %8
  tail call void @runtimeFunc0(i1 %9)
  %10 = add i32 %6, %3
  %11 = getelementptr i8, ptr %0, i64 16
  %12 = load ptr, ptr %11, align 8
  store i32 %10, ptr %12, align 4
  ret void
}

attributes #0 = { memory(readwrite) }
attributes #1 = { memory(readwrite) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
