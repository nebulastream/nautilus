; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext range(i32 0, 11) i32 @execute(i32 %0) local_unnamed_addr #0 {
  %2 = icmp sgt i32 %0, 0
  %spec.select = zext i1 %2 to i32
  %.inv = icmp slt i32 %0, 2
  %3 = select i1 %.inv, i32 %spec.select, i32 2
  %4 = insertelement <8 x i32> poison, i32 %0, i64 0
  %5 = shufflevector <8 x i32> %4, <8 x i32> poison, <8 x i32> zeroinitializer
  %6 = icmp sgt <8 x i32> %5, <i32 3, i32 2, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9>
  %7 = bitcast <8 x i1> %6 to i8
  %8 = tail call range(i8 0, 9) i8 @llvm.ctpop.i8(i8 %7)
  %9 = zext nneg i8 %8 to i32
  %op.rdx = add nuw nsw i32 %3, %9
  ret i32 %op.rdx
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext range(i32 0, 11) i32 @_mlir_ciface_execute(i32 %0) local_unnamed_addr #1 {
  %2 = icmp sgt i32 %0, 0
  %spec.select.i = zext i1 %2 to i32
  %.inv.i = icmp slt i32 %0, 2
  %3 = select i1 %.inv.i, i32 %spec.select.i, i32 2
  %4 = insertelement <8 x i32> poison, i32 %0, i64 0
  %5 = shufflevector <8 x i32> %4, <8 x i32> poison, <8 x i32> zeroinitializer
  %6 = icmp sgt <8 x i32> %5, <i32 3, i32 2, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9>
  %7 = bitcast <8 x i1> %6 to i8
  %8 = tail call range(i8 0, 9) i8 @llvm.ctpop.i8(i8 %7)
  %9 = zext nneg i8 %8 to i32
  %op.rdx = add nuw nsw i32 %3, %9
  ret i32 %op.rdx
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = icmp sgt i32 %3, 0
  %spec.select.i = zext i1 %4 to i32
  %.inv.i = icmp slt i32 %3, 2
  %5 = select i1 %.inv.i, i32 %spec.select.i, i32 2
  %6 = insertelement <8 x i32> poison, i32 %3, i64 0
  %7 = shufflevector <8 x i32> %6, <8 x i32> poison, <8 x i32> zeroinitializer
  %8 = icmp sgt <8 x i32> %7, <i32 3, i32 2, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9>
  %9 = bitcast <8 x i1> %8 to i8
  %10 = tail call range(i8 0, 9) i8 @llvm.ctpop.i8(i8 %9)
  %11 = zext nneg i8 %10 to i32
  %op.rdx = add nuw nsw i32 %5, %11
  %12 = getelementptr i8, ptr %0, i64 8
  %13 = load ptr, ptr %12, align 8
  store i32 %op.rdx, ptr %13, align 4
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = icmp sgt i32 %3, 0
  %spec.select.i.i = zext i1 %4 to i32
  %.inv.i.i = icmp slt i32 %3, 2
  %5 = select i1 %.inv.i.i, i32 %spec.select.i.i, i32 2
  %6 = insertelement <8 x i32> poison, i32 %3, i64 0
  %7 = shufflevector <8 x i32> %6, <8 x i32> poison, <8 x i32> zeroinitializer
  %8 = icmp sgt <8 x i32> %7, <i32 3, i32 2, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9>
  %9 = bitcast <8 x i1> %8 to i8
  %10 = tail call range(i8 0, 9) i8 @llvm.ctpop.i8(i8 %9)
  %11 = zext nneg i8 %10 to i32
  %op.rdx = add nuw nsw i32 %5, %11
  %12 = getelementptr i8, ptr %0, i64 8
  %13 = load ptr, ptr %12, align 8
  store i32 %op.rdx, ptr %13, align 4
  ret void
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i8 @llvm.ctpop.i8(i8) #3

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
attributes #2 = { mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none) }
attributes #3 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
