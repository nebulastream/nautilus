; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nofree norecurse nosync nounwind memory(none)
define signext i32 @execute(i32 %0) local_unnamed_addr #0 {
  %2 = icmp sgt i32 %0, 0
  br i1 %2, label %iter.check, label %._crit_edge

iter.check:                                       ; preds = %1
  %min.iters.check = icmp ult i32 %0, 4
  br i1 %min.iters.check, label %.lr.ph.preheader, label %vector.main.loop.iter.check

vector.main.loop.iter.check:                      ; preds = %iter.check
  %min.iters.check2 = icmp ult i32 %0, 32
  br i1 %min.iters.check2, label %vec.epilog.ph, label %vector.ph

vector.ph:                                        ; preds = %vector.main.loop.iter.check
  %n.vec = and i32 %0, 2147483616
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i32 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %vec.ind = phi <8 x i32> [ <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7>, %vector.ph ], [ %vec.ind.next, %vector.body ]
  %vec.phi = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %11, %vector.body ]
  %vec.phi3 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %12, %vector.body ]
  %vec.phi4 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %13, %vector.body ]
  %vec.phi5 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %14, %vector.body ]
  %3 = and <8 x i32> %vec.ind, splat (i32 1)
  %4 = and <8 x i32> %vec.ind, splat (i32 1)
  %5 = and <8 x i32> %vec.ind, splat (i32 1)
  %6 = and <8 x i32> %vec.ind, splat (i32 1)
  %7 = xor <8 x i32> %3, splat (i32 1)
  %8 = xor <8 x i32> %4, splat (i32 1)
  %9 = xor <8 x i32> %5, splat (i32 1)
  %10 = xor <8 x i32> %6, splat (i32 1)
  %11 = add <8 x i32> %7, %vec.phi
  %12 = add <8 x i32> %8, %vec.phi3
  %13 = add <8 x i32> %9, %vec.phi4
  %14 = add <8 x i32> %10, %vec.phi5
  %index.next = add nuw i32 %index, 32
  %vec.ind.next = add <8 x i32> %vec.ind, splat (i32 32)
  %15 = icmp eq i32 %index.next, %n.vec
  br i1 %15, label %middle.block, label %vector.body, !llvm.loop !1

middle.block:                                     ; preds = %vector.body
  %bin.rdx = add <8 x i32> %12, %11
  %bin.rdx6 = add <8 x i32> %13, %bin.rdx
  %bin.rdx7 = add <8 x i32> %14, %bin.rdx6
  %16 = tail call i32 @llvm.vector.reduce.add.v8i32(<8 x i32> %bin.rdx7)
  %cmp.n = icmp eq i32 %0, %n.vec
  br i1 %cmp.n, label %._crit_edge, label %vec.epilog.iter.check

vec.epilog.iter.check:                            ; preds = %middle.block
  %n.vec.remaining = and i32 %0, 28
  %min.epilog.iters.check = icmp eq i32 %n.vec.remaining, 0
  br i1 %min.epilog.iters.check, label %.lr.ph.preheader, label %vec.epilog.ph

