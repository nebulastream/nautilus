; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i32 @execute(i32 %0) local_unnamed_addr #0 {
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

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
