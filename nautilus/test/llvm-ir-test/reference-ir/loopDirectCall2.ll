; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nounwind
define signext i32 @execute(i32 %0) local_unnamed_addr #0 {
  %2 = tail call i32 @runtimeFunc0()
  %3 = sext i32 %2 to i64
  %.not = icmp eq i32 %2, 0
  br i1 %.not, label %._crit_edge, label %.lr.ph

.lr.ph:                                           ; preds = %1, %.lr.ph
  %4 = phi i64 [ %7, %.lr.ph ], [ 0, %1 ]
  %5 = phi i32 [ %6, %.lr.ph ], [ 0, %1 ]
  %6 = tail call i32 @runtimeFunc1(i32 %5, i32 %0)
  %7 = add nuw i64 %4, 1
  %exitcond.not = icmp eq i64 %7, %3
  br i1 %exitcond.not, label %._crit_edge, label %.lr.ph

._crit_edge:                                      ; preds = %.lr.ph, %1
  %.lcssa = phi i32 [ 0, %1 ], [ %6, %.lr.ph ]
  ret i32 %.lcssa
}

; Function Attrs: nounwind memory(readwrite)
declare i32 @runtimeFunc0() local_unnamed_addr #1

; Function Attrs: nounwind memory(readwrite)
declare i32 @runtimeFunc1(i32, i32) local_unnamed_addr #1

attributes #0 = { nounwind }
attributes #1 = { nounwind memory(readwrite) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