vec.epilog.ph:                                    ; preds = %vec.epilog.iter.check, %vector.main.loop.iter.check
  %bc.resume.val = phi i32 [ %n.vec, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %bc.merge.rdx = phi i32 [ %16, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %n.vec9 = and i32 %0, 2147483644
  %17 = insertelement <4 x i32> <i32 poison, i32 0, i32 0, i32 0>, i32 %bc.merge.rdx, i64 0
  %broadcast.splatinsert = insertelement <4 x i32> poison, i32 %bc.resume.val, i64 0
  %broadcast.splat = shufflevector <4 x i32> %broadcast.splatinsert, <4 x i32> poison, <4 x i32> zeroinitializer
  %induction = or disjoint <4 x i32> %broadcast.splat, <i32 0, i32 1, i32 2, i32 3>
  br label %vec.epilog.vector.body

vec.epilog.vector.body:                           ; preds = %vec.epilog.vector.body, %vec.epilog.ph
  %index10 = phi i32 [ %bc.resume.val, %vec.epilog.ph ], [ %index.next13, %vec.epilog.vector.body ]
  %vec.ind11 = phi <4 x i32> [ %induction, %vec.epilog.ph ], [ %vec.ind.next14, %vec.epilog.vector.body ]
  %vec.phi12 = phi <4 x i32> [ %17, %vec.epilog.ph ], [ %20, %vec.epilog.vector.body ]
  %18 = and <4 x i32> %vec.ind11, splat (i32 1)
  %19 = xor <4 x i32> %18, splat (i32 1)
  %20 = add <4 x i32> %19, %vec.phi12
  %index.next13 = add nuw i32 %index10, 4
  %vec.ind.next14 = add <4 x i32> %vec.ind11, splat (i32 4)
  %21 = icmp eq i32 %index.next13, %n.vec9
  br i1 %21, label %vec.epilog.middle.block, label %vec.epilog.vector.body, !llvm.loop !4

vec.epilog.middle.block:                          ; preds = %vec.epilog.vector.body
  %22 = tail call i32 @llvm.vector.reduce.add.v4i32(<4 x i32> %20)
  %cmp.n15 = icmp eq i32 %0, %n.vec9
  br i1 %cmp.n15, label %._crit_edge, label %.lr.ph.preheader

.lr.ph.preheader:                                 ; preds = %vec.epilog.iter.check, %vec.epilog.middle.block, %iter.check
  %.ph = phi i32 [ 0, %iter.check ], [ %n.vec, %vec.epilog.iter.check ], [ %n.vec9, %vec.epilog.middle.block ]
  %.ph18 = phi i32 [ 0, %iter.check ], [ %16, %vec.epilog.iter.check ], [ %22, %vec.epilog.middle.block ]
  br label %.lr.ph

.lr.ph:                                           ; preds = %.lr.ph.preheader, %.lr.ph
  %23 = phi i32 [ %27, %.lr.ph ], [ %.ph, %.lr.ph.preheader ]
  %24 = phi i32 [ %spec.select, %.lr.ph ], [ %.ph18, %.lr.ph.preheader ]
  %25 = and i32 %23, 1
  %26 = xor i32 %25, 1
  %spec.select = add i32 %26, %24
  %27 = add nuw nsw i32 %23, 1
  %exitcond.not = icmp eq i32 %27, %0
  br i1 %exitcond.not, label %._crit_edge, label %.lr.ph, !llvm.loop !5

._crit_edge:                                      ; preds = %.lr.ph, %middle.block, %vec.epilog.middle.block, %1
  %.lcssa = phi i32 [ 0, %1 ], [ %16, %middle.block ], [ %22, %vec.epilog.middle.block ], [ %spec.select, %.lr.ph ]
  ret i32 %.lcssa
}

; Function Attrs: nofree norecurse nosync nounwind memory(none)
define signext i32 @_mlir_ciface_execute(i32 %0) local_unnamed_addr #1 {
  %2 = icmp sgt i32 %0, 0
  br i1 %2, label %iter.check, label %execute.exit

iter.check:                                       ; preds = %1
  %min.iters.check = icmp ult i32 %0, 4
  br i1 %min.iters.check, label %.lr.ph.i.preheader, label %vector.main.loop.iter.check

vector.main.loop.iter.check:                      ; preds = %iter.check
  %min.iters.check1 = icmp ult i32 %0, 32
  br i1 %min.iters.check1, label %vec.epilog.ph, label %vector.ph

vector.ph:                                        ; preds = %vector.main.loop.iter.check
  %n.vec = and i32 %0, 2147483616
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i32 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %vec.ind = phi <8 x i32> [ <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7>, %vector.ph ], [ %vec.ind.next, %vector.body ]
  %vec.phi = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %11, %vector.body ]
  %vec.phi2 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %12, %vector.body ]
  %vec.phi3 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %13, %vector.body ]
  %vec.phi4 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %14, %vector.body ]
  %3 = and <8 x i32> %vec.ind, splat (i32 1)
  %4 = and <8 x i32> %vec.ind, splat (i32 1)
  %5 = and <8 x i32> %vec.ind, splat (i32 1)
  %6 = and <8 x i32> %vec.ind, splat (i32 1)
  %7 = xor <8 x i32> %3, splat (i32 1)
  %8 = xor <8 x i32> %4, splat (i32 1)
  %9 = xor <8 x i32> %5, splat (i32 1)
  %10 = xor <8 x i32> %6, splat (i32 1)
  %11 = add <8 x i32> %7, %vec.phi
  %12 = add <8 x i32> %8, %vec.phi2
  %13 = add <8 x i32> %9, %vec.phi3
  %14 = add <8 x i32> %10, %vec.phi4
  %index.next = add nuw i32 %index, 32
  %vec.ind.next = add <8 x i32> %vec.ind, splat (i32 32)
  %15 = icmp eq i32 %index.next, %n.vec
  br i1 %15, label %middle.block, label %vector.body, !llvm.loop !6

