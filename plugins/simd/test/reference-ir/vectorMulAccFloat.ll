; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:o-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "arm64-apple-darwin25.2.0"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @execute(ptr readonly %0, ptr readonly %1, ptr readonly %2, ptr readonly %3, ptr writeonly initializes((0, 16)) %4) local_unnamed_addr #0 {
  %6 = load <4 x float>, ptr %0, align 16
  %7 = load <4 x float>, ptr %1, align 16
  %8 = fmul <4 x float> %6, %7
  %9 = load <4 x float>, ptr %2, align 16
  %10 = load <4 x float>, ptr %3, align 16
  %11 = fmul <4 x float> %9, %10
  %12 = fadd <4 x float> %8, %11
  store <4 x float> %12, ptr %4, align 16
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @_mlir_ciface_execute(ptr readonly %0, ptr readonly %1, ptr readonly %2, ptr readonly %3, ptr writeonly initializes((0, 16)) %4) local_unnamed_addr #0 {
  %6 = load <4 x float>, ptr %0, align 16
  %7 = load <4 x float>, ptr %1, align 16
  %8 = fmul <4 x float> %6, %7
  %9 = load <4 x float>, ptr %2, align 16
  %10 = load <4 x float>, ptr %3, align 16
  %11 = fmul <4 x float> %9, %10
  %12 = fadd <4 x float> %8, %11
  store <4 x float> %12, ptr %4, align 16
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #1 {
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
  %16 = load <4 x float>, ptr %3, align 16
  %17 = load <4 x float>, ptr %6, align 16
  %18 = fmul <4 x float> %16, %17
  %19 = load <4 x float>, ptr %9, align 16
  %20 = load <4 x float>, ptr %12, align 16
  %21 = fmul <4 x float> %19, %20
  %22 = fadd <4 x float> %18, %21
  store <4 x float> %22, ptr %15, align 16
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #1 {
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
  %16 = load <4 x float>, ptr %3, align 16
  %17 = load <4 x float>, ptr %6, align 16
  %18 = fmul <4 x float> %16, %17
  %19 = load <4 x float>, ptr %9, align 16
  %20 = load <4 x float>, ptr %12, align 16
  %21 = fmul <4 x float> %19, %20
  %22 = fadd <4 x float> %18, %21
  store <4 x float> %22, ptr %15, align 16
  ret void
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite) "target-features" }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none) "target-features" }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
