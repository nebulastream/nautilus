; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @execute(ptr readonly %0, ptr writeonly %1) local_unnamed_addr #0 {
  %3 = load <16 x float>, ptr %0, align 64
  %4 = tail call <16 x float> @llvm.fabs.v16f32(<16 x float> %3)
  store <16 x float> %4, ptr %1, align 64
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @_mlir_ciface_execute(ptr readonly %0, ptr writeonly %1) local_unnamed_addr #0 {
  %3 = load <16 x float>, ptr %0, align 64
  %4 = tail call <16 x float> @llvm.fabs.v16f32(<16 x float> %3)
  store <16 x float> %4, ptr %1, align 64
  ret void
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare <16 x float> @llvm.fabs.v16f32(<16 x float>) #1

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load ptr, ptr %5, align 8
  %7 = load <16 x float>, ptr %3, align 64
  %8 = tail call <16 x float> @llvm.fabs.v16f32(<16 x float> %7)
  store <16 x float> %8, ptr %6, align 64
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load ptr, ptr %5, align 8
  %7 = load <16 x float>, ptr %3, align 64
  %8 = tail call <16 x float> @llvm.fabs.v16f32(<16 x float> %7)
  store <16 x float> %8, ptr %6, align 64
  ret void
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite) }
attributes #1 = { mustprogress nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #2 = { mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