middle.block:                                     ; preds = %vector.body
  %bin.rdx = add <8 x i32> %12, %11
  %bin.rdx5 = add <8 x i32> %13, %bin.rdx
  %bin.rdx6 = add <8 x i32> %14, %bin.rdx5
  %16 = tail call i32 @llvm.vector.reduce.add.v8i32(<8 x i32> %bin.rdx6)
  %cmp.n = icmp eq i32 %0, %n.vec
  br i1 %cmp.n, label %execute.exit, label %vec.epilog.iter.check

vec.epilog.iter.check:                            ; preds = %middle.block
  %n.vec.remaining = and i32 %0, 28
  %min.epilog.iters.check = icmp eq i32 %n.vec.remaining, 0
  br i1 %min.epilog.iters.check, label %.lr.ph.i.preheader, label %vec.epilog.ph

vec.epilog.ph:                                    ; preds = %vec.epilog.iter.check, %vector.main.loop.iter.check
  %bc.resume.val = phi i32 [ %n.vec, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %bc.merge.rdx = phi i32 [ %16, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %n.vec8 = and i32 %0, 2147483644
  %17 = insertelement <4 x i32> <i32 poison, i32 0, i32 0, i32 0>, i32 %bc.merge.rdx, i64 0
  %broadcast.splatinsert = insertelement <4 x i32> poison, i32 %bc.resume.val, i64 0
  %broadcast.splat = shufflevector <4 x i32> %broadcast.splatinsert, <4 x i32> poison, <4 x i32> zeroinitializer
  %induction = or disjoint <4 x i32> %broadcast.splat, <i32 0, i32 1, i32 2, i32 3>
  br label %vec.epilog.vector.body

vec.epilog.vector.body:                           ; preds = %vec.epilog.vector.body, %vec.epilog.ph
  %index9 = phi i32 [ %bc.resume.val, %vec.epilog.ph ], [ %index.next12, %vec.epilog.vector.body ]
  %vec.ind10 = phi <4 x i32> [ %induction, %vec.epilog.ph ], [ %vec.ind.next13, %vec.epilog.vector.body ]
  %vec.phi11 = phi <4 x i32> [ %17, %vec.epilog.ph ], [ %20, %vec.epilog.vector.body ]
  %18 = and <4 x i32> %vec.ind10, splat (i32 1)
  %19 = xor <4 x i32> %18, splat (i32 1)
  %20 = add <4 x i32> %19, %vec.phi11
  %index.next12 = add nuw i32 %index9, 4
  %vec.ind.next13 = add <4 x i32> %vec.ind10, splat (i32 4)
  %21 = icmp eq i32 %index.next12, %n.vec8
  br i1 %21, label %vec.epilog.middle.block, label %vec.epilog.vector.body, !llvm.loop !7

vec.epilog.middle.block:                          ; preds = %vec.epilog.vector.body
  %22 = tail call i32 @llvm.vector.reduce.add.v4i32(<4 x i32> %20)
  %cmp.n14 = icmp eq i32 %0, %n.vec8
  br i1 %cmp.n14, label %execute.exit, label %.lr.ph.i.preheader

.lr.ph.i.preheader:                               ; preds = %vec.epilog.iter.check, %vec.epilog.middle.block, %iter.check
  %.ph = phi i32 [ 0, %iter.check ], [ %n.vec, %vec.epilog.iter.check ], [ %n.vec8, %vec.epilog.middle.block ]
  %.ph17 = phi i32 [ 0, %iter.check ], [ %16, %vec.epilog.iter.check ], [ %22, %vec.epilog.middle.block ]
  br label %.lr.ph.i

.lr.ph.i:                                         ; preds = %.lr.ph.i.preheader, %.lr.ph.i
  %23 = phi i32 [ %27, %.lr.ph.i ], [ %.ph, %.lr.ph.i.preheader ]
  %24 = phi i32 [ %spec.select.i, %.lr.ph.i ], [ %.ph17, %.lr.ph.i.preheader ]
  %25 = and i32 %23, 1
  %26 = xor i32 %25, 1
  %spec.select.i = add i32 %26, %24
  %27 = add nuw nsw i32 %23, 1
  %exitcond.not.i = icmp eq i32 %27, %0
  br i1 %exitcond.not.i, label %execute.exit, label %.lr.ph.i, !llvm.loop !8

execute.exit:                                     ; preds = %.lr.ph.i, %middle.block, %vec.epilog.middle.block, %1
  %.lcssa.i = phi i32 [ 0, %1 ], [ %16, %middle.block ], [ %22, %vec.epilog.middle.block ], [ %spec.select.i, %.lr.ph.i ]
  ret i32 %.lcssa.i
}

; Function Attrs: nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: none)
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = icmp sgt i32 %3, 0
  br i1 %4, label %iter.check, label %execute.exit

