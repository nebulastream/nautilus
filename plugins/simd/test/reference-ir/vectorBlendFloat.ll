; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @execute(ptr readonly %0, ptr readonly %1, ptr readonly %2, ptr writeonly initializes((0, 64)) %3) local_unnamed_addr #0 {
  %5 = load <16 x float>, ptr %0, align 64
  %6 = load <16 x float>, ptr %1, align 64
  %7 = load <16 x i32>, ptr %2, align 64
  %.not = icmp eq <16 x i32> %7, zeroinitializer
  %8 = select <16 x i1> %.not, <16 x float> %6, <16 x float> %5
  store <16 x float> %8, ptr %3, align 64
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @_mlir_ciface_execute(ptr readonly %0, ptr readonly %1, ptr readonly %2, ptr writeonly initializes((0, 64)) %3) local_unnamed_addr #0 {
  %5 = load <16 x float>, ptr %0, align 64
  %6 = load <16 x float>, ptr %1, align 64
  %7 = load <16 x i32>, ptr %2, align 64
  %.not.i = icmp eq <16 x i32> %7, zeroinitializer
  %8 = select <16 x i1> %.not.i, <16 x float> %6, <16 x float> %5
  store <16 x float> %8, ptr %3, align 64
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
  %13 = load <16 x float>, ptr %3, align 64
  %14 = load <16 x float>, ptr %6, align 64
  %15 = load <16 x i32>, ptr %9, align 64
  %.not.i = icmp eq <16 x i32> %15, zeroinitializer
  %16 = select <16 x i1> %.not.i, <16 x float> %14, <16 x float> %13
  store <16 x float> %16, ptr %12, align 64
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
  %13 = load <16 x float>, ptr %3, align 64
  %14 = load <16 x float>, ptr %6, align 64
  %15 = load <16 x i32>, ptr %9, align 64
  %.not.i.i = icmp eq <16 x i32> %15, zeroinitializer
  %16 = select <16 x i1> %.not.i.i, <16 x float> %14, <16 x float> %13
  store <16 x float> %16, ptr %12, align 64
  ret void
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite) }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
