; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i32 @execute(i32 %0) local_unnamed_addr #0 {
  %2 = icmp eq i32 %0, 0
  br i1 %2, label %._crit_edge, label %.lr.ph.preheader

.lr.ph.preheader:                                 ; preds = %1
  %3 = add i32 %0, -1
  %umin = tail call i32 @llvm.umin.i32(i32 %3, i32 9)
  %4 = mul nuw nsw i32 %umin, 10
  %5 = add nuw nsw i32 %4, 10
  br label %._crit_edge

._crit_edge:                                      ; preds = %.lr.ph.preheader, %1
  %.lcssa = phi i32 [ 0, %1 ], [ %5, %.lr.ph.preheader ]
  ret i32 %.lcssa
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i32 @_mlir_ciface_execute(i32 %0) local_unnamed_addr #1 {
  %2 = icmp eq i32 %0, 0
  br i1 %2, label %execute.exit, label %.lr.ph.preheader.i

.lr.ph.preheader.i:                               ; preds = %1
  %3 = add i32 %0, -1
  %umin.i = tail call i32 @llvm.umin.i32(i32 %3, i32 9)
  %4 = mul nuw nsw i32 %umin.i, 10
  %5 = add nuw nsw i32 %4, 10
  br label %execute.exit

execute.exit:                                     ; preds = %1, %.lr.ph.preheader.i
  %.lcssa.i = phi i32 [ 0, %1 ], [ %5, %.lr.ph.preheader.i ]
  ret i32 %.lcssa.i
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = icmp eq i32 %3, 0
  br i1 %4, label %execute.exit, label %.lr.ph.preheader.i

.lr.ph.preheader.i:                               ; preds = %1
  %5 = add i32 %3, -1
  %umin.i = tail call i32 @llvm.umin.i32(i32 %5, i32 9)
  %6 = mul nuw nsw i32 %umin.i, 10
  %7 = add nuw nsw i32 %6, 10
  br label %execute.exit

execute.exit:                                     ; preds = %1, %.lr.ph.preheader.i
  %.lcssa.i = phi i32 [ 0, %1 ], [ %7, %.lr.ph.preheader.i ]
  %8 = getelementptr i8, ptr %0, i64 8
  %9 = load ptr, ptr %8, align 8
  store i32 %.lcssa.i, ptr %9, align 4
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = icmp eq i32 %3, 0
  br i1 %4, label %_mlir_ciface_execute.exit, label %.lr.ph.preheader.i.i

.lr.ph.preheader.i.i:                             ; preds = %1
  %5 = add i32 %3, -1
  %umin.i.i = tail call i32 @llvm.umin.i32(i32 %5, i32 9)
  %6 = mul nuw nsw i32 %umin.i.i, 10
  %7 = add nuw nsw i32 %6, 10
  br label %_mlir_ciface_execute.exit

_mlir_ciface_execute.exit:                        ; preds = %1, %.lr.ph.preheader.i.i
  %.lcssa.i.i = phi i32 [ 0, %1 ], [ %7, %.lr.ph.preheader.i.i ]
  %8 = getelementptr i8, ptr %0, i64 8
  %9 = load ptr, ptr %8, align 8
  store i32 %.lcssa.i.i, ptr %9, align 4
  ret void
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i32 @llvm.umin.i32(i32, i32) #3

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
attributes #2 = { mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none) }
attributes #3 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
