; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define signext i32 @execute(ptr %0) local_unnamed_addr #0 {
  %2 = load i32, ptr %0, align 4
  %3 = add i32 %2, 10
  store i32 %3, ptr %0, align 4
  ret i32 %3
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite)
define void @voidHelper(ptr %0) local_unnamed_addr #0 {
  %2 = load i32, ptr %0, align 4
  %3 = add i32 %2, 10
  store i32 %3, ptr %0, align 4
  ret void
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: readwrite) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
