; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define zeroext i8 @execute(i8 zeroext %0, i8 zeroext %1) local_unnamed_addr #0 {
  %3 = zext i8 %0 to i32
  %4 = zext nneg i8 %1 to i32
  %5 = lshr i32 %3, %4
  %6 = trunc nuw i32 %5 to i8
  ret i8 %6
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
