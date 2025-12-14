; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(inaccessiblemem: write)
define signext i32 @execute(i32 %0, i32 %1) local_unnamed_addr #0 {
  %3 = icmp sgt i32 %0, 0
  %4 = icmp sgt i32 %1, 0
  tail call void @llvm.assume(i1 %3)
  tail call void @llvm.assume(i1 %4)
  %5 = add nuw i32 %1, %0
  ret i32 %5
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(inaccessiblemem: write)
define signext i32 @_mlir_ciface_execute(i32 %0, i32 %1) local_unnamed_addr #1 {
  %3 = icmp sgt i32 %0, 0
  %4 = icmp sgt i32 %1, 0
  tail call void @llvm.assume(i1 %3)
  tail call void @llvm.assume(i1 %4)
  %5 = add nuw i32 %1, %0
  ret i32 %5
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(inaccessiblemem: write)
declare void @llvm.assume(i1 noundef) #2

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: write)
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #3 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load i32, ptr %5, align 4
  %7 = icmp sgt i32 %3, 0
  %8 = icmp sgt i32 %6, 0
  tail call void @llvm.assume(i1 %7)
  tail call void @llvm.assume(i1 %8)
  %9 = add nuw i32 %6, %3
  %10 = getelementptr i8, ptr %0, i64 16
  %11 = load ptr, ptr %10, align 8
  store i32 %9, ptr %11, align 4
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: write)
define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #3 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load i32, ptr %5, align 4
  %7 = icmp sgt i32 %3, 0
  %8 = icmp sgt i32 %6, 0
  tail call void @llvm.assume(i1 %7)
  tail call void @llvm.assume(i1 %8)
  %9 = add nuw i32 %6, %3
  %10 = getelementptr i8, ptr %0, i64 16
  %11 = load ptr, ptr %10, align 8
  store i32 %9, ptr %11, align 4
  ret void
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(inaccessiblemem: write) }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(inaccessiblemem: write) }
attributes #2 = { mustprogress nocallback nofree nosync nounwind willreturn memory(inaccessiblemem: write) }
attributes #3 = { mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: write) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
