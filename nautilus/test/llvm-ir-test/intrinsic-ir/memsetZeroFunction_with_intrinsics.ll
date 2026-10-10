; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nounwind willreturn memory(argmem: write, inaccessiblemem: readwrite)
define ptr @execute(ptr returned writeonly %0, i64 %1) local_unnamed_addr #0 {
  tail call void @llvm.memset.p0.i64(ptr %0, i8 0, i64 %1, i1 true)
  ret ptr %0
}

; Function Attrs: mustprogress nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr writeonly, i8, i64, i1 immarg) #1

attributes #0 = { mustprogress nofree norecurse nounwind willreturn memory(argmem: write, inaccessiblemem: readwrite) }
attributes #1 = { mustprogress nocallback nofree nounwind willreturn memory(argmem: write) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
