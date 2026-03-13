; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i32 @subtract5(i32 %0) local_unnamed_addr #0 {
  %2 = add i32 %0, -5
  ret i32 %2
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i32 @_mlir_ciface_subtract5(i32 %0) local_unnamed_addr #0 {
  %2 = add i32 %0, -5
  ret i32 %2
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i32 @multiply2(i32 %0) local_unnamed_addr #0 {
  %2 = shl i32 %0, 1
  ret i32 %2
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i32 @_mlir_ciface_multiply2(i32 %0) local_unnamed_addr #0 {
  %2 = shl i32 %0, 1
  ret i32 %2
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i32 @add10(i32 %0) local_unnamed_addr #0 {
  %2 = add i32 %0, 10
  ret i32 %2
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i32 @_mlir_ciface_add10(i32 %0) local_unnamed_addr #0 {
  %2 = add i32 %0, 10
  ret i32 %2
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i32 @execute(i32 %0) local_unnamed_addr #1 {
  %2 = shl i32 %0, 1
  %3 = add i32 %2, 15
  ret i32 %3
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i32 @_mlir_ciface_execute(i32 %0) local_unnamed_addr #0 {
  %2 = shl i32 %0, 1
  %3 = add i32 %2, 15
  ret i32 %3
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir_subtract5(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = add i32 %3, -5
  %5 = getelementptr i8, ptr %0, i64 8
  %6 = load ptr, ptr %5, align 8
  store i32 %4, ptr %6, align 4
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir__mlir_ciface_subtract5(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = add i32 %3, -5
  %5 = getelementptr i8, ptr %0, i64 8
  %6 = load ptr, ptr %5, align 8
  store i32 %4, ptr %6, align 4
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir_multiply2(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = shl i32 %3, 1
  %5 = getelementptr i8, ptr %0, i64 8
  %6 = load ptr, ptr %5, align 8
  store i32 %4, ptr %6, align 4
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir__mlir_ciface_multiply2(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = shl i32 %3, 1
  %5 = getelementptr i8, ptr %0, i64 8
  %6 = load ptr, ptr %5, align 8
  store i32 %4, ptr %6, align 4
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir_add10(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = add i32 %3, 10
  %5 = getelementptr i8, ptr %0, i64 8
  %6 = load ptr, ptr %5, align 8
  store i32 %4, ptr %6, align 4
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir__mlir_ciface_add10(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = add i32 %3, 10
  %5 = getelementptr i8, ptr %0, i64 8
  %6 = load ptr, ptr %5, align 8
  store i32 %4, ptr %6, align 4
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = shl i32 %3, 1
  %5 = add i32 %4, 15
  %6 = getelementptr i8, ptr %0, i64 8
  %7 = load ptr, ptr %6, align 8
  store i32 %5, ptr %7, align 4
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = shl i32 %3, 1
  %5 = add i32 %4, 15
  %6 = getelementptr i8, ptr %0, i64 8
  %7 = load ptr, ptr %6, align 8
  store i32 %5, ptr %7, align 4
  ret void
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
attributes #2 = { mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
