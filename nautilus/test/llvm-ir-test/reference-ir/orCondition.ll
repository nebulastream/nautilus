; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i32 @execute(i32 %0) local_unnamed_addr #0 {
  %2 = icmp eq i32 %0, 8
  %3 = icmp eq i32 %0, 1
  %4 = or i1 %2, %3
  %5 = select i1 %4, i32 15, i32 1
  ret i32 %5
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i32 @_mlir_ciface_execute(i32 %0) local_unnamed_addr #0 {
  %2 = icmp eq i32 %0, 8
  %3 = icmp eq i32 %0, 1
  %4 = or i1 %2, %3
  %5 = select i1 %4, i32 15, i32 1
  ret i32 %5
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #1 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = icmp eq i32 %3, 8
  %5 = icmp eq i32 %3, 1
  %6 = or i1 %4, %5
  %7 = select i1 %6, i32 15, i32 1
  %8 = getelementptr i8, ptr %0, i64 8
  %9 = load ptr, ptr %8, align 8
  store i32 %7, ptr %9, align 4
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #1 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = icmp eq i32 %3, 8
  %5 = icmp eq i32 %3, 1
  %6 = or i1 %4, %5
  %7 = select i1 %6, i32 15, i32 1
  %8 = getelementptr i8, ptr %0, i64 8
  %9 = load ptr, ptr %8, align 8
  store i32 %7, ptr %9, align 4
  ret void
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
