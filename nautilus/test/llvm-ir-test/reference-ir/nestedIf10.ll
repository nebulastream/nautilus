; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext range(i32 0, 11) i32 @execute(i32 %0) local_unnamed_addr #0 {
  %2 = icmp sgt i32 %0, 0
  br i1 %2, label %3, label %16

3:                                                ; preds = %1
  %.not = icmp eq i32 %0, 1
  br i1 %.not, label %16, label %4

4:                                                ; preds = %3
  %5 = icmp samesign ugt i32 %0, 2
  br i1 %5, label %6, label %16

6:                                                ; preds = %4
  %.not1 = icmp eq i32 %0, 3
  br i1 %.not1, label %16, label %7

7:                                                ; preds = %6
  %8 = icmp samesign ugt i32 %0, 4
  br i1 %8, label %9, label %16

9:                                                ; preds = %7
  %.not2 = icmp eq i32 %0, 5
  br i1 %.not2, label %16, label %10

10:                                               ; preds = %9
  %11 = icmp samesign ugt i32 %0, 6
  br i1 %11, label %12, label %16

12:                                               ; preds = %10
  %.not3 = icmp eq i32 %0, 7
  br i1 %.not3, label %16, label %13

13:                                               ; preds = %12
  %14 = icmp samesign ugt i32 %0, 8
  br i1 %14, label %15, label %16

15:                                               ; preds = %13
  %.not4 = icmp eq i32 %0, 9
  %spec.select = select i1 %.not4, i32 9, i32 10
  br label %16

16:                                               ; preds = %15, %13, %12, %10, %9, %7, %6, %4, %3, %1
  %17 = phi i32 [ 8, %13 ], [ 7, %12 ], [ 6, %10 ], [ 5, %9 ], [ 4, %7 ], [ 3, %6 ], [ 2, %4 ], [ 1, %3 ], [ 0, %1 ], [ %spec.select, %15 ]
  ret i32 %17
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext range(i32 0, 11) i32 @_mlir_ciface_execute(i32 %0) local_unnamed_addr #1 {
  %2 = icmp sgt i32 %0, 0
  br i1 %2, label %3, label %execute.exit

3:                                                ; preds = %1
  %.not.i = icmp eq i32 %0, 1
  br i1 %.not.i, label %execute.exit, label %4

4:                                                ; preds = %3
  %5 = icmp samesign ugt i32 %0, 2
  br i1 %5, label %6, label %execute.exit

6:                                                ; preds = %4
  %.not1.i = icmp eq i32 %0, 3
  br i1 %.not1.i, label %execute.exit, label %7

7:                                                ; preds = %6
  %8 = icmp samesign ugt i32 %0, 4
  br i1 %8, label %9, label %execute.exit

9:                                                ; preds = %7
  %.not2.i = icmp eq i32 %0, 5
  br i1 %.not2.i, label %execute.exit, label %10

10:                                               ; preds = %9
  %11 = icmp samesign ugt i32 %0, 6
  br i1 %11, label %12, label %execute.exit

12:                                               ; preds = %10
  %.not3.i = icmp eq i32 %0, 7
  br i1 %.not3.i, label %execute.exit, label %13

13:                                               ; preds = %12
  %14 = icmp samesign ugt i32 %0, 8
  br i1 %14, label %15, label %execute.exit

15:                                               ; preds = %13
  %.not4.i = icmp eq i32 %0, 9
  %spec.select.i = select i1 %.not4.i, i32 9, i32 10
  br label %execute.exit

execute.exit:                                     ; preds = %1, %3, %4, %6, %7, %9, %10, %12, %13, %15
  %16 = phi i32 [ 8, %13 ], [ 7, %12 ], [ 6, %10 ], [ 5, %9 ], [ 4, %7 ], [ 3, %6 ], [ 2, %4 ], [ 1, %3 ], [ 0, %1 ], [ %spec.select.i, %15 ]
  ret i32 %16
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = icmp sgt i32 %3, 0
  br i1 %4, label %5, label %execute.exit

5:                                                ; preds = %1
  %.not.i = icmp eq i32 %3, 1
  br i1 %.not.i, label %execute.exit, label %6

