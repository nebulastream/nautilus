; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i32 @execute(i32 %0) local_unnamed_addr #0 {
  %2 = icmp slt i32 %0, 0
  %spec.select = select i1 %2, i32 0, i32 10
  %3 = icmp sgt i32 %spec.select, %0
  %4 = add nuw nsw i32 %spec.select, 10
  %spec.select.1 = select i1 %3, i32 %spec.select, i32 %4
  %5 = icmp sgt i32 %spec.select.1, %0
  %6 = add nuw nsw i32 %spec.select.1, 10
  %spec.select.2 = select i1 %5, i32 %spec.select.1, i32 %6
  %7 = icmp sgt i32 %spec.select.2, %0
  %8 = add nuw nsw i32 %spec.select.2, 10
  %spec.select.3 = select i1 %7, i32 %spec.select.2, i32 %8
  %9 = icmp sgt i32 %spec.select.3, %0
  %10 = add nuw nsw i32 %spec.select.3, 10
  %spec.select.4 = select i1 %9, i32 %spec.select.3, i32 %10
  %11 = icmp sgt i32 %spec.select.4, %0
  %12 = add nuw nsw i32 %spec.select.4, 10
  %spec.select.5 = select i1 %11, i32 %spec.select.4, i32 %12
  %13 = icmp sgt i32 %spec.select.5, %0
  %14 = add nuw nsw i32 %spec.select.5, 10
  %spec.select.6 = select i1 %13, i32 %spec.select.5, i32 %14
  %15 = icmp sgt i32 %spec.select.6, %0
  %16 = add nuw nsw i32 %spec.select.6, 10
  %spec.select.7 = select i1 %15, i32 %spec.select.6, i32 %16
  %17 = icmp sgt i32 %spec.select.7, %0
  %18 = add nuw nsw i32 %spec.select.7, 10
  %spec.select.8 = select i1 %17, i32 %spec.select.7, i32 %18
  %19 = icmp sgt i32 %spec.select.8, %0
  %20 = add nuw nsw i32 %spec.select.8, 10
  %spec.select.9 = select i1 %19, i32 %spec.select.8, i32 %20
  ret i32 %spec.select.9
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i32 @_mlir_ciface_execute(i32 %0) local_unnamed_addr #1 {
  %2 = icmp slt i32 %0, 0
  %spec.select.i = select i1 %2, i32 0, i32 10
  %3 = icmp sgt i32 %spec.select.i, %0
  %4 = add nuw nsw i32 %spec.select.i, 10
  %spec.select.1.i = select i1 %3, i32 %spec.select.i, i32 %4
  %5 = icmp sgt i32 %spec.select.1.i, %0
  %6 = add nuw nsw i32 %spec.select.1.i, 10
  %spec.select.2.i = select i1 %5, i32 %spec.select.1.i, i32 %6
  %7 = icmp sgt i32 %spec.select.2.i, %0
  %8 = add nuw nsw i32 %spec.select.2.i, 10
  %spec.select.3.i = select i1 %7, i32 %spec.select.2.i, i32 %8
  %9 = icmp sgt i32 %spec.select.3.i, %0
  %10 = add nuw nsw i32 %spec.select.3.i, 10
  %spec.select.4.i = select i1 %9, i32 %spec.select.3.i, i32 %10
  %11 = icmp sgt i32 %spec.select.4.i, %0
  %12 = add nuw nsw i32 %spec.select.4.i, 10
  %spec.select.5.i = select i1 %11, i32 %spec.select.4.i, i32 %12
  %13 = icmp sgt i32 %spec.select.5.i, %0
  %14 = add nuw nsw i32 %spec.select.5.i, 10
  %spec.select.6.i = select i1 %13, i32 %spec.select.5.i, i32 %14
  %15 = icmp sgt i32 %spec.select.6.i, %0
  %16 = add nuw nsw i32 %spec.select.6.i, 10
  %spec.select.7.i = select i1 %15, i32 %spec.select.6.i, i32 %16
  %17 = icmp sgt i32 %spec.select.7.i, %0
  %18 = add nuw nsw i32 %spec.select.7.i, 10
  %spec.select.8.i = select i1 %17, i32 %spec.select.7.i, i32 %18
  %19 = icmp sgt i32 %spec.select.8.i, %0
  %20 = add nuw nsw i32 %spec.select.8.i, 10
  %spec.select.9.i = select i1 %19, i32 %spec.select.8.i, i32 %20
  ret i32 %spec.select.9.i
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = icmp slt i32 %3, 0
  %spec.select.i = select i1 %4, i32 0, i32 10
  %5 = icmp sgt i32 %spec.select.i, %3
  %6 = add nuw nsw i32 %spec.select.i, 10
  %spec.select.1.i = select i1 %5, i32 %spec.select.i, i32 %6
  %7 = icmp sgt i32 %spec.select.1.i, %3
  %8 = add nuw nsw i32 %spec.select.1.i, 10
  %spec.select.2.i = select i1 %7, i32 %spec.select.1.i, i32 %8
  %9 = icmp sgt i32 %spec.select.2.i, %3
  %10 = add nuw nsw i32 %spec.select.2.i, 10
  %spec.select.3.i = select i1 %9, i32 %spec.select.2.i, i32 %10
  %11 = icmp sgt i32 %spec.select.3.i, %3
  %12 = add nuw nsw i32 %spec.select.3.i, 10
  %spec.select.4.i = select i1 %11, i32 %spec.select.3.i, i32 %12
  %13 = icmp sgt i32 %spec.select.4.i, %3
  %14 = add nuw nsw i32 %spec.select.4.i, 10
  %spec.select.5.i = select i1 %13, i32 %spec.select.4.i, i32 %14
  %15 = icmp sgt i32 %spec.select.5.i, %3
  %16 = add nuw nsw i32 %spec.select.5.i, 10
  %spec.select.6.i = select i1 %15, i32 %spec.select.5.i, i32 %16
  %17 = icmp sgt i32 %spec.select.6.i, %3
  %18 = add nuw nsw i32 %spec.select.6.i, 10
  %spec.select.7.i = select i1 %17, i32 %spec.select.6.i, i32 %18
  %19 = icmp sgt i32 %spec.select.7.i, %3
  %20 = add nuw nsw i32 %spec.select.7.i, 10
  %spec.select.8.i = select i1 %19, i32 %spec.select.7.i, i32 %20
  %21 = icmp sgt i32 %spec.select.8.i, %3
  %22 = add nuw nsw i32 %spec.select.8.i, 10
  %spec.select.9.i = select i1 %21, i32 %spec.select.8.i, i32 %22
  %23 = getelementptr i8, ptr %0, i64 8
  %24 = load ptr, ptr %23, align 8
  store i32 %spec.select.9.i, ptr %24, align 4
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = icmp slt i32 %3, 0
  %spec.select.i.i = select i1 %4, i32 0, i32 10
  %5 = icmp sgt i32 %spec.select.i.i, %3
  %6 = add nuw nsw i32 %spec.select.i.i, 10
  %spec.select.1.i.i = select i1 %5, i32 %spec.select.i.i, i32 %6
  %7 = icmp sgt i32 %spec.select.1.i.i, %3
  %8 = add nuw nsw i32 %spec.select.1.i.i, 10
  %spec.select.2.i.i = select i1 %7, i32 %spec.select.1.i.i, i32 %8
  %9 = icmp sgt i32 %spec.select.2.i.i, %3
  %10 = add nuw nsw i32 %spec.select.2.i.i, 10
  %spec.select.3.i.i = select i1 %9, i32 %spec.select.2.i.i, i32 %10
  %11 = icmp sgt i32 %spec.select.3.i.i, %3
  %12 = add nuw nsw i32 %spec.select.3.i.i, 10
  %spec.select.4.i.i = select i1 %11, i32 %spec.select.3.i.i, i32 %12
  %13 = icmp sgt i32 %spec.select.4.i.i, %3
  %14 = add nuw nsw i32 %spec.select.4.i.i, 10
  %spec.select.5.i.i = select i1 %13, i32 %spec.select.4.i.i, i32 %14
  %15 = icmp sgt i32 %spec.select.5.i.i, %3
  %16 = add nuw nsw i32 %spec.select.5.i.i, 10
  %spec.select.6.i.i = select i1 %15, i32 %spec.select.5.i.i, i32 %16
  %17 = icmp sgt i32 %spec.select.6.i.i, %3
  %18 = add nuw nsw i32 %spec.select.6.i.i, 10
  %spec.select.7.i.i = select i1 %17, i32 %spec.select.6.i.i, i32 %18
  %19 = icmp sgt i32 %spec.select.7.i.i, %3
  %20 = add nuw nsw i32 %spec.select.7.i.i, 10
  %spec.select.8.i.i = select i1 %19, i32 %spec.select.7.i.i, i32 %20
  %21 = icmp sgt i32 %spec.select.8.i.i, %3
  %22 = add nuw nsw i32 %spec.select.8.i.i, 10
  %spec.select.9.i.i = select i1 %21, i32 %spec.select.8.i.i, i32 %22
  %23 = getelementptr i8, ptr %0, i64 8
  %24 = load ptr, ptr %23, align 8
  store i32 %spec.select.9.i.i, ptr %24, align 4
  ret void
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
attributes #2 = { mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
