; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @execute(ptr readonly %0, ptr readonly %1, ptr readonly %2, ptr readonly %3, ptr writeonly %4) local_unnamed_addr #0 {
  %6 = load <16 x float>, ptr %0, align 64
  %7 = load <16 x float>, ptr %1, align 64
  %8 = fmul <16 x float> %6, %7
  %9 = load <16 x float>, ptr %2, align 64
  %10 = load <16 x float>, ptr %3, align 64
  %11 = fmul <16 x float> %9, %10
  %12 = fadd <16 x float> %8, %11
  store <16 x float> %12, ptr %4, align 64
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @_mlir_ciface_execute(ptr readonly %0, ptr readonly %1, ptr readonly %2, ptr readonly %3, ptr writeonly %4) local_unnamed_addr #0 {
  %6 = load <16 x float>, ptr %0, align 64
  %7 = load <16 x float>, ptr %1, align 64
  %8 = fmul <16 x float> %6, %7
  %9 = load <16 x float>, ptr %2, align 64
  %10 = load <16 x float>, ptr %3, align 64
  %11 = fmul <16 x float> %9, %10
  %12 = fadd <16 x float> %8, %11
  store <16 x float> %12, ptr %4, align 64
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
  %16 = load <16 x float>, ptr %3, align 64
  %17 = load <16 x float>, ptr %6, align 64
  %18 = fmul <16 x float> %16, %17
  %19 = load <16 x float>, ptr %9, align 64
  %20 = load <16 x float>, ptr %12, align 64
  %21 = fmul <16 x float> %19, %20
  %22 = fadd <16 x float> %18, %21
  store <16 x float> %22, ptr %15, align 64
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
  %16 = load <16 x float>, ptr %3, align 64
  %17 = load <16 x float>, ptr %6, align 64
  %18 = fmul <16 x float> %16, %17
  %19 = load <16 x float>, ptr %9, align 64
  %20 = load <16 x float>, ptr %12, align 64
  %21 = fmul <16 x float> %19, %20
  %22 = fadd <16 x float> %18, %21
  store <16 x float> %22, ptr %15, align 64
  ret void
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite) }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
