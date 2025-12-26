; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i32 @recursiveHelper(i32 %0, i32 %1) local_unnamed_addr #0 {
  %3 = icmp sgt i32 %1, 0
  %4 = zext i1 %3 to i32
  %spec.select = add i32 %0, %4
  ret i32 %spec.select
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i32 @_mlir_ciface_recursiveHelper(i32 %0, i32 %1) local_unnamed_addr #0 {
  %3 = icmp sgt i32 %1, 0
  %4 = zext i1 %3 to i32
  %spec.select.i = add i32 %0, %4
  ret i32 %spec.select.i
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i32 @execute(i32 %0) local_unnamed_addr #1 {
  %2 = add i32 %0, 3
  ret i32 %2
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i32 @_mlir_ciface_execute(i32 %0) local_unnamed_addr #0 {
  %2 = add i32 %0, 3
  ret i32 %2
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir_recursiveHelper(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load i32, ptr %5, align 4
  %7 = icmp sgt i32 %6, 0
  %8 = zext i1 %7 to i32
  %spec.select.i = add i32 %3, %8
  %9 = getelementptr i8, ptr %0, i64 16
  %10 = load ptr, ptr %9, align 8
  store i32 %spec.select.i, ptr %10, align 4
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir__mlir_ciface_recursiveHelper(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load i32, ptr %5, align 4
  %7 = icmp sgt i32 %6, 0
  %8 = zext i1 %7 to i32
  %spec.select.i.i = add i32 %3, %8
  %9 = getelementptr i8, ptr %0, i64 16
  %10 = load ptr, ptr %9, align 8
  store i32 %spec.select.i.i, ptr %10, align 4
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = add i32 %3, 3
  %5 = getelementptr i8, ptr %0, i64 8
  %6 = load ptr, ptr %5, align 8
  store i32 %4, ptr %6, align 4
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = add i32 %3, 3
  %5 = getelementptr i8, ptr %0, i64 8
  %6 = load ptr, ptr %5, align 8
  store i32 %4, ptr %6, align 4
  ret void
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
attributes #2 = { mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