iter.check:                                       ; preds = %1
  %min.iters.check = icmp ult i32 %3, 4
  br i1 %min.iters.check, label %.lr.ph.i.preheader, label %vector.main.loop.iter.check

vector.main.loop.iter.check:                      ; preds = %iter.check
  %min.iters.check1 = icmp ult i32 %3, 32
  br i1 %min.iters.check1, label %vec.epilog.ph, label %vector.ph

vector.ph:                                        ; preds = %vector.main.loop.iter.check
  %n.vec = and i32 %3, 2147483616
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i32 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %vec.ind = phi <8 x i32> [ <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7>, %vector.ph ], [ %vec.ind.next, %vector.body ]
  %vec.phi = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %13, %vector.body ]
  %vec.phi2 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %14, %vector.body ]
  %vec.phi3 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %15, %vector.body ]
  %vec.phi4 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %16, %vector.body ]
  %5 = and <8 x i32> %vec.ind, splat (i32 1)
  %6 = and <8 x i32> %vec.ind, splat (i32 1)
  %7 = and <8 x i32> %vec.ind, splat (i32 1)
  %8 = and <8 x i32> %vec.ind, splat (i32 1)
  %9 = xor <8 x i32> %5, splat (i32 1)
  %10 = xor <8 x i32> %6, splat (i32 1)
  %11 = xor <8 x i32> %7, splat (i32 1)
  %12 = xor <8 x i32> %8, splat (i32 1)
  %13 = add <8 x i32> %9, %vec.phi
  %14 = add <8 x i32> %10, %vec.phi2
  %15 = add <8 x i32> %11, %vec.phi3
  %16 = add <8 x i32> %12, %vec.phi4
  %index.next = add nuw i32 %index, 32
  %vec.ind.next = add <8 x i32> %vec.ind, splat (i32 32)
  %17 = icmp eq i32 %index.next, %n.vec
  br i1 %17, label %middle.block, label %vector.body, !llvm.loop !9

middle.block:                                     ; preds = %vector.body
  %bin.rdx = add <8 x i32> %14, %13
  %bin.rdx5 = add <8 x i32> %15, %bin.rdx
  %bin.rdx6 = add <8 x i32> %16, %bin.rdx5
  %18 = tail call i32 @llvm.vector.reduce.add.v8i32(<8 x i32> %bin.rdx6)
  %cmp.n = icmp eq i32 %3, %n.vec
  br i1 %cmp.n, label %execute.exit, label %vec.epilog.iter.check

