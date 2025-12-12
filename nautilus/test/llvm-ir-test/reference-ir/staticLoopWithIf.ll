; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext range(i32 116, 115) i32 @execute(i32 %0) local_unnamed_addr #0 {
  %2 = icmp sgt i32 %0, 5
  %3 = zext i1 %2 to i32
  %spec.select = add nuw i32 %0, %3
  %4 = add i32 %spec.select, 10
  %5 = icmp sgt i32 %4, 5
  %6 = add i32 %spec.select, 11
  %7 = select i1 %5, i32 %6, i32 %4
  %8 = add i32 %7, 10
  %9 = icmp sgt i32 %8, 5
  %10 = add i32 %7, 11
  %11 = select i1 %9, i32 %10, i32 %8
  %12 = add i32 %11, 10
  %13 = icmp sgt i32 %12, 5
  %14 = add i32 %11, 11
  %15 = select i1 %13, i32 %14, i32 %12
  %16 = add i32 %15, 10
  %17 = icmp sgt i32 %16, 5
  %18 = add i32 %15, 11
  %19 = select i1 %17, i32 %18, i32 %16
  %20 = add i32 %19, 10
  %21 = icmp sgt i32 %20, 5
  %22 = add i32 %19, 11
  %23 = select i1 %21, i32 %22, i32 %20
  %24 = add i32 %23, 10
  %25 = icmp sgt i32 %24, 5
  %26 = add i32 %23, 11
  %27 = select i1 %25, i32 %26, i32 %24
  %28 = add i32 %27, 10
  %29 = icmp sgt i32 %28, 5
  %30 = add i32 %27, 11
  %31 = select i1 %29, i32 %30, i32 %28
  %32 = add i32 %31, 10
  %33 = icmp sgt i32 %32, 5
  %34 = add i32 %31, 11
  %35 = select i1 %33, i32 %34, i32 %32
  %36 = add i32 %35, 10
  %37 = icmp sgt i32 %36, 5
  %38 = add i32 %35, 11
  %39 = select i1 %37, i32 %38, i32 %36
  %40 = add i32 %39, 10
  ret i32 %40
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext range(i32 116, 115) i32 @_mlir_ciface_execute(i32 %0) local_unnamed_addr #1 {
  %2 = icmp sgt i32 %0, 5
  %3 = zext i1 %2 to i32
  %spec.select.i = add nuw i32 %0, %3
  %4 = add i32 %spec.select.i, 10
  %5 = icmp sgt i32 %4, 5
  %6 = add i32 %spec.select.i, 11
  %7 = select i1 %5, i32 %6, i32 %4
  %8 = add i32 %7, 10
  %9 = icmp sgt i32 %8, 5
  %10 = add i32 %7, 11
  %11 = select i1 %9, i32 %10, i32 %8
  %12 = add i32 %11, 10
  %13 = icmp sgt i32 %12, 5
  %14 = add i32 %11, 11
  %15 = select i1 %13, i32 %14, i32 %12
  %16 = add i32 %15, 10
  %17 = icmp sgt i32 %16, 5
  %18 = add i32 %15, 11
  %19 = select i1 %17, i32 %18, i32 %16
  %20 = add i32 %19, 10
  %21 = icmp sgt i32 %20, 5
  %22 = add i32 %19, 11
  %23 = select i1 %21, i32 %22, i32 %20
  %24 = add i32 %23, 10
  %25 = icmp sgt i32 %24, 5
  %26 = add i32 %23, 11
  %27 = select i1 %25, i32 %26, i32 %24
  %28 = add i32 %27, 10
  %29 = icmp sgt i32 %28, 5
  %30 = add i32 %27, 11
  %31 = select i1 %29, i32 %30, i32 %28
  %32 = add i32 %31, 10
  %33 = icmp sgt i32 %32, 5
  %34 = add i32 %31, 11
  %35 = select i1 %33, i32 %34, i32 %32
  %36 = add i32 %35, 10
  %37 = icmp sgt i32 %36, 5
  %38 = add i32 %35, 11
  %39 = select i1 %37, i32 %38, i32 %36
  %40 = add i32 %39, 10
  ret i32 %40
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = icmp sgt i32 %3, 5
  %5 = zext i1 %4 to i32
  %spec.select.i = add nuw i32 %3, %5
  %6 = add i32 %spec.select.i, 10
  %7 = icmp sgt i32 %6, 5
  %8 = add i32 %spec.select.i, 11
  %9 = select i1 %7, i32 %8, i32 %6
  %10 = add i32 %9, 10
  %11 = icmp sgt i32 %10, 5
  %12 = add i32 %9, 11
  %13 = select i1 %11, i32 %12, i32 %10
  %14 = add i32 %13, 10
  %15 = icmp sgt i32 %14, 5
  %16 = add i32 %13, 11
  %17 = select i1 %15, i32 %16, i32 %14
  %18 = add i32 %17, 10
  %19 = icmp sgt i32 %18, 5
  %20 = add i32 %17, 11
  %21 = select i1 %19, i32 %20, i32 %18
  %22 = add i32 %21, 10
  %23 = icmp sgt i32 %22, 5
  %24 = add i32 %21, 11
  %25 = select i1 %23, i32 %24, i32 %22
  %26 = add i32 %25, 10
  %27 = icmp sgt i32 %26, 5
  %28 = add i32 %25, 11
  %29 = select i1 %27, i32 %28, i32 %26
  %30 = add i32 %29, 10
  %31 = icmp sgt i32 %30, 5
  %32 = add i32 %29, 11
  %33 = select i1 %31, i32 %32, i32 %30
  %34 = add i32 %33, 10
  %35 = icmp sgt i32 %34, 5
  %36 = add i32 %33, 11
  %37 = select i1 %35, i32 %36, i32 %34
  %38 = add i32 %37, 10
  %39 = icmp sgt i32 %38, 5
  %40 = add i32 %37, 11
  %41 = select i1 %39, i32 %40, i32 %38
  %42 = add i32 %41, 10
  %43 = getelementptr i8, ptr %0, i64 8
  %44 = load ptr, ptr %43, align 8
  store i32 %42, ptr %44, align 4
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = icmp sgt i32 %3, 5
  %5 = zext i1 %4 to i32
  %spec.select.i.i = add nuw i32 %3, %5
  %6 = add i32 %spec.select.i.i, 10
  %7 = icmp sgt i32 %6, 5
  %8 = add i32 %spec.select.i.i, 11
  %9 = select i1 %7, i32 %8, i32 %6
  %10 = add i32 %9, 10
  %11 = icmp sgt i32 %10, 5
  %12 = add i32 %9, 11
  %13 = select i1 %11, i32 %12, i32 %10
  %14 = add i32 %13, 10
  %15 = icmp sgt i32 %14, 5
  %16 = add i32 %13, 11
  %17 = select i1 %15, i32 %16, i32 %14
  %18 = add i32 %17, 10
  %19 = icmp sgt i32 %18, 5
  %20 = add i32 %17, 11
  %21 = select i1 %19, i32 %20, i32 %18
  %22 = add i32 %21, 10
  %23 = icmp sgt i32 %22, 5
  %24 = add i32 %21, 11
  %25 = select i1 %23, i32 %24, i32 %22
  %26 = add i32 %25, 10
  %27 = icmp sgt i32 %26, 5
  %28 = add i32 %25, 11
  %29 = select i1 %27, i32 %28, i32 %26
  %30 = add i32 %29, 10
  %31 = icmp sgt i32 %30, 5
  %32 = add i32 %29, 11
  %33 = select i1 %31, i32 %32, i32 %30
  %34 = add i32 %33, 10
  %35 = icmp sgt i32 %34, 5
  %36 = add i32 %33, 11
  %37 = select i1 %35, i32 %36, i32 %34
  %38 = add i32 %37, 10
  %39 = icmp sgt i32 %38, 5
  %40 = add i32 %37, 11
  %41 = select i1 %39, i32 %40, i32 %38
  %42 = add i32 %41, 10
  %43 = getelementptr i8, ptr %0, i64 8
  %44 = load ptr, ptr %43, align 8
  store i32 %42, ptr %44, align 4
  ret void
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
attributes #2 = { mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
