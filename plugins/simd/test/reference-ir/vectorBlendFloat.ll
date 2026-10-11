; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @execute(ptr readonly %0, ptr readonly %1, ptr readonly %2, ptr writeonly %3) local_unnamed_addr #0 {
  %5 = load <16 x float>, ptr %0, align 4
  %6 = load <16 x float>, ptr %1, align 4
  %7 = load <16 x i32>, ptr %2, align 4
  %.not = icmp eq <16 x i32> %7, zeroinitializer
  %8 = select <16 x i1> %.not, <16 x float> %6, <16 x float> %5
  store <16 x float> %8, ptr %3, align 4
  ret void
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
