; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nofree nounwind memory(read)
define signext i32 @execute(i1 zeroext %0) local_unnamed_addr #0 {
  %2 = tail call i32 @runtimeFunc0(i1 %0)
  %3 = tail call i32 @runtimeFunc0(i1 false)
  ret i32 %3
}

; Function Attrs: nofree nounwind memory(read)
declare i32 @runtimeFunc0(i1 zeroext) local_unnamed_addr #1

attributes #0 = { nofree nounwind memory(read) }
attributes #1 = { nofree nounwind memory(read) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
