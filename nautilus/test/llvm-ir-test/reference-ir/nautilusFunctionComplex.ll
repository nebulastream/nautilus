; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i32 @complexComputation(i32 %0, i32 %1, i32 %2) local_unnamed_addr #0 {
  %4 = mul i32 %1, %0
  %5 = add i32 %2, %1
  %6 = sub i32 %4, %5
  %7 = shl i32 %6, 1
  ret i32 %7
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i32 @_mlir_ciface_complexComputation(i32 %0, i32 %1, i32 %2) local_unnamed_addr #0 {
  %4 = mul i32 %1, %0
  %5 = add i32 %2, %1
  %6 = sub i32 %4, %5
  %7 = shl i32 %6, 1
  ret i32 %7
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i32 @execute(i32 %0, i32 %1) local_unnamed_addr #1 {
  %3 = add i32 %0, 2147483647
  %reass.sub = mul i32 %1, %3
  %4 = add i32 %reass.sub, 2147483643
  %5 = shl i32 %0, 1
  %6 = mul i32 %5, %4
  %7 = add i32 %1, %0
  %8 = sub i32 %6, %7
  %9 = shl i32 %8, 1
  ret i32 %9
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i32 @_mlir_ciface_execute(i32 %0, i32 %1) local_unnamed_addr #0 {
  %3 = add i32 %0, 2147483647
  %reass.sub.i = mul i32 %1, %3
  %4 = add i32 %reass.sub.i, 2147483643
  %5 = shl i32 %0, 1
  %6 = mul i32 %5, %4
  %7 = add i32 %1, %0
  %8 = sub i32 %6, %7
  %9 = shl i32 %8, 1
  ret i32 %9
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir_complexComputation(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load i32, ptr %5, align 4
  %7 = getelementptr i8, ptr %0, i64 16
  %8 = load ptr, ptr %7, align 8
  %9 = load i32, ptr %8, align 4
  %10 = add i32 %3, 2147483647
  %.neg2 = mul i32 %6, %10
  %11 = sub i32 %.neg2, %9
  %12 = shl i32 %11, 1
  %13 = getelementptr i8, ptr %0, i64 24
  %14 = load ptr, ptr %13, align 8
  store i32 %12, ptr %14, align 4
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir__mlir_ciface_complexComputation(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load i32, ptr %5, align 4
  %7 = getelementptr i8, ptr %0, i64 16
  %8 = load ptr, ptr %7, align 8
  %9 = load i32, ptr %8, align 4
  %10 = add i32 %3, 2147483647
  %.neg2 = mul i32 %6, %10
  %11 = sub i32 %.neg2, %9
  %12 = shl i32 %11, 1
  %13 = getelementptr i8, ptr %0, i64 24
  %14 = load ptr, ptr %13, align 8
  store i32 %12, ptr %14, align 4
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load i32, ptr %5, align 4
  %7 = add i32 %3, 2147483647
  %reass.sub.i = mul i32 %6, %7
  %8 = add i32 %reass.sub.i, 2147483643
  %9 = shl i32 %3, 1
  %10 = mul i32 %9, %8
  %11 = add i32 %6, %3
  %12 = sub i32 %10, %11
  %13 = shl i32 %12, 1
  %14 = getelementptr i8, ptr %0, i64 16
  %15 = load ptr, ptr %14, align 8
  store i32 %13, ptr %15, align 4
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load i32, ptr %5, align 4
  %7 = add i32 %3, 2147483647
  %reass.sub.i.i = mul i32 %6, %7
  %8 = add i32 %reass.sub.i.i, 2147483643
  %9 = shl i32 %3, 1
  %10 = mul i32 %9, %8
  %11 = add i32 %6, %3
  %12 = sub i32 %10, %11
  %13 = shl i32 %12, 1
  %14 = getelementptr i8, ptr %0, i64 16
  %15 = load ptr, ptr %14, align 8
  store i32 %13, ptr %15, align 4
  ret void
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
attributes #2 = { mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
