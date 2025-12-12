; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext range(i32 0, 6) i32 @execute(i32 %0) local_unnamed_addr #0 {
  %2 = icmp sgt i32 %0, 1
  %spec.select = zext i1 %2 to i32
  %3 = icmp sgt i32 %0, 2
  %4 = select i1 %3, i32 2, i32 %spec.select
  %5 = icmp sgt i32 %0, 3
  %6 = zext i1 %5 to i32
  %7 = icmp sgt i32 %0, 4
  %8 = zext i1 %7 to i32
  %9 = icmp sgt i32 %0, 5
  %10 = zext i1 %9 to i32
  %11 = add nuw nsw i32 %8, %6
  %12 = add nuw nsw i32 %11, %10
  %13 = add nuw nsw i32 %12, %4
  ret i32 %13
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext range(i32 0, 6) i32 @_mlir_ciface_execute(i32 %0) local_unnamed_addr #1 {
  %2 = icmp sgt i32 %0, 1
  %spec.select.i = zext i1 %2 to i32
  %3 = icmp sgt i32 %0, 2
  %4 = select i1 %3, i32 2, i32 %spec.select.i
  %5 = icmp sgt i32 %0, 3
  %6 = zext i1 %5 to i32
  %7 = icmp sgt i32 %0, 4
  %8 = zext i1 %7 to i32
  %9 = icmp sgt i32 %0, 5
  %10 = zext i1 %9 to i32
  %11 = add nuw nsw i32 %8, %6
  %12 = add nuw nsw i32 %11, %10
  %13 = add nuw nsw i32 %12, %4
  ret i32 %13
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = icmp sgt i32 %3, 1
  %spec.select.i = zext i1 %4 to i32
  %5 = icmp sgt i32 %3, 2
  %6 = select i1 %5, i32 2, i32 %spec.select.i
  %7 = icmp sgt i32 %3, 3
  %8 = zext i1 %7 to i32
  %9 = icmp sgt i32 %3, 4
  %10 = zext i1 %9 to i32
  %11 = icmp sgt i32 %3, 5
  %12 = zext i1 %11 to i32
  %13 = add nuw nsw i32 %10, %8
  %14 = add nuw nsw i32 %13, %12
  %15 = add nuw nsw i32 %14, %6
  %16 = getelementptr i8, ptr %0, i64 8
  %17 = load ptr, ptr %16, align 8
  store i32 %15, ptr %17, align 4
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = icmp sgt i32 %3, 1
  %spec.select.i.i = zext i1 %4 to i32
  %5 = icmp sgt i32 %3, 2
  %6 = select i1 %5, i32 2, i32 %spec.select.i.i
  %7 = icmp sgt i32 %3, 3
  %8 = zext i1 %7 to i32
  %9 = icmp sgt i32 %3, 4
  %10 = zext i1 %9 to i32
  %11 = icmp sgt i32 %3, 5
  %12 = zext i1 %11 to i32
  %13 = add nuw nsw i32 %10, %8
  %14 = add nuw nsw i32 %13, %12
  %15 = add nuw nsw i32 %14, %6
  %16 = getelementptr i8, ptr %0, i64 8
  %17 = load ptr, ptr %16, align 8
  store i32 %15, ptr %17, align 4
  ret void
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
attributes #2 = { mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
