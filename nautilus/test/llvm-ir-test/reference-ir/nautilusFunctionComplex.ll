; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i32 @complexComputation(i32 %0, i32 %1, i32 %2) local_unnamed_addr #0 {
  %4 = mul i32 %1, %0
  %5 = add i32 %2, %1
  %6 = sub i32 %4, %5
  %7 = shl i32 %6, 1
  ret i32 %7
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i32 @execute(i32 %0, i32 %1) local_unnamed_addr #0 {
  %3 = add i32 %0, 2147483647
  %reass.sub = mul i32 %1, %3
  %4 = add i32 %reass.sub, 2147483643
  %5 = shl i32 %0, 1
  %6 = mul i32 %5, %4
  %7 = add i32 %1, %0
  %8 = sub i32 %6, %7
  %9 = shl i32 %8, 1
  ret i32 %9
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
