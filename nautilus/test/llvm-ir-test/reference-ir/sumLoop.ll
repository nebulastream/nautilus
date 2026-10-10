; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i32 @execute(i32 %0) local_unnamed_addr #0 {
._crit_edge:
  %1 = mul i32 %0, 10
  %2 = or disjoint i32 %1, 1
  %.inv = icmp slt i32 %0, 1
  %.lcssa = select i1 %.inv, i32 1, i32 %2
  ret i32 %.lcssa
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
