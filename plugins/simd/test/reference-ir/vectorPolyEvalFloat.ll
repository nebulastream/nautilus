; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:o-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "arm64-apple-darwin25.2.0"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @execute(ptr readonly %0, ptr readonly %1, ptr readonly %2, ptr readonly %3, ptr readonly %4, ptr writeonly initializes((0, 16)) %5) local_unnamed_addr #0 {
  %7 = load <4 x float>, ptr %0, align 16
  %8 = load <4 x float>, ptr %1, align 16
  %9 = load <4 x float>, ptr %2, align 16
  %10 = load <4 x float>, ptr %3, align 16
  %11 = load <4 x float>, ptr %4, align 16
  %12 = tail call <4 x float> @llvm.fma.v4f32(<4 x float> %11, <4 x float> %7, <4 x float> %10)
  %13 = tail call <4 x float> @llvm.fma.v4f32(<4 x float> %12, <4 x float> %7, <4 x float> %9)
  %14 = tail call <4 x float> @llvm.fma.v4f32(<4 x float> %13, <4 x float> %7, <4 x float> %8)
  store <4 x float> %14, ptr %5, align 16
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @_mlir_ciface_execute(ptr readonly %0, ptr readonly %1, ptr readonly %2, ptr readonly %3, ptr readonly %4, ptr writeonly initializes((0, 16)) %5) local_unnamed_addr #0 {
  %7 = load <4 x float>, ptr %0, align 16
  %8 = load <4 x float>, ptr %1, align 16
  %9 = load <4 x float>, ptr %2, align 16
  %10 = load <4 x float>, ptr %3, align 16
  %11 = load <4 x float>, ptr %4, align 16
  %12 = tail call <4 x float> @llvm.fma.v4f32(<4 x float> %11, <4 x float> %7, <4 x float> %10)
  %13 = tail call <4 x float> @llvm.fma.v4f32(<4 x float> %12, <4 x float> %7, <4 x float> %9)
  %14 = tail call <4 x float> @llvm.fma.v4f32(<4 x float> %13, <4 x float> %7, <4 x float> %8)
  store <4 x float> %14, ptr %5, align 16
  ret void
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare <4 x float> @llvm.fma.v4f32(<4 x float>, <4 x float>, <4 x float>) #1

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load ptr, ptr %5, align 8
  %7 = getelementptr i8, ptr %0, i64 16
  %8 = load ptr, ptr %7, align 8
  %9 = load ptr, ptr %8, align 8
  %10 = getelementptr i8, ptr %0, i64 24
  %11 = load ptr, ptr %10, align 8
  %12 = load ptr, ptr %11, align 8
  %13 = getelementptr i8, ptr %0, i64 32
  %14 = load ptr, ptr %13, align 8
  %15 = load ptr, ptr %14, align 8
  %16 = getelementptr i8, ptr %0, i64 40
  %17 = load ptr, ptr %16, align 8
  %18 = load ptr, ptr %17, align 8
  %19 = load <4 x float>, ptr %3, align 16
  %20 = load <4 x float>, ptr %6, align 16
  %21 = load <4 x float>, ptr %9, align 16
  %22 = load <4 x float>, ptr %12, align 16
  %23 = load <4 x float>, ptr %15, align 16
  %24 = tail call <4 x float> @llvm.fma.v4f32(<4 x float> %23, <4 x float> %19, <4 x float> %22)
  %25 = tail call <4 x float> @llvm.fma.v4f32(<4 x float> %24, <4 x float> %19, <4 x float> %21)
  %26 = tail call <4 x float> @llvm.fma.v4f32(<4 x float> %25, <4 x float> %19, <4 x float> %20)
  store <4 x float> %26, ptr %18, align 16
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load ptr, ptr %5, align 8
  %7 = getelementptr i8, ptr %0, i64 16
  %8 = load ptr, ptr %7, align 8
  %9 = load ptr, ptr %8, align 8
  %10 = getelementptr i8, ptr %0, i64 24
  %11 = load ptr, ptr %10, align 8
  %12 = load ptr, ptr %11, align 8
  %13 = getelementptr i8, ptr %0, i64 32
  %14 = load ptr, ptr %13, align 8
  %15 = load ptr, ptr %14, align 8
  %16 = getelementptr i8, ptr %0, i64 40
  %17 = load ptr, ptr %16, align 8
  %18 = load ptr, ptr %17, align 8
  %19 = load <4 x float>, ptr %3, align 16
  %20 = load <4 x float>, ptr %6, align 16
  %21 = load <4 x float>, ptr %9, align 16
  %22 = load <4 x float>, ptr %12, align 16
  %23 = load <4 x float>, ptr %15, align 16
  %24 = tail call <4 x float> @llvm.fma.v4f32(<4 x float> %23, <4 x float> %19, <4 x float> %22)
  %25 = tail call <4 x float> @llvm.fma.v4f32(<4 x float> %24, <4 x float> %19, <4 x float> %21)
  %26 = tail call <4 x float> @llvm.fma.v4f32(<4 x float> %25, <4 x float> %19, <4 x float> %20)
  store <4 x float> %26, ptr %18, align 16
  ret void
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite) "target-features" }
attributes #1 = { mustprogress nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #2 = { mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none) "target-features" }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
