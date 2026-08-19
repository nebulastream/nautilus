; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nounwind
define noundef signext i32 @execute() local_unnamed_addr #0 {
  %1 = alloca i8, align 1
  call void @runtimeFunc0(ptr nonnull %1)
  call void @runtimeFunc2(ptr nonnull %1, i32 3)
  call void @runtimeFunc1(ptr nonnull %1)
  ret i32 42
}

; Function Attrs: nounwind
define noundef signext i32 @_mlir_ciface_execute() local_unnamed_addr #0 {
  %1 = alloca i8, align 1
  call void @llvm.lifetime.start.p0(ptr nonnull %1)
  call void @runtimeFunc0(ptr nonnull %1)
  call void @runtimeFunc2(ptr nonnull %1, i32 3)
  call void @runtimeFunc1(ptr nonnull %1)
  call void @llvm.lifetime.end.p0(ptr nonnull %1)
  ret i32 42
}

; Function Attrs: nounwind memory(readwrite)
declare void @runtimeFunc0(ptr) local_unnamed_addr #1

; Function Attrs: nounwind memory(readwrite)
declare void @runtimeFunc2(ptr, i32) local_unnamed_addr #1

; Function Attrs: nounwind memory(readwrite)
declare void @runtimeFunc1(ptr) local_unnamed_addr #1

; Function Attrs: nounwind
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #0 {
  %2 = alloca i8, align 1
  call void @llvm.lifetime.start.p0(ptr nonnull %2)
  call void @runtimeFunc0(ptr nonnull %2)
  call void @runtimeFunc2(ptr nonnull %2, i32 3)
  call void @runtimeFunc1(ptr nonnull %2)
  call void @llvm.lifetime.end.p0(ptr nonnull %2)
  %3 = load ptr, ptr %0, align 8
  store i32 42, ptr %3, align 4
  ret void
}

; Function Attrs: nounwind
define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #0 {
  %2 = alloca i8, align 1
  call void @llvm.lifetime.start.p0(ptr nonnull %2)
  call void @runtimeFunc0(ptr nonnull %2)
  call void @runtimeFunc2(ptr nonnull %2, i32 3)
  call void @runtimeFunc1(ptr nonnull %2)
  call void @llvm.lifetime.end.p0(ptr nonnull %2)
  %3 = load ptr, ptr %0, align 8
  store i32 42, ptr %3, align 4
  ret void
}

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.start.p0(ptr) #2

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.end.p0(ptr) #2

attributes #0 = { nounwind }
attributes #1 = { nounwind memory(readwrite) }
attributes #2 = { nocallback nofree nosync nounwind willreturn memory(argmem: readwrite) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
