; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:o-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "arm64-apple-darwin25.4.0"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define zeroext i8 @execute(i8 %0, i8 %1) local_unnamed_addr #0 {
  %3 = zext i8 %0 to i32
  %4 = zext nneg i8 %1 to i32
  %5 = lshr i32 %3, %4
  %6 = trunc nuw i32 %5 to i8
  ret i8 %6
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define zeroext i8 @_mlir_ciface_execute(i8 %0, i8 %1) local_unnamed_addr #1 {
  %3 = zext i8 %0 to i32
  %4 = zext nneg i8 %1 to i32
  %5 = lshr i32 %3, %4
  %6 = trunc nuw i32 %5 to i8
  ret i8 %6
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i8, ptr %2, align 1
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load i8, ptr %5, align 1
  %7 = zext i8 %3 to i32
  %8 = zext nneg i8 %6 to i32
  %9 = lshr i32 %7, %8
  %10 = trunc nuw i32 %9 to i8
  %11 = getelementptr i8, ptr %0, i64 16
  %12 = load ptr, ptr %11, align 8
  store i8 %10, ptr %12, align 1
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i8, ptr %2, align 1
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load i8, ptr %5, align 1
  %7 = zext i8 %3 to i32
  %8 = zext nneg i8 %6 to i32
  %9 = lshr i32 %7, %8
  %10 = trunc nuw i32 %9 to i8
  %11 = getelementptr i8, ptr %0, i64 16
  %12 = load ptr, ptr %11, align 8
  store i8 %10, ptr %12, align 1
  ret void
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) "target-features" }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
attributes #2 = { mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
