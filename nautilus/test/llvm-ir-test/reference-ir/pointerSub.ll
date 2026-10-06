; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: read)
define signext i32 @execute(ptr readonly %0, i32 %1) local_unnamed_addr #0 {
  %3 = sext i32 %1 to i64
  %.neg = mul nsw i64 %3, -4
  %4 = getelementptr i8, ptr %0, i64 %.neg
  %5 = load i32, ptr %4, align 4
  ret i32 %5
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: read)
define signext i32 @_mlir_ciface_execute(ptr readonly %0, i32 %1) local_unnamed_addr #0 {
  %3 = sext i32 %1 to i64
  %.neg.i = mul nsw i64 %3, -4
  %4 = getelementptr i8, ptr %0, i64 %.neg.i
  %5 = load i32, ptr %4, align 4
  ret i32 %5
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #1 {
  %2 = load ptr, ptr %0, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load i32, ptr %5, align 4
  %7 = sext i32 %6 to i64
  %.neg.i = mul nsw i64 %7, -4
  %8 = getelementptr i8, ptr %3, i64 %.neg.i
  %9 = load i32, ptr %8, align 4
  %10 = getelementptr i8, ptr %0, i64 16
  %11 = load ptr, ptr %10, align 8
  store i32 %9, ptr %11, align 4
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #1 {
  %2 = load ptr, ptr %0, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load i32, ptr %5, align 4
  %7 = sext i32 %6 to i64
  %.neg.i.i = mul nsw i64 %7, -4
  %8 = getelementptr i8, ptr %3, i64 %.neg.i.i
  %9 = load i32, ptr %8, align 4
  %10 = getelementptr i8, ptr %0, i64 16
  %11 = load ptr, ptr %10, align 8
  store i32 %9, ptr %11, align 4
  ret void
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: read) }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
