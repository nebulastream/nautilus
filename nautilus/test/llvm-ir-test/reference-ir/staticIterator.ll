; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i32 @execute(i32 %0) local_unnamed_addr #0 {
common.ret:
  %1 = icmp sgt i32 %0, 1
  %2 = zext i1 %1 to i32
  %3 = icmp sgt i32 %0, 2
  %spec.select = select i1 %3, i32 2, i32 %2
  %4 = icmp sgt i32 %0, 3
  %5 = zext i1 %4 to i32
  %6 = icmp sgt i32 %0, 4
  %7 = zext i1 %6 to i32
  %8 = icmp sgt i32 %0, 5
  %9 = zext i1 %8 to i32
  %10 = add nuw nsw i32 %7, %5
  %11 = add nuw nsw i32 %10, %9
  %common.ret.op = add nuw nsw i32 %11, %spec.select
  ret i32 %common.ret.op
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
