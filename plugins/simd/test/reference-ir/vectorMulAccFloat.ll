; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @execute(ptr readonly %0, ptr readonly %1, ptr readonly %2, ptr readonly %3, ptr writeonly %4) local_unnamed_addr #0 {
  %6 = load <16 x float>, ptr %0, align 4
  %7 = load <16 x float>, ptr %1, align 4
  %8 = fmul <16 x float> %6, %7
  %9 = load <16 x float>, ptr %2, align 4
  %10 = load <16 x float>, ptr %3, align 4
  %11 = fmul <16 x float> %9, %10
  %12 = fadd <16 x float> %8, %11
  store <16 x float> %12, ptr %4, align 4
  ret void
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
