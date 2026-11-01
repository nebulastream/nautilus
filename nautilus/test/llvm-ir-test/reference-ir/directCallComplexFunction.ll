; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: memory(readwrite)
declare double @runtimeFunc0(i32, i32) local_unnamed_addr #0

define signext i32 @execute(i32 %0, i32 %1) local_unnamed_addr #1 {
  %3 = tail call double @runtimeFunc0(i32 %0, i32 %1)
  %4 = tail call i32 @llvm.fptosi.sat.i32.f64(double %3)
  ret i32 %4
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i32 @llvm.fptosi.sat.i32.f64(double) #2

attributes #0 = { memory(readwrite) }
attributes #2 = { mustprogress nocallback nofree nosync nounwind speculatable willreturn memory(none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
