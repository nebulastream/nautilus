; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i32 @execute(i32 %0) local_unnamed_addr #0 {
  %switch.selectcmp = icmp eq i32 %0, 1
  %switch.select = select i1 %switch.selectcmp, i32 20, i32 30
  %switch.selectcmp1 = icmp eq i32 %0, 0
  %switch.select2 = select i1 %switch.selectcmp1, i32 10, i32 %switch.select
  ret i32 %switch.select2
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i32 @_mlir_ciface_execute(i32 %0) local_unnamed_addr #1 {
  %switch.selectcmp.i = icmp eq i32 %0, 1
  %switch.select.i = select i1 %switch.selectcmp.i, i32 20, i32 30
  %switch.selectcmp1.i = icmp eq i32 %0, 0
  %switch.select2.i = select i1 %switch.selectcmp1.i, i32 10, i32 %switch.select.i
  ret i32 %switch.select2.i
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %switch.selectcmp.i = icmp eq i32 %3, 1
  %switch.select.i = select i1 %switch.selectcmp.i, i32 20, i32 30
  %switch.selectcmp1.i = icmp eq i32 %3, 0
  %switch.select2.i = select i1 %switch.selectcmp1.i, i32 10, i32 %switch.select.i
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  store i32 %switch.select2.i, ptr %5, align 4
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %switch.selectcmp.i.i = icmp eq i32 %3, 1
  %switch.select.i.i = select i1 %switch.selectcmp.i.i, i32 20, i32 30
  %switch.selectcmp1.i.i = icmp eq i32 %3, 0
  %switch.select2.i.i = select i1 %switch.selectcmp1.i.i, i32 10, i32 %switch.select.i.i
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  store i32 %switch.select2.i.i, ptr %5, align 4
  ret void
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
attributes #2 = { mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