6:                                                ; preds = %5
  %7 = icmp samesign ugt i32 %3, 2
  br i1 %7, label %8, label %execute.exit

8:                                                ; preds = %6
  %.not1.i = icmp eq i32 %3, 3
  br i1 %.not1.i, label %execute.exit, label %9

9:                                                ; preds = %8
  %10 = icmp samesign ugt i32 %3, 4
  br i1 %10, label %11, label %execute.exit

11:                                               ; preds = %9
  %.not2.i = icmp eq i32 %3, 5
  br i1 %.not2.i, label %execute.exit, label %12

12:                                               ; preds = %11
  %13 = icmp samesign ugt i32 %3, 6
  br i1 %13, label %14, label %execute.exit

14:                                               ; preds = %12
  %.not3.i = icmp eq i32 %3, 7
  br i1 %.not3.i, label %execute.exit, label %15

15:                                               ; preds = %14
  %16 = icmp samesign ugt i32 %3, 8
  br i1 %16, label %17, label %execute.exit

17:                                               ; preds = %15
  %.not4.i = icmp eq i32 %3, 9
  %spec.select.i = select i1 %.not4.i, i32 9, i32 10
  br label %execute.exit

execute.exit:                                     ; preds = %1, %5, %6, %8, %9, %11, %12, %14, %15, %17
  %18 = phi i32 [ 8, %15 ], [ 7, %14 ], [ 6, %12 ], [ 5, %11 ], [ 4, %9 ], [ 3, %8 ], [ 2, %6 ], [ 1, %5 ], [ 0, %1 ], [ %spec.select.i, %17 ]
  %19 = getelementptr i8, ptr %0, i64 8
  %20 = load ptr, ptr %19, align 8
  store i32 %18, ptr %20, align 4
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = icmp sgt i32 %3, 0
  br i1 %4, label %5, label %_mlir_ciface_execute.exit

5:                                                ; preds = %1
  %.not.i.i = icmp eq i32 %3, 1
  br i1 %.not.i.i, label %_mlir_ciface_execute.exit, label %6

6:                                                ; preds = %5
  %7 = icmp samesign ugt i32 %3, 2
  br i1 %7, label %8, label %_mlir_ciface_execute.exit

8:                                                ; preds = %6
  %.not1.i.i = icmp eq i32 %3, 3
  br i1 %.not1.i.i, label %_mlir_ciface_execute.exit, label %9

9:                                                ; preds = %8
  %10 = icmp samesign ugt i32 %3, 4
  br i1 %10, label %11, label %_mlir_ciface_execute.exit

11:                                               ; preds = %9
  %.not2.i.i = icmp eq i32 %3, 5
  br i1 %.not2.i.i, label %_mlir_ciface_execute.exit, label %12

12:                                               ; preds = %11
  %13 = icmp samesign ugt i32 %3, 6
  br i1 %13, label %14, label %_mlir_ciface_execute.exit

14:                                               ; preds = %12
  %.not3.i.i = icmp eq i32 %3, 7
  br i1 %.not3.i.i, label %_mlir_ciface_execute.exit, label %15

15:                                               ; preds = %14
  %16 = icmp samesign ugt i32 %3, 8
  br i1 %16, label %17, label %_mlir_ciface_execute.exit

17:                                               ; preds = %15
  %.not4.i.i = icmp eq i32 %3, 9
  %spec.select.i.i = select i1 %.not4.i.i, i32 9, i32 10
  br label %_mlir_ciface_execute.exit

_mlir_ciface_execute.exit:                        ; preds = %1, %5, %6, %8, %9, %11, %12, %14, %15, %17
  %18 = phi i32 [ 8, %15 ], [ 7, %14 ], [ 6, %12 ], [ 5, %11 ], [ 4, %9 ], [ 3, %8 ], [ 2, %6 ], [ 1, %5 ], [ 0, %1 ], [ %spec.select.i.i, %17 ]
  %19 = getelementptr i8, ptr %0, i64 8
  %20 = load ptr, ptr %19, align 8
  store i32 %18, ptr %20, align 4
  ret void
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
attributes #2 = { mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