vec.epilog.iter.check:                            ; preds = %middle.block
  %n.vec.remaining = and i32 %3, 28
  %min.epilog.iters.check = icmp eq i32 %n.vec.remaining, 0
  br i1 %min.epilog.iters.check, label %.lr.ph.i.preheader, label %vec.epilog.ph

vec.epilog.ph:                                    ; preds = %vec.epilog.iter.check, %vector.main.loop.iter.check
  %bc.resume.val = phi i32 [ %n.vec, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %bc.merge.rdx = phi i32 [ %18, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %n.vec8 = and i32 %3, 2147483644
  %19 = insertelement <4 x i32> <i32 poison, i32 0, i32 0, i32 0>, i32 %bc.merge.rdx, i64 0
  %broadcast.splatinsert = insertelement <4 x i32> poison, i32 %bc.resume.val, i64 0
  %broadcast.splat = shufflevector <4 x i32> %broadcast.splatinsert, <4 x i32> poison, <4 x i32> zeroinitializer
  %induction = or disjoint <4 x i32> %broadcast.splat, <i32 0, i32 1, i32 2, i32 3>
  br label %vec.epilog.vector.body

vec.epilog.vector.body:                           ; preds = %vec.epilog.vector.body, %vec.epilog.ph
  %index9 = phi i32 [ %bc.resume.val, %vec.epilog.ph ], [ %index.next12, %vec.epilog.vector.body ]
  %vec.ind10 = phi <4 x i32> [ %induction, %vec.epilog.ph ], [ %vec.ind.next13, %vec.epilog.vector.body ]
  %vec.phi11 = phi <4 x i32> [ %19, %vec.epilog.ph ], [ %22, %vec.epilog.vector.body ]
  %20 = and <4 x i32> %vec.ind10, splat (i32 1)
  %21 = xor <4 x i32> %20, splat (i32 1)
  %22 = add <4 x i32> %21, %vec.phi11
  %index.next12 = add nuw i32 %index9, 4
  %vec.ind.next13 = add <4 x i32> %vec.ind10, splat (i32 4)
  %23 = icmp eq i32 %index.next12, %n.vec8
  br i1 %23, label %vec.epilog.middle.block, label %vec.epilog.vector.body, !llvm.loop !10

vec.epilog.middle.block:                          ; preds = %vec.epilog.vector.body
  %24 = tail call i32 @llvm.vector.reduce.add.v4i32(<4 x i32> %22)
  %cmp.n14 = icmp eq i32 %3, %n.vec8
  br i1 %cmp.n14, label %execute.exit, label %.lr.ph.i.preheader

.lr.ph.i.preheader:                               ; preds = %vec.epilog.iter.check, %vec.epilog.middle.block, %iter.check
  %.ph = phi i32 [ 0, %iter.check ], [ %n.vec, %vec.epilog.iter.check ], [ %n.vec8, %vec.epilog.middle.block ]
  %.ph17 = phi i32 [ 0, %iter.check ], [ %18, %vec.epilog.iter.check ], [ %24, %vec.epilog.middle.block ]
  br label %.lr.ph.i

.lr.ph.i:                                         ; preds = %.lr.ph.i.preheader, %.lr.ph.i
  %25 = phi i32 [ %29, %.lr.ph.i ], [ %.ph, %.lr.ph.i.preheader ]
  %26 = phi i32 [ %spec.select.i, %.lr.ph.i ], [ %.ph17, %.lr.ph.i.preheader ]
  %27 = and i32 %25, 1
  %28 = xor i32 %27, 1
  %spec.select.i = add i32 %28, %26
  %29 = add nuw nsw i32 %25, 1
  %exitcond.not.i = icmp eq i32 %29, %3
  br i1 %exitcond.not.i, label %execute.exit, label %.lr.ph.i, !llvm.loop !11

execute.exit:                                     ; preds = %.lr.ph.i, %middle.block, %vec.epilog.middle.block, %1
  %.lcssa.i = phi i32 [ 0, %1 ], [ %18, %middle.block ], [ %24, %vec.epilog.middle.block ], [ %spec.select.i, %.lr.ph.i ]
  %30 = getelementptr i8, ptr %0, i64 8
  %31 = load ptr, ptr %30, align 8
  store i32 %.lcssa.i, ptr %31, align 4
  ret void
}

; Function Attrs: nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: none)
define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = icmp sgt i32 %3, 0
  br i1 %4, label %iter.check, label %_mlir_ciface_execute.exit

