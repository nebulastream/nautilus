; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @execute(ptr readonly %0, ptr readonly %1, ptr readonly %2, ptr readonly %3, ptr readonly %4, ptr writeonly %5) local_unnamed_addr #0 {
  %7 = load <16 x float>, ptr %0, align 4
  %8 = load <16 x float>, ptr %1, align 4
  %9 = load <16 x float>, ptr %2, align 4
  %10 = load <16 x float>, ptr %3, align 4
  %11 = load <16 x float>, ptr %4, align 4
  %12 = tail call <16 x float> @llvm.fma.v16f32(<16 x float> %11, <16 x float> %7, <16 x float> %10)
  %13 = tail call <16 x float> @llvm.fma.v16f32(<16 x float> %12, <16 x float> %7, <16 x float> %9)
  %14 = tail call <16 x float> @llvm.fma.v16f32(<16 x float> %13, <16 x float> %7, <16 x float> %8)
  store <16 x float> %14, ptr %5, align 4
  ret void
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare <16 x float> @llvm.fma.v16f32(<16 x float>, <16 x float>, <16 x float>) #1

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite) }
attributes #1 = { mustprogress nocallback nofree nosync nounwind speculatable willreturn memory(none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
