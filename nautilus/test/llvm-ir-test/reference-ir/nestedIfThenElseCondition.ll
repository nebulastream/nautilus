; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i32 @execute(i32 %0, i32 %1) local_unnamed_addr #0 {
  %3 = icmp eq i32 %0, 42
  %4 = icmp eq i32 %1, 8
  %or.cond = select i1 %3, i1 true, i1 %4
  %5 = select i1 %or.cond, i32 2, i32 4
  %6 = add i32 %5, %1
  ret i32 %6
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
