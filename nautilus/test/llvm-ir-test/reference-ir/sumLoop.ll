; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext range(i32 1, 0) i32 @execute(i32 %0) local_unnamed_addr #0 {
._crit_edge:
  %1 = mul i32 %0, 10
  %2 = or disjoint i32 %1, 1
  %.inv = icmp slt i32 %0, 1
  %.lcssa = select i1 %.inv, i32 1, i32 %2
  ret i32 %.lcssa
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext range(i32 1, 0) i32 @_mlir_ciface_execute(i32 %0) local_unnamed_addr #1 {
  %2 = mul i32 %0, 10
  %3 = or disjoint i32 %2, 1
  %.inv.i = icmp slt i32 %0, 1
  %.lcssa.i = select i1 %.inv.i, i32 1, i32 %3
  ret i32 %.lcssa.i
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = mul i32 %3, 10
  %5 = or disjoint i32 %4, 1
  %.inv.i = icmp slt i32 %3, 1
  %.lcssa.i = select i1 %.inv.i, i32 1, i32 %5
  %6 = getelementptr i8, ptr %0, i64 8
  %7 = load ptr, ptr %6, align 8
  store i32 %.lcssa.i, ptr %7, align 4
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = mul i32 %3, 10
  %5 = or disjoint i32 %4, 1
  %.inv.i.i = icmp slt i32 %3, 1
  %.lcssa.i.i = select i1 %.inv.i.i, i32 1, i32 %5
  %6 = getelementptr i8, ptr %0, i64 8
  %7 = load ptr, ptr %6, align 8
  store i32 %.lcssa.i.i, ptr %7, align 4
  ret void
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
attributes #2 = { mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
