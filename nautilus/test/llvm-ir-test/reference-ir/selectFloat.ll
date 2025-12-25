; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define float @execute(i1 %0, float %1, float %2) local_unnamed_addr #0 {
  %4 = select i1 %0, float %1, float %2
  ret float %4
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define float @_mlir_ciface_execute(i1 %0, float %1, float %2) local_unnamed_addr #1 {
  %4 = select i1 %0, float %1, float %2
  ret float %4
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i1, ptr %2, align 1
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load float, ptr %5, align 4
  %7 = getelementptr i8, ptr %0, i64 16
  %8 = load ptr, ptr %7, align 8
  %9 = load float, ptr %8, align 4
  %10 = select i1 %3, float %6, float %9
  %11 = getelementptr i8, ptr %0, i64 24
  %12 = load ptr, ptr %11, align 8
  store float %10, ptr %12, align 4
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i1, ptr %2, align 1
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load float, ptr %5, align 4
  %7 = getelementptr i8, ptr %0, i64 16
  %8 = load ptr, ptr %7, align 8
  %9 = load float, ptr %8, align 4
  %10 = select i1 %3, float %6, float %9
  %11 = getelementptr i8, ptr %0, i64 24
  %12 = load ptr, ptr %11, align 8
  store float %10, ptr %12, align 4
  ret void
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
attributes #2 = { mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
