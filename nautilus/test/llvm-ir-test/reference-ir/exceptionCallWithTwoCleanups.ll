; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

define noundef signext i32 @execute() local_unnamed_addr #0 personality ptr @__gxx_personality_v0 {
  %1 = alloca i8, align 1
  %2 = alloca i8, align 1
  call void @runtimeFunc0(ptr nonnull %1)
  call void @runtimeFunc2(ptr nonnull %1, i32 1)
  call void @runtimeFunc0(ptr nonnull %2)
  call void @runtimeFunc2(ptr nonnull %2, i32 2)
  invoke void @runtimeFunc3(ptr nonnull %2)
          to label %3 unwind label %4

3:                                                ; preds = %0
  call void @runtimeFunc1(ptr nonnull %2)
  call void @runtimeFunc1(ptr nonnull %1)
  ret i32 0

4:                                                ; preds = %0
  %5 = landingpad { ptr, i32 }
          cleanup
  call void @runtimeFunc1(ptr nonnull %2)
  call void @runtimeFunc1(ptr nonnull %1)
  resume { ptr, i32 } %5
}

define noundef signext i32 @_mlir_ciface_execute() local_unnamed_addr #0 personality ptr @__gxx_personality_v0 {
  %1 = tail call i32 @execute()
  ret i32 0
}

; Function Attrs: nounwind memory(readwrite)
declare void @runtimeFunc0(ptr) local_unnamed_addr #1

; Function Attrs: nounwind memory(readwrite)
declare void @runtimeFunc2(ptr, i32) local_unnamed_addr #1

; Function Attrs: memory(readwrite)
declare void @runtimeFunc3(ptr) local_unnamed_addr #2

; Function Attrs: nounwind memory(readwrite)
declare void @runtimeFunc1(ptr) local_unnamed_addr #1

declare i32 @__gxx_personality_v0(...)

define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #0 {
  %2 = tail call i32 @execute()
  %3 = load ptr, ptr %0, align 8
  store i32 0, ptr %3, align 4
  ret void
}

define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #0 personality ptr @__gxx_personality_v0 {
  %2 = tail call i32 @execute()
  %3 = load ptr, ptr %0, align 8
  store i32 0, ptr %3, align 4
  ret void
}

attributes #1 = { nounwind memory(readwrite) }
attributes #2 = { memory(readwrite) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
