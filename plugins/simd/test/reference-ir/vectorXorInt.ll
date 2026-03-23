; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:o-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "arm64-apple-darwin25.2.0"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @execute(ptr readonly %0, ptr readonly %1, ptr writeonly initializes((0, 16)) %2) local_unnamed_addr #0 {
  %4 = load <4 x i32>, ptr %0, align 16
  %5 = load <4 x i32>, ptr %1, align 16
  %6 = xor <4 x i32> %5, %4
  store <4 x i32> %6, ptr %2, align 16
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @_mlir_ciface_execute(ptr readonly %0, ptr readonly %1, ptr writeonly initializes((0, 16)) %2) local_unnamed_addr #0 {
  %4 = load <4 x i32>, ptr %0, align 16
  %5 = load <4 x i32>, ptr %1, align 16
  %6 = xor <4 x i32> %5, %4
  store <4 x i32> %6, ptr %2, align 16
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
  %10 = load <4 x i32>, ptr %3, align 16
  %11 = load <4 x i32>, ptr %6, align 16
  %12 = xor <4 x i32> %11, %10
  store <4 x i32> %12, ptr %9, align 16
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
  %10 = load <4 x i32>, ptr %3, align 16
  %11 = load <4 x i32>, ptr %6, align 16
  %12 = xor <4 x i32> %11, %10
  store <4 x i32> %12, ptr %9, align 16
  ret void
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite) "target-features" }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none) "target-features" }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
