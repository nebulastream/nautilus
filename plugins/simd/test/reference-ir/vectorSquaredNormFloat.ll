; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:o-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "arm64-apple-darwin25.2.0"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: read)
define float @execute(ptr readonly %0) local_unnamed_addr #0 {
  %2 = load <4 x float>, ptr %0, align 16
  %3 = fmul <4 x float> %2, %2
  %4 = tail call reassoc float @llvm.vector.reduce.fadd.v4f32(float 0.000000e+00, <4 x float> %3)
  ret float %4
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: read)
define float @_mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #0 {
  %2 = load <4 x float>, ptr %0, align 16
  %3 = fmul <4 x float> %2, %2
  %4 = tail call reassoc float @llvm.vector.reduce.fadd.v4f32(float 0.000000e+00, <4 x float> %3)
  ret float %4
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare float @llvm.vector.reduce.fadd.v4f32(float, <4 x float>) #1

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = load <4 x float>, ptr %3, align 16
  %5 = fmul <4 x float> %4, %4
  %6 = tail call reassoc float @llvm.vector.reduce.fadd.v4f32(float 0.000000e+00, <4 x float> %5)
  %7 = getelementptr i8, ptr %0, i64 8
  %8 = load ptr, ptr %7, align 8
  store float %6, ptr %8, align 4
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = load <4 x float>, ptr %3, align 16
  %5 = fmul <4 x float> %4, %4
  %6 = tail call reassoc float @llvm.vector.reduce.fadd.v4f32(float 0.000000e+00, <4 x float> %5)
  %7 = getelementptr i8, ptr %0, i64 8
  %8 = load ptr, ptr %7, align 8
  store float %6, ptr %8, align 4
  ret void
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: read) "target-features" }
attributes #1 = { mustprogress nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #2 = { mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none) "target-features" }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
