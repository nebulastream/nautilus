; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nofree norecurse nosync nounwind memory(none)
define signext i32 @execute(i32 %0) local_unnamed_addr #0 {
  br label %2

2:                                                ; preds = %2, %1
  %3 = phi i32 [ %8, %2 ], [ %0, %1 ]
  %4 = phi i32 [ %7, %2 ], [ 0, %1 ]
  %5 = mul i32 %4, 10
  %6 = srem i32 %3, 10
  %7 = add i32 %5, %6
  %8 = udiv i32 %3, 10
  %9 = icmp sgt i32 %3, 9
  br i1 %9, label %2, label %10

10:                                               ; preds = %2
  ret i32 %7
}

; Function Attrs: nofree norecurse nosync nounwind memory(none)
define signext i32 @_mlir_ciface_execute(i32 %0) local_unnamed_addr #1 {
  br label %2

2:                                                ; preds = %2, %1
  %3 = phi i32 [ %8, %2 ], [ %0, %1 ]
  %4 = phi i32 [ %7, %2 ], [ 0, %1 ]
  %5 = mul i32 %4, 10
  %6 = srem i32 %3, 10
  %7 = add i32 %5, %6
  %8 = udiv i32 %3, 10
  %9 = icmp sgt i32 %3, 9
  br i1 %9, label %2, label %execute.exit

execute.exit:                                     ; preds = %2
  ret i32 %7
}

; Function Attrs: nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: none)
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  br label %4

4:                                                ; preds = %4, %1
  %5 = phi i32 [ %10, %4 ], [ %3, %1 ]
  %6 = phi i32 [ %9, %4 ], [ 0, %1 ]
  %7 = mul i32 %6, 10
  %8 = srem i32 %5, 10
  %9 = add i32 %7, %8
  %10 = udiv i32 %5, 10
  %11 = icmp sgt i32 %5, 9
  br i1 %11, label %4, label %execute.exit

execute.exit:                                     ; preds = %4
  %12 = getelementptr i8, ptr %0, i64 8
  %13 = load ptr, ptr %12, align 8
  store i32 %9, ptr %13, align 4
  ret void
}

; Function Attrs: nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: none)
define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  br label %4

4:                                                ; preds = %4, %1
  %5 = phi i32 [ %10, %4 ], [ %3, %1 ]
  %6 = phi i32 [ %9, %4 ], [ 0, %1 ]
  %7 = mul i32 %6, 10
  %8 = srem i32 %5, 10
  %9 = add i32 %7, %8
  %10 = udiv i32 %5, 10
  %11 = icmp sgt i32 %5, 9
  br i1 %11, label %4, label %_mlir_ciface_execute.exit

_mlir_ciface_execute.exit:                        ; preds = %4
  %12 = getelementptr i8, ptr %0, i64 8
  %13 = load ptr, ptr %12, align 8
  store i32 %9, ptr %13, align 4
  ret void
}

attributes #0 = { nofree norecurse nosync nounwind memory(none) }
attributes #1 = { nofree norecurse nosync nounwind memory(none) }
attributes #2 = { nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
