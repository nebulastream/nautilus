; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

define noundef signext i32 @execute() local_unnamed_addr #0 personality ptr @__gxx_personality_v0 {
  %1 = alloca i8, align 1
  call void @runtimeFunc0(ptr nonnull %1)
  invoke void @runtimeFunc2(ptr nonnull %1)
          to label %2 unwind label %3

2:                                                ; preds = %0
  call void @runtimeFunc1(ptr nonnull %1)
  ret i32 42

3:                                                ; preds = %0
  %4 = landingpad { ptr, i32 }
          cleanup
  call void @runtimeFunc1(ptr nonnull %1)
  resume { ptr, i32 } %4
}

define noundef signext i32 @_mlir_ciface_execute() local_unnamed_addr #0 personality ptr @__gxx_personality_v0 {
  %1 = alloca i8, align 1
  call void @llvm.lifetime.start.p0(ptr nonnull %1)
  call void @runtimeFunc0(ptr nonnull %1)
  invoke void @runtimeFunc2(ptr nonnull %1)
          to label %execute.exit unwind label %2

2:                                                ; preds = %0
  %3 = landingpad { ptr, i32 }
          cleanup
  call void @runtimeFunc1(ptr nonnull %1)
  resume { ptr, i32 } %3

execute.exit:                                     ; preds = %0
  call void @runtimeFunc1(ptr nonnull %1)
  call void @llvm.lifetime.end.p0(ptr nonnull %1)
  ret i32 42
}

; Function Attrs: nounwind memory(readwrite)
declare void @runtimeFunc0(ptr) local_unnamed_addr #1

; Function Attrs: memory(readwrite)
declare void @runtimeFunc2(ptr) local_unnamed_addr #2

; Function Attrs: nounwind memory(readwrite)
declare void @runtimeFunc1(ptr) local_unnamed_addr #1

declare i32 @__gxx_personality_v0(...)

define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #0 personality ptr @__gxx_personality_v0 {
  %2 = alloca i8, align 1
  call void @llvm.lifetime.start.p0(ptr nonnull %2)
  call void @runtimeFunc0(ptr nonnull %2)
  invoke void @runtimeFunc2(ptr nonnull %2)
          to label %execute.exit unwind label %3

3:                                                ; preds = %1
  %4 = landingpad { ptr, i32 }
          cleanup
  call void @runtimeFunc1(ptr nonnull %2)
  resume { ptr, i32 } %4

execute.exit:                                     ; preds = %1
  call void @runtimeFunc1(ptr nonnull %2)
  call void @llvm.lifetime.end.p0(ptr nonnull %2)
  %5 = load ptr, ptr %0, align 8
  store i32 42, ptr %5, align 4
  ret void
}

define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #0 personality ptr @__gxx_personality_v0 {
  %2 = alloca i8, align 1
  call void @llvm.lifetime.start.p0(ptr nonnull %2)
  call void @runtimeFunc0(ptr nonnull %2)
  invoke void @runtimeFunc2(ptr nonnull %2)
          to label %_mlir_ciface_execute.exit unwind label %3

3:                                                ; preds = %1
  %4 = landingpad { ptr, i32 }
          cleanup
  call void @runtimeFunc1(ptr nonnull %2)
  resume { ptr, i32 } %4

_mlir_ciface_execute.exit:                        ; preds = %1
  call void @runtimeFunc1(ptr nonnull %2)
  call void @llvm.lifetime.end.p0(ptr nonnull %2)
  %5 = load ptr, ptr %0, align 8
  store i32 42, ptr %5, align 4
  ret void
}

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.start.p0(ptr) #3

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.end.p0(ptr) #3

attributes #1 = { nounwind memory(readwrite) }
attributes #2 = { memory(readwrite) }
attributes #3 = { nocallback nofree nosync nounwind willreturn memory(argmem: readwrite) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
