; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: read)
define float @execute(ptr readonly %0, ptr readonly %1) local_unnamed_addr #0 {
  %3 = load <16 x float>, ptr %0, align 64
  %4 = load <16 x float>, ptr %1, align 64
  %5 = fmul <16 x float> %3, %4
  %6 = tail call reassoc float @llvm.vector.reduce.fadd.v16f32(float 0.000000e+00, <16 x float> %5)
  ret float %6
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: read)
define float @_mlir_ciface_execute(ptr readonly %0, ptr readonly %1) local_unnamed_addr #0 {
  %3 = load <16 x float>, ptr %0, align 64
  %4 = load <16 x float>, ptr %1, align 64
  %5 = fmul <16 x float> %3, %4
  %6 = tail call reassoc float @llvm.vector.reduce.fadd.v16f32(float 0.000000e+00, <16 x float> %5)
  ret float %6
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare float @llvm.vector.reduce.fadd.v16f32(float, <16 x float>) #1

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load ptr, ptr %5, align 8
  %7 = load <16 x float>, ptr %3, align 64
  %8 = load <16 x float>, ptr %6, align 64
  %9 = fmul <16 x float> %7, %8
  %10 = tail call reassoc float @llvm.vector.reduce.fadd.v16f32(float 0.000000e+00, <16 x float> %9)
  %11 = getelementptr i8, ptr %0, i64 16
  %12 = load ptr, ptr %11, align 8
  store float %10, ptr %12, align 4
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
  %8 = load <16 x float>, ptr %6, align 64
  %9 = fmul <16 x float> %7, %8
  %10 = tail call reassoc float @llvm.vector.reduce.fadd.v16f32(float 0.000000e+00, <16 x float> %9)
  %11 = getelementptr i8, ptr %0, i64 16
  %12 = load ptr, ptr %11, align 8
  store float %10, ptr %12, align 4
  ret void
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: read) }
attributes #1 = { mustprogress nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #2 = { mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
