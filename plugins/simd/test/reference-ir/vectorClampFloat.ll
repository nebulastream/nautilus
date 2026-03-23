; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:o-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "arm64-apple-darwin25.2.0"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @execute(ptr readonly %0, ptr readonly %1, ptr readonly %2, ptr writeonly initializes((0, 16)) %3) local_unnamed_addr #0 {
  %5 = load <4 x float>, ptr %0, align 16
  %6 = load <4 x float>, ptr %1, align 16
  %7 = load <4 x float>, ptr %2, align 16
  %8 = tail call <4 x float> @llvm.minnum.v4f32(<4 x float> %5, <4 x float> %7)
  %9 = tail call <4 x float> @llvm.maxnum.v4f32(<4 x float> %8, <4 x float> %6)
  store <4 x float> %9, ptr %3, align 16
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @_mlir_ciface_execute(ptr readonly %0, ptr readonly %1, ptr readonly %2, ptr writeonly initializes((0, 16)) %3) local_unnamed_addr #0 {
  %5 = load <4 x float>, ptr %0, align 16
  %6 = load <4 x float>, ptr %1, align 16
  %7 = load <4 x float>, ptr %2, align 16
  %8 = tail call <4 x float> @llvm.minnum.v4f32(<4 x float> %5, <4 x float> %7)
  %9 = tail call <4 x float> @llvm.maxnum.v4f32(<4 x float> %8, <4 x float> %6)
  store <4 x float> %9, ptr %3, align 16
  ret void
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare <4 x float> @llvm.minnum.v4f32(<4 x float>, <4 x float>) #1

; Function Attrs: mustprogress nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare <4 x float> @llvm.maxnum.v4f32(<4 x float>, <4 x float>) #1

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
  %13 = load <4 x float>, ptr %3, align 16
  %14 = load <4 x float>, ptr %6, align 16
  %15 = load <4 x float>, ptr %9, align 16
  %16 = tail call <4 x float> @llvm.minnum.v4f32(<4 x float> %13, <4 x float> %15)
  %17 = tail call <4 x float> @llvm.maxnum.v4f32(<4 x float> %16, <4 x float> %14)
  store <4 x float> %17, ptr %12, align 16
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
  %13 = load <4 x float>, ptr %3, align 16
  %14 = load <4 x float>, ptr %6, align 16
  %15 = load <4 x float>, ptr %9, align 16
  %16 = tail call <4 x float> @llvm.minnum.v4f32(<4 x float> %13, <4 x float> %15)
  %17 = tail call <4 x float> @llvm.maxnum.v4f32(<4 x float> %16, <4 x float> %14)
  store <4 x float> %17, ptr %12, align 16
  ret void
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite) "target-features" }
attributes #1 = { mustprogress nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #2 = { mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none) "target-features" }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
