; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

define noundef signext i32 @execute() local_unnamed_addr #0 {
  tail call void @runtimeFunc0()
  ret i32 42
}

define noundef signext i32 @_mlir_ciface_execute() local_unnamed_addr #0 {
  tail call void @runtimeFunc0()
  ret i32 42
}

; Function Attrs: memory(readwrite)
declare void @runtimeFunc0() local_unnamed_addr #1

define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #0 {
  tail call void @runtimeFunc0()
  %2 = load ptr, ptr %0, align 8
  store i32 42, ptr %2, align 4
  ret void
}

define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #0 {
  tail call void @runtimeFunc0()
  %2 = load ptr, ptr %0, align 8
  store i32 42, ptr %2, align 4
  ret void
}

attributes #1 = { memory(readwrite) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
