; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i32 @execute(i32 %0) local_unnamed_addr #0 {
  %2 = icmp eq i32 %0, 1
  br i1 %2, label %3, label %5

3:                                                ; preds = %7, %5, %1
  %4 = phi i32 [ %9, %7 ], [ 42, %5 ], [ 1, %1 ]
  ret i32 %4

5:                                                ; preds = %1
  %6 = icmp slt i32 %0, 10
  br i1 %6, label %3, label %7

7:                                                ; preds = %5
  %8 = shl nuw i32 %0, 1
  %9 = or disjoint i32 %8, 1
  br label %3
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i32 @_mlir_ciface_execute(i32 %0) local_unnamed_addr #1 {
  %2 = icmp eq i32 %0, 1
  br i1 %2, label %execute.exit, label %3

3:                                                ; preds = %1
  %4 = icmp slt i32 %0, 10
  br i1 %4, label %execute.exit, label %5

5:                                                ; preds = %3
  %6 = shl nuw i32 %0, 1
  %7 = or disjoint i32 %6, 1
  br label %execute.exit

execute.exit:                                     ; preds = %1, %3, %5
  %8 = phi i32 [ %7, %5 ], [ 42, %3 ], [ 1, %1 ]
  ret i32 %8
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = icmp eq i32 %3, 1
  br i1 %4, label %execute.exit, label %5

5:                                                ; preds = %1
  %6 = icmp slt i32 %3, 10
  br i1 %6, label %execute.exit, label %7

7:                                                ; preds = %5
  %8 = shl nuw i32 %3, 1
  %9 = or disjoint i32 %8, 1
  br label %execute.exit

execute.exit:                                     ; preds = %1, %5, %7
  %10 = phi i32 [ %9, %7 ], [ 42, %5 ], [ 1, %1 ]
  %11 = getelementptr i8, ptr %0, i64 8
  %12 = load ptr, ptr %11, align 8
  store i32 %10, ptr %12, align 4
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = icmp eq i32 %3, 1
  br i1 %4, label %_mlir_ciface_execute.exit, label %5

5:                                                ; preds = %1
  %6 = icmp slt i32 %3, 10
  br i1 %6, label %_mlir_ciface_execute.exit, label %7

7:                                                ; preds = %5
  %8 = shl nuw i32 %3, 1
  %9 = or disjoint i32 %8, 1
  br label %_mlir_ciface_execute.exit

_mlir_ciface_execute.exit:                        ; preds = %1, %5, %7
  %10 = phi i32 [ %9, %7 ], [ 42, %5 ], [ 1, %1 ]
  %11 = getelementptr i8, ptr %0, i64 8
  %12 = load ptr, ptr %11, align 8
  store i32 %10, ptr %12, align 4
  ret void
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
attributes #2 = { mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
