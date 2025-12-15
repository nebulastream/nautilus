; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nounwind willreturn memory(argmem: readwrite, inaccessiblemem: readwrite)
define ptr @execute(ptr returned writeonly %0, ptr readonly %1, i64 %2) local_unnamed_addr #0 {
  tail call void @llvm.memmove.p0.p0.i64(ptr %0, ptr %1, i64 %2, i1 true)
  ret ptr %0
}

; Function Attrs: mustprogress nofree norecurse nounwind willreturn memory(argmem: readwrite, inaccessiblemem: readwrite)
define ptr @_mlir_ciface_execute(ptr returned writeonly %0, ptr readonly %1, i64 %2) local_unnamed_addr #1 {
  tail call void @llvm.memmove.p0.p0.i64(ptr %0, ptr readonly %1, i64 %2, i1 true)
  ret ptr %0
}

; Function Attrs: mustprogress nocallback nofree nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memmove.p0.p0.i64(ptr writeonly, ptr readonly, i64, i1 immarg) #2

; Function Attrs: mustprogress nofree norecurse nounwind willreturn
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #3 {
  %2 = load ptr, ptr %0, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load ptr, ptr %5, align 8
  %7 = getelementptr i8, ptr %0, i64 16
  %8 = load ptr, ptr %7, align 8
  %9 = load i64, ptr %8, align 8
  tail call void @llvm.memmove.p0.p0.i64(ptr %3, ptr readonly %6, i64 %9, i1 true)
  %10 = getelementptr i8, ptr %0, i64 24
  %11 = load ptr, ptr %10, align 8
  store ptr %3, ptr %11, align 8
  ret void
}

; Function Attrs: mustprogress nofree norecurse nounwind willreturn
define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #3 {
  %2 = load ptr, ptr %0, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load ptr, ptr %5, align 8
  %7 = getelementptr i8, ptr %0, i64 16
  %8 = load ptr, ptr %7, align 8
  %9 = load i64, ptr %8, align 8
  tail call void @llvm.memmove.p0.p0.i64(ptr %3, ptr readonly %6, i64 %9, i1 true)
  %10 = getelementptr i8, ptr %0, i64 24
  %11 = load ptr, ptr %10, align 8
  store ptr %3, ptr %11, align 8
  ret void
}

attributes #0 = { mustprogress nofree norecurse nounwind willreturn memory(argmem: readwrite, inaccessiblemem: readwrite) }
attributes #1 = { mustprogress nofree norecurse nounwind willreturn memory(argmem: readwrite, inaccessiblemem: readwrite) }
attributes #2 = { mustprogress nocallback nofree nounwind willreturn memory(argmem: readwrite) }
attributes #3 = { mustprogress nofree norecurse nounwind willreturn }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
