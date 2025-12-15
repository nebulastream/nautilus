; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nounwind willreturn memory(argmem: write, inaccessiblemem: readwrite)
define ptr @execute(ptr returned writeonly %0, i32 %1, i64 %2) local_unnamed_addr #0 {
  %4 = trunc i32 %1 to i8
  tail call void @llvm.memset.p0.i64(ptr %0, i8 %4, i64 %2, i1 true)
  ret ptr %0
}

; Function Attrs: mustprogress nofree norecurse nounwind willreturn memory(argmem: write, inaccessiblemem: readwrite)
define ptr @_mlir_ciface_execute(ptr returned writeonly %0, i32 %1, i64 %2) local_unnamed_addr #1 {
  %4 = trunc i32 %1 to i8
  tail call void @llvm.memset.p0.i64(ptr %0, i8 %4, i64 %2, i1 true)
  ret ptr %0
}

; Function Attrs: mustprogress nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr writeonly, i8, i64, i1 immarg) #2

; Function Attrs: mustprogress nofree norecurse nounwind willreturn
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #3 {
  %2 = load ptr, ptr %0, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load i32, ptr %5, align 4
  %7 = getelementptr i8, ptr %0, i64 16
  %8 = load ptr, ptr %7, align 8
  %9 = load i64, ptr %8, align 8
  %10 = trunc i32 %6 to i8
  tail call void @llvm.memset.p0.i64(ptr %3, i8 %10, i64 %9, i1 true)
  %11 = getelementptr i8, ptr %0, i64 24
  %12 = load ptr, ptr %11, align 8
  store ptr %3, ptr %12, align 8
  ret void
}

; Function Attrs: mustprogress nofree norecurse nounwind willreturn
define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #3 {
  %2 = load ptr, ptr %0, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load i32, ptr %5, align 4
  %7 = getelementptr i8, ptr %0, i64 16
  %8 = load ptr, ptr %7, align 8
  %9 = load i64, ptr %8, align 8
  %10 = trunc i32 %6 to i8
  tail call void @llvm.memset.p0.i64(ptr %3, i8 %10, i64 %9, i1 true)
  %11 = getelementptr i8, ptr %0, i64 24
  %12 = load ptr, ptr %11, align 8
  store ptr %3, ptr %12, align 8
  ret void
}

attributes #0 = { mustprogress nofree norecurse nounwind willreturn memory(argmem: write, inaccessiblemem: readwrite) }
attributes #1 = { mustprogress nofree norecurse nounwind willreturn memory(argmem: write, inaccessiblemem: readwrite) }
attributes #2 = { mustprogress nocallback nofree nounwind willreturn memory(argmem: write) }
attributes #3 = { mustprogress nofree norecurse nounwind willreturn }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