iter.check:                                       ; preds = %1
  %min.iters.check = icmp ult i32 %3, 4
  br i1 %min.iters.check, label %.lr.ph.i.i.preheader, label %vector.main.loop.iter.check

vector.main.loop.iter.check:                      ; preds = %iter.check
  %min.iters.check1 = icmp ult i32 %3, 32
  br i1 %min.iters.check1, label %vec.epilog.ph, label %vector.ph

vector.ph:                                        ; preds = %vector.main.loop.iter.check
  %n.vec = and i32 %3, 2147483616
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i32 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %vec.ind = phi <8 x i32> [ <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7>, %vector.ph ], [ %vec.ind.next, %vector.body ]
  %vec.phi = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %13, %vector.body ]
  %vec.phi2 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %14, %vector.body ]
  %vec.phi3 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %15, %vector.body ]
  %vec.phi4 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %16, %vector.body ]
  %5 = and <8 x i32> %vec.ind, splat (i32 1)
  %6 = and <8 x i32> %vec.ind, splat (i32 1)
  %7 = and <8 x i32> %vec.ind, splat (i32 1)
  %8 = and <8 x i32> %vec.ind, splat (i32 1)
  %9 = xor <8 x i32> %5, splat (i32 1)
  %10 = xor <8 x i32> %6, splat (i32 1)
  %11 = xor <8 x i32> %7, splat (i32 1)
  %12 = xor <8 x i32> %8, splat (i32 1)
  %13 = add <8 x i32> %9, %vec.phi
  %14 = add <8 x i32> %10, %vec.phi2
  %15 = add <8 x i32> %11, %vec.phi3
  %16 = add <8 x i32> %12, %vec.phi4
  %index.next = add nuw i32 %index, 32
  %vec.ind.next = add <8 x i32> %vec.ind, splat (i32 32)
  %17 = icmp eq i32 %index.next, %n.vec
  br i1 %17, label %middle.block, label %vector.body, !llvm.loop !12

middle.block:                                     ; preds = %vector.body
  %bin.rdx = add <8 x i32> %14, %13
  %bin.rdx5 = add <8 x i32> %15, %bin.rdx
  %bin.rdx6 = add <8 x i32> %16, %bin.rdx5
  %18 = tail call i32 @llvm.vector.reduce.add.v8i32(<8 x i32> %bin.rdx6)
  %cmp.n = icmp eq i32 %3, %n.vec
  br i1 %cmp.n, label %_mlir_ciface_execute.exit, label %vec.epilog.iter.check

vec.epilog.iter.check:                            ; preds = %middle.block
  %n.vec.remaining = and i32 %3, 28
  %min.epilog.iters.check = icmp eq i32 %n.vec.remaining, 0
  br i1 %min.epilog.iters.check, label %.lr.ph.i.i.preheader, label %vec.epilog.ph

