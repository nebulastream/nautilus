; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nounwind memory(readwrite)
declare void @runtimeFunc2(ptr) local_unnamed_addr #0

; Function Attrs: nounwind memory(readwrite)
declare void @runtimeFunc1(ptr, i32) local_unnamed_addr #0

; Function Attrs: nounwind memory(readwrite)
declare void @runtimeFunc0(ptr) local_unnamed_addr #0

; Function Attrs: nounwind
define noundef signext i32 @execute() local_unnamed_addr #1 {
  %1 = alloca i8, align 1
  call void @runtimeFunc0(ptr nonnull %1)
  call void @runtimeFunc1(ptr nonnull %1, i32 3)
  call void @runtimeFunc2(ptr nonnull %1)
  ret i32 42
}

attributes #0 = { nounwind memory(readwrite) }
attributes #1 = { nounwind }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
