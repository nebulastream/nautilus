; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i32 @execute(i32 %0, i32 %1) local_unnamed_addr #0 {
  %3 = icmp eq i32 %0, 42
  %4 = icmp eq i32 %1, 8
  %5 = add i32 %1, 2
  %spec.select = select i1 %4, i32 8, i32 %5
  %6 = select i1 %3, i32 %1, i32 %spec.select
  %7 = add i32 %6, 2
  ret i32 %7
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i32 @_mlir_ciface_execute(i32 %0, i32 %1) local_unnamed_addr #1 {
  %3 = icmp eq i32 %0, 42
  %4 = icmp eq i32 %1, 8
  %5 = add i32 %1, 2
  %spec.select.i = select i1 %4, i32 8, i32 %5
  %6 = select i1 %3, i32 %1, i32 %spec.select.i
  %7 = add i32 %6, 2
  ret i32 %7
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load i32, ptr %5, align 4
  %7 = icmp eq i32 %3, 42
  %8 = icmp eq i32 %6, 8
  %9 = add i32 %6, 2
  %spec.select.i = select i1 %8, i32 8, i32 %9
  %10 = select i1 %7, i32 %6, i32 %spec.select.i
  %11 = add i32 %10, 2
  %12 = getelementptr i8, ptr %0, i64 16
  %13 = load ptr, ptr %12, align 8
  store i32 %11, ptr %13, align 4
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load i32, ptr %5, align 4
  %7 = icmp eq i32 %3, 42
  %8 = icmp eq i32 %6, 8
  %9 = add i32 %6, 2
  %spec.select.i.i = select i1 %8, i32 8, i32 %9
  %10 = select i1 %7, i32 %6, i32 %spec.select.i.i
  %11 = add i32 %10, 2
  %12 = getelementptr i8, ptr %0, i64 16
  %13 = load ptr, ptr %12, align 8
  store i32 %11, ptr %13, align 4
  ret void
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
attributes #2 = { mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
