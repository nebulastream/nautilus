; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: read)
define float @execute(ptr readonly %0, ptr readonly %1) local_unnamed_addr #0 {
  %3 = load <16 x float>, ptr %0, align 4
  %4 = load <16 x float>, ptr %1, align 4
  %5 = fsub <16 x float> %3, %4
  %6 = fmul <16 x float> %5, %5
  %7 = tail call reassoc float @llvm.vector.reduce.fadd.v16f32(float 0.000000e+00, <16 x float> %6)
  ret float %7
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare float @llvm.vector.reduce.fadd.v16f32(float, <16 x float>) #1

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: read) }
attributes #1 = { mustprogress nocallback nofree nosync nounwind speculatable willreturn memory(none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
