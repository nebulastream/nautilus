; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nofree norecurse nosync nounwind memory(none)
define signext range(i32 0, 110) i32 @execute(i32 %0) local_unnamed_addr #0 {
  br label %2

2:                                                ; preds = %2, %1
  %3 = phi i32 [ %6, %2 ], [ 0, %1 ]
  %4 = icmp samesign ugt i32 %3, 99
  %5 = icmp eq i32 %3, %0
  %or.cond = select i1 %4, i1 true, i1 %5
  %6 = add nuw nsw i32 %3, 10
  br i1 %or.cond, label %7, label %2

7:                                                ; preds = %2
  ret i32 %3
}

; Function Attrs: nofree norecurse nosync nounwind memory(none)
define signext range(i32 0, 110) i32 @_mlir_ciface_execute(i32 %0) local_unnamed_addr #1 {
  br label %2

2:                                                ; preds = %2, %1
  %3 = phi i32 [ %6, %2 ], [ 0, %1 ]
  %4 = icmp samesign ugt i32 %3, 99
  %5 = icmp eq i32 %3, %0
  %or.cond.i = select i1 %4, i1 true, i1 %5
  %6 = add nuw nsw i32 %3, 10
  br i1 %or.cond.i, label %execute.exit, label %2

execute.exit:                                     ; preds = %2
  ret i32 %3
}

; Function Attrs: nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: none)
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  br label %4

4:                                                ; preds = %4, %1
  %5 = phi i32 [ %8, %4 ], [ 0, %1 ]
  %6 = icmp samesign ugt i32 %5, 99
  %7 = icmp eq i32 %5, %3
  %or.cond.i = select i1 %6, i1 true, i1 %7
  %8 = add nuw nsw i32 %5, 10
  br i1 %or.cond.i, label %execute.exit, label %4

execute.exit:                                     ; preds = %4
  %9 = getelementptr i8, ptr %0, i64 8
  %10 = load ptr, ptr %9, align 8
  store i32 %5, ptr %10, align 4
  ret void
}

; Function Attrs: nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: none)
define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  br label %4

4:                                                ; preds = %4, %1
  %5 = phi i32 [ %8, %4 ], [ 0, %1 ]
  %6 = icmp samesign ugt i32 %5, 99
  %7 = icmp eq i32 %5, %3
  %or.cond.i.i = select i1 %6, i1 true, i1 %7
  %8 = add nuw nsw i32 %5, 10
  br i1 %or.cond.i.i, label %_mlir_ciface_execute.exit, label %4

_mlir_ciface_execute.exit:                        ; preds = %4
  %9 = getelementptr i8, ptr %0, i64 8
  %10 = load ptr, ptr %9, align 8
  store i32 %5, ptr %10, align 4
  ret void
}

attributes #0 = { nofree norecurse nosync nounwind memory(none) }
attributes #1 = { nofree norecurse nosync nounwind memory(none) }
attributes #2 = { nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