vec.epilog.ph:                                    ; preds = %vec.epilog.iter.check, %vector.main.loop.iter.check
  %bc.resume.val = phi i32 [ %n.vec, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %bc.merge.rdx = phi i32 [ %18, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %n.vec8 = and i32 %3, 2147483644
  %19 = insertelement <4 x i32> <i32 poison, i32 0, i32 0, i32 0>, i32 %bc.merge.rdx, i64 0
  %broadcast.splatinsert = insertelement <4 x i32> poison, i32 %bc.resume.val, i64 0
  %broadcast.splat = shufflevector <4 x i32> %broadcast.splatinsert, <4 x i32> poison, <4 x i32> zeroinitializer
  %induction = or disjoint <4 x i32> %broadcast.splat, <i32 0, i32 1, i32 2, i32 3>
  br label %vec.epilog.vector.body

vec.epilog.vector.body:                           ; preds = %vec.epilog.vector.body, %vec.epilog.ph
  %index9 = phi i32 [ %bc.resume.val, %vec.epilog.ph ], [ %index.next12, %vec.epilog.vector.body ]
  %vec.ind10 = phi <4 x i32> [ %induction, %vec.epilog.ph ], [ %vec.ind.next13, %vec.epilog.vector.body ]
  %vec.phi11 = phi <4 x i32> [ %19, %vec.epilog.ph ], [ %22, %vec.epilog.vector.body ]
  %20 = and <4 x i32> %vec.ind10, splat (i32 1)
  %21 = xor <4 x i32> %20, splat (i32 1)
  %22 = add <4 x i32> %21, %vec.phi11
  %index.next12 = add nuw i32 %index9, 4
  %vec.ind.next13 = add <4 x i32> %vec.ind10, splat (i32 4)
  %23 = icmp eq i32 %index.next12, %n.vec8
  br i1 %23, label %vec.epilog.middle.block, label %vec.epilog.vector.body, !llvm.loop !13

vec.epilog.middle.block:                          ; preds = %vec.epilog.vector.body
  %24 = tail call i32 @llvm.vector.reduce.add.v4i32(<4 x i32> %22)
  %cmp.n14 = icmp eq i32 %3, %n.vec8
  br i1 %cmp.n14, label %_mlir_ciface_execute.exit, label %.lr.ph.i.i.preheader

.lr.ph.i.i.preheader:                             ; preds = %vec.epilog.iter.check, %vec.epilog.middle.block, %iter.check
  %.ph = phi i32 [ 0, %iter.check ], [ %n.vec, %vec.epilog.iter.check ], [ %n.vec8, %vec.epilog.middle.block ]
  %.ph17 = phi i32 [ 0, %iter.check ], [ %18, %vec.epilog.iter.check ], [ %24, %vec.epilog.middle.block ]
  br label %.lr.ph.i.i

.lr.ph.i.i:                                       ; preds = %.lr.ph.i.i.preheader, %.lr.ph.i.i
  %25 = phi i32 [ %29, %.lr.ph.i.i ], [ %.ph, %.lr.ph.i.i.preheader ]
  %26 = phi i32 [ %spec.select.i.i, %.lr.ph.i.i ], [ %.ph17, %.lr.ph.i.i.preheader ]
  %27 = and i32 %25, 1
  %28 = xor i32 %27, 1
  %spec.select.i.i = add i32 %28, %26
  %29 = add nuw nsw i32 %25, 1
  %exitcond.not.i.i = icmp eq i32 %29, %3
  br i1 %exitcond.not.i.i, label %_mlir_ciface_execute.exit, label %.lr.ph.i.i, !llvm.loop !14

_mlir_ciface_execute.exit:                        ; preds = %.lr.ph.i.i, %middle.block, %vec.epilog.middle.block, %1
  %.lcssa.i.i = phi i32 [ 0, %1 ], [ %18, %middle.block ], [ %24, %vec.epilog.middle.block ], [ %spec.select.i.i, %.lr.ph.i.i ]
  %30 = getelementptr i8, ptr %0, i64 8
  %31 = load ptr, ptr %30, align 8
  store i32 %.lcssa.i.i, ptr %31, align 4
  ret void
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i32 @llvm.vector.reduce.add.v8i32(<8 x i32>) #3

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i32 @llvm.vector.reduce.add.v4i32(<4 x i32>) #3

attributes #0 = { nofree norecurse nosync nounwind memory(none) }
attributes #1 = { nofree norecurse nosync nounwind memory(none) }
attributes #2 = { nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: none) }
attributes #3 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
!1 = distinct !{!1, !2, !3}
!2 = !{!"llvm.loop.isvectorized", i32 1}
!3 = !{!"llvm.loop.unroll.runtime.disable"}
!4 = distinct !{!4, !2, !3}
!5 = distinct !{!5, !3, !2}
!6 = distinct !{!6, !2, !3}
!7 = distinct !{!7, !2, !3}
!8 = distinct !{!8, !3, !2}
!9 = distinct !{!9, !2, !3}
!10 = distinct !{!10, !2, !3}
!11 = distinct !{!11, !3, !2}
!12 = distinct !{!12, !2, !3}
!13 = distinct !{!13, !2, !3}
!14 = distinct !{!14, !3, !2}
