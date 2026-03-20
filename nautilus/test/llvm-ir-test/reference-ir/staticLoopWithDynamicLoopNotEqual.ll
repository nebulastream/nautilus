; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nofree norecurse nosync nounwind memory(none)
define signext i32 @execute(i32 %0) local_unnamed_addr #0 {
  %.not3 = icmp eq i32 %0, 0
  br i1 %.not3, label %._crit_edge, label %iter.check

iter.check:                                       ; preds = %1
  %min.iters.check = icmp ult i32 %0, 8
  br i1 %min.iters.check, label %.lr.ph.preheader, label %vector.main.loop.iter.check

vector.main.loop.iter.check:                      ; preds = %iter.check
  %min.iters.check7 = icmp ult i32 %0, 32
  br i1 %min.iters.check7, label %vec.epilog.ph, label %vector.ph

vector.ph:                                        ; preds = %vector.main.loop.iter.check
  %n.vec = and i32 %0, -32
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i32 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %vec.ind = phi <8 x i32> [ <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7>, %vector.ph ], [ %vec.ind.next, %vector.body ]
  %vec.phi = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %22, %vector.body ]
  %vec.phi8 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %23, %vector.body ]
  %vec.phi9 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %24, %vector.body ]
  %vec.phi10 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %25, %vector.body ]
  %step.add = add <8 x i32> %vec.ind, splat (i32 8)
  %step.add.2 = add <8 x i32> %vec.ind, splat (i32 16)
  %step.add.3 = add <8 x i32> %vec.ind, splat (i32 24)
  %2 = icmp sgt <8 x i32> %vec.ind, splat (i32 5)
  %3 = icmp sgt <8 x i32> %step.add, splat (i32 5)
  %4 = icmp sgt <8 x i32> %step.add.2, splat (i32 5)
  %5 = icmp sgt <8 x i32> %step.add.3, splat (i32 5)
  %6 = zext <8 x i1> %2 to <8 x i32>
  %7 = zext <8 x i1> %3 to <8 x i32>
  %8 = zext <8 x i1> %4 to <8 x i32>
  %9 = zext <8 x i1> %5 to <8 x i32>
  %10 = select <8 x i1> %2, <8 x i32> splat (i32 22), <8 x i32> splat (i32 20)
  %11 = select <8 x i1> %3, <8 x i32> splat (i32 22), <8 x i32> splat (i32 20)
  %12 = select <8 x i1> %4, <8 x i32> splat (i32 22), <8 x i32> splat (i32 20)
  %13 = select <8 x i1> %5, <8 x i32> splat (i32 22), <8 x i32> splat (i32 20)
  %14 = add <8 x i32> %vec.phi, splat (i32 10)
  %15 = add <8 x i32> %vec.phi8, splat (i32 10)
  %16 = add <8 x i32> %vec.phi9, splat (i32 10)
  %17 = add <8 x i32> %vec.phi10, splat (i32 10)
  %18 = add <8 x i32> %14, %6
  %19 = add <8 x i32> %15, %7
  %20 = add <8 x i32> %16, %8
  %21 = add <8 x i32> %17, %9
  %22 = add <8 x i32> %18, %10
  %23 = add <8 x i32> %19, %11
  %24 = add <8 x i32> %20, %12
  %25 = add <8 x i32> %21, %13
  %index.next = add nuw i32 %index, 32
  %vec.ind.next = add <8 x i32> %vec.ind, splat (i32 32)
  %26 = icmp eq i32 %index.next, %n.vec
  br i1 %26, label %middle.block, label %vector.body, !llvm.loop !1

middle.block:                                     ; preds = %vector.body
  %bin.rdx = add <8 x i32> %23, %22
  %bin.rdx11 = add <8 x i32> %24, %bin.rdx
  %bin.rdx12 = add <8 x i32> %25, %bin.rdx11
  %27 = tail call i32 @llvm.vector.reduce.add.v8i32(<8 x i32> %bin.rdx12)
  %cmp.n = icmp eq i32 %0, %n.vec
  br i1 %cmp.n, label %._crit_edge, label %vec.epilog.iter.check

vec.epilog.iter.check:                            ; preds = %middle.block
  %n.vec.remaining = and i32 %0, 24
  %min.epilog.iters.check = icmp eq i32 %n.vec.remaining, 0
  br i1 %min.epilog.iters.check, label %.lr.ph.preheader, label %vec.epilog.ph

vec.epilog.ph:                                    ; preds = %vec.epilog.iter.check, %vector.main.loop.iter.check
  %bc.resume.val = phi i32 [ %n.vec, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %bc.merge.rdx = phi i32 [ %27, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %n.vec14 = and i32 %0, -8
  %28 = insertelement <8 x i32> <i32 poison, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0>, i32 %bc.merge.rdx, i64 0
  %broadcast.splatinsert = insertelement <8 x i32> poison, i32 %bc.resume.val, i64 0
  %broadcast.splat = shufflevector <8 x i32> %broadcast.splatinsert, <8 x i32> poison, <8 x i32> zeroinitializer
  %induction = or disjoint <8 x i32> %broadcast.splat, <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7>
  br label %vec.epilog.vector.body

vec.epilog.vector.body:                           ; preds = %vec.epilog.vector.body, %vec.epilog.ph
  %index15 = phi i32 [ %bc.resume.val, %vec.epilog.ph ], [ %index.next18, %vec.epilog.vector.body ]
  %vec.ind16 = phi <8 x i32> [ %induction, %vec.epilog.ph ], [ %vec.ind.next19, %vec.epilog.vector.body ]
  %vec.phi17 = phi <8 x i32> [ %28, %vec.epilog.ph ], [ %34, %vec.epilog.vector.body ]
  %29 = icmp sgt <8 x i32> %vec.ind16, splat (i32 5)
  %30 = zext <8 x i1> %29 to <8 x i32>
  %31 = select <8 x i1> %29, <8 x i32> splat (i32 22), <8 x i32> splat (i32 20)
  %32 = add <8 x i32> %vec.phi17, splat (i32 10)
  %33 = add <8 x i32> %32, %30
  %34 = add <8 x i32> %33, %31
  %index.next18 = add nuw i32 %index15, 8
  %vec.ind.next19 = add <8 x i32> %vec.ind16, splat (i32 8)
  %35 = icmp eq i32 %index.next18, %n.vec14
  br i1 %35, label %vec.epilog.middle.block, label %vec.epilog.vector.body, !llvm.loop !4

vec.epilog.middle.block:                          ; preds = %vec.epilog.vector.body
  %36 = tail call i32 @llvm.vector.reduce.add.v8i32(<8 x i32> %34)
  %cmp.n20 = icmp eq i32 %0, %n.vec14
  br i1 %cmp.n20, label %._crit_edge, label %.lr.ph.preheader

.lr.ph.preheader:                                 ; preds = %vec.epilog.iter.check, %vec.epilog.middle.block, %iter.check
  %.ph = phi i32 [ 0, %iter.check ], [ %n.vec, %vec.epilog.iter.check ], [ %n.vec14, %vec.epilog.middle.block ]
  %.ph23 = phi i32 [ 0, %iter.check ], [ %27, %vec.epilog.iter.check ], [ %36, %vec.epilog.middle.block ]
  br label %.lr.ph

.lr.ph:                                           ; preds = %.lr.ph.preheader, %.lr.ph
  %37 = phi i32 [ %43, %.lr.ph ], [ %.ph, %.lr.ph.preheader ]
  %38 = phi i32 [ %42, %.lr.ph ], [ %.ph23, %.lr.ph.preheader ]
  %39 = icmp sgt i32 %37, 5
  %40 = zext i1 %39 to i32
  %.v = select i1 %39, i32 22, i32 20
  %spec.select = add i32 %38, 10
  %41 = add i32 %spec.select, %40
  %42 = add i32 %41, %.v
  %43 = add nuw i32 %37, 1
  %.not = icmp eq i32 %43, %0
  br i1 %.not, label %._crit_edge, label %.lr.ph, !llvm.loop !5

._crit_edge:                                      ; preds = %.lr.ph, %middle.block, %vec.epilog.middle.block, %1
  %.lcssa = phi i32 [ 0, %1 ], [ %27, %middle.block ], [ %36, %vec.epilog.middle.block ], [ %42, %.lr.ph ]
  ret i32 %.lcssa
}

; Function Attrs: nofree norecurse nosync nounwind memory(none)
define signext i32 @_mlir_ciface_execute(i32 %0) local_unnamed_addr #1 {
  %.not3.i = icmp eq i32 %0, 0
  br i1 %.not3.i, label %execute.exit, label %iter.check

iter.check:                                       ; preds = %1
  %min.iters.check = icmp ult i32 %0, 8
  br i1 %min.iters.check, label %.lr.ph.i.preheader, label %vector.main.loop.iter.check

vector.main.loop.iter.check:                      ; preds = %iter.check
  %min.iters.check1 = icmp ult i32 %0, 32
  br i1 %min.iters.check1, label %vec.epilog.ph, label %vector.ph

vector.ph:                                        ; preds = %vector.main.loop.iter.check
  %n.vec = and i32 %0, -32
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i32 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %vec.ind = phi <8 x i32> [ <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7>, %vector.ph ], [ %vec.ind.next, %vector.body ]
  %vec.phi = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %22, %vector.body ]
  %vec.phi2 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %23, %vector.body ]
  %vec.phi3 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %24, %vector.body ]
  %vec.phi4 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %25, %vector.body ]
  %step.add = add <8 x i32> %vec.ind, splat (i32 8)
  %step.add.2 = add <8 x i32> %vec.ind, splat (i32 16)
  %step.add.3 = add <8 x i32> %vec.ind, splat (i32 24)
  %2 = icmp sgt <8 x i32> %vec.ind, splat (i32 5)
  %3 = icmp sgt <8 x i32> %step.add, splat (i32 5)
  %4 = icmp sgt <8 x i32> %step.add.2, splat (i32 5)
  %5 = icmp sgt <8 x i32> %step.add.3, splat (i32 5)
  %6 = zext <8 x i1> %2 to <8 x i32>
  %7 = zext <8 x i1> %3 to <8 x i32>
  %8 = zext <8 x i1> %4 to <8 x i32>
  %9 = zext <8 x i1> %5 to <8 x i32>
  %10 = select <8 x i1> %2, <8 x i32> splat (i32 22), <8 x i32> splat (i32 20)
  %11 = select <8 x i1> %3, <8 x i32> splat (i32 22), <8 x i32> splat (i32 20)
  %12 = select <8 x i1> %4, <8 x i32> splat (i32 22), <8 x i32> splat (i32 20)
  %13 = select <8 x i1> %5, <8 x i32> splat (i32 22), <8 x i32> splat (i32 20)
  %14 = add <8 x i32> %vec.phi, splat (i32 10)
  %15 = add <8 x i32> %vec.phi2, splat (i32 10)
  %16 = add <8 x i32> %vec.phi3, splat (i32 10)
  %17 = add <8 x i32> %vec.phi4, splat (i32 10)
  %18 = add <8 x i32> %14, %6
  %19 = add <8 x i32> %15, %7
  %20 = add <8 x i32> %16, %8
  %21 = add <8 x i32> %17, %9
  %22 = add <8 x i32> %18, %10
  %23 = add <8 x i32> %19, %11
  %24 = add <8 x i32> %20, %12
  %25 = add <8 x i32> %21, %13
  %index.next = add nuw i32 %index, 32
  %vec.ind.next = add <8 x i32> %vec.ind, splat (i32 32)
  %26 = icmp eq i32 %index.next, %n.vec
  br i1 %26, label %middle.block, label %vector.body, !llvm.loop !6

middle.block:                                     ; preds = %vector.body
  %bin.rdx = add <8 x i32> %23, %22
  %bin.rdx5 = add <8 x i32> %24, %bin.rdx
  %bin.rdx6 = add <8 x i32> %25, %bin.rdx5
  %27 = tail call i32 @llvm.vector.reduce.add.v8i32(<8 x i32> %bin.rdx6)
  %cmp.n = icmp eq i32 %0, %n.vec
  br i1 %cmp.n, label %execute.exit, label %vec.epilog.iter.check

vec.epilog.iter.check:                            ; preds = %middle.block
  %n.vec.remaining = and i32 %0, 24
  %min.epilog.iters.check = icmp eq i32 %n.vec.remaining, 0
  br i1 %min.epilog.iters.check, label %.lr.ph.i.preheader, label %vec.epilog.ph

vec.epilog.ph:                                    ; preds = %vec.epilog.iter.check, %vector.main.loop.iter.check
  %bc.resume.val = phi i32 [ %n.vec, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %bc.merge.rdx = phi i32 [ %27, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %n.vec8 = and i32 %0, -8
  %28 = insertelement <8 x i32> <i32 poison, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0>, i32 %bc.merge.rdx, i64 0
  %broadcast.splatinsert = insertelement <8 x i32> poison, i32 %bc.resume.val, i64 0
  %broadcast.splat = shufflevector <8 x i32> %broadcast.splatinsert, <8 x i32> poison, <8 x i32> zeroinitializer
  %induction = or disjoint <8 x i32> %broadcast.splat, <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7>
  br label %vec.epilog.vector.body

vec.epilog.vector.body:                           ; preds = %vec.epilog.vector.body, %vec.epilog.ph
  %index9 = phi i32 [ %bc.resume.val, %vec.epilog.ph ], [ %index.next12, %vec.epilog.vector.body ]
  %vec.ind10 = phi <8 x i32> [ %induction, %vec.epilog.ph ], [ %vec.ind.next13, %vec.epilog.vector.body ]
  %vec.phi11 = phi <8 x i32> [ %28, %vec.epilog.ph ], [ %34, %vec.epilog.vector.body ]
  %29 = icmp sgt <8 x i32> %vec.ind10, splat (i32 5)
  %30 = zext <8 x i1> %29 to <8 x i32>
  %31 = select <8 x i1> %29, <8 x i32> splat (i32 22), <8 x i32> splat (i32 20)
  %32 = add <8 x i32> %vec.phi11, splat (i32 10)
  %33 = add <8 x i32> %32, %30
  %34 = add <8 x i32> %33, %31
  %index.next12 = add nuw i32 %index9, 8
  %vec.ind.next13 = add <8 x i32> %vec.ind10, splat (i32 8)
  %35 = icmp eq i32 %index.next12, %n.vec8
  br i1 %35, label %vec.epilog.middle.block, label %vec.epilog.vector.body, !llvm.loop !7

vec.epilog.middle.block:                          ; preds = %vec.epilog.vector.body
  %36 = tail call i32 @llvm.vector.reduce.add.v8i32(<8 x i32> %34)
  %cmp.n14 = icmp eq i32 %0, %n.vec8
  br i1 %cmp.n14, label %execute.exit, label %.lr.ph.i.preheader

.lr.ph.i.preheader:                               ; preds = %vec.epilog.iter.check, %vec.epilog.middle.block, %iter.check
  %.ph = phi i32 [ 0, %iter.check ], [ %n.vec, %vec.epilog.iter.check ], [ %n.vec8, %vec.epilog.middle.block ]
  %.ph17 = phi i32 [ 0, %iter.check ], [ %27, %vec.epilog.iter.check ], [ %36, %vec.epilog.middle.block ]
  br label %.lr.ph.i

.lr.ph.i:                                         ; preds = %.lr.ph.i.preheader, %.lr.ph.i
  %37 = phi i32 [ %43, %.lr.ph.i ], [ %.ph, %.lr.ph.i.preheader ]
  %38 = phi i32 [ %42, %.lr.ph.i ], [ %.ph17, %.lr.ph.i.preheader ]
  %39 = icmp sgt i32 %37, 5
  %40 = zext i1 %39 to i32
  %.v.i = select i1 %39, i32 22, i32 20
  %spec.select.i = add i32 %38, 10
  %41 = add i32 %spec.select.i, %40
  %42 = add i32 %41, %.v.i
  %43 = add nuw i32 %37, 1
  %.not.i = icmp eq i32 %43, %0
  br i1 %.not.i, label %execute.exit, label %.lr.ph.i, !llvm.loop !8

execute.exit:                                     ; preds = %.lr.ph.i, %middle.block, %vec.epilog.middle.block, %1
  %.lcssa.i = phi i32 [ 0, %1 ], [ %27, %middle.block ], [ %36, %vec.epilog.middle.block ], [ %42, %.lr.ph.i ]
  ret i32 %.lcssa.i
}

; Function Attrs: nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: none)
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %.not3.i = icmp eq i32 %3, 0
  br i1 %.not3.i, label %execute.exit, label %iter.check

iter.check:                                       ; preds = %1
  %min.iters.check = icmp ult i32 %3, 8
  br i1 %min.iters.check, label %.lr.ph.i.preheader, label %vector.main.loop.iter.check

vector.main.loop.iter.check:                      ; preds = %iter.check
  %min.iters.check1 = icmp ult i32 %3, 32
  br i1 %min.iters.check1, label %vec.epilog.ph, label %vector.ph

vector.ph:                                        ; preds = %vector.main.loop.iter.check
  %n.vec = and i32 %3, -32
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i32 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %vec.ind = phi <8 x i32> [ <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7>, %vector.ph ], [ %vec.ind.next, %vector.body ]
  %vec.phi = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %24, %vector.body ]
  %vec.phi2 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %25, %vector.body ]
  %vec.phi3 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %26, %vector.body ]
  %vec.phi4 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %27, %vector.body ]
  %step.add = add <8 x i32> %vec.ind, splat (i32 8)
  %step.add.2 = add <8 x i32> %vec.ind, splat (i32 16)
  %step.add.3 = add <8 x i32> %vec.ind, splat (i32 24)
  %4 = icmp sgt <8 x i32> %vec.ind, splat (i32 5)
  %5 = icmp sgt <8 x i32> %step.add, splat (i32 5)
  %6 = icmp sgt <8 x i32> %step.add.2, splat (i32 5)
  %7 = icmp sgt <8 x i32> %step.add.3, splat (i32 5)
  %8 = zext <8 x i1> %4 to <8 x i32>
  %9 = zext <8 x i1> %5 to <8 x i32>
  %10 = zext <8 x i1> %6 to <8 x i32>
  %11 = zext <8 x i1> %7 to <8 x i32>
  %12 = select <8 x i1> %4, <8 x i32> splat (i32 22), <8 x i32> splat (i32 20)
  %13 = select <8 x i1> %5, <8 x i32> splat (i32 22), <8 x i32> splat (i32 20)
  %14 = select <8 x i1> %6, <8 x i32> splat (i32 22), <8 x i32> splat (i32 20)
  %15 = select <8 x i1> %7, <8 x i32> splat (i32 22), <8 x i32> splat (i32 20)
  %16 = add <8 x i32> %vec.phi, splat (i32 10)
  %17 = add <8 x i32> %vec.phi2, splat (i32 10)
  %18 = add <8 x i32> %vec.phi3, splat (i32 10)
  %19 = add <8 x i32> %vec.phi4, splat (i32 10)
  %20 = add <8 x i32> %16, %8
  %21 = add <8 x i32> %17, %9
  %22 = add <8 x i32> %18, %10
  %23 = add <8 x i32> %19, %11
  %24 = add <8 x i32> %20, %12
  %25 = add <8 x i32> %21, %13
  %26 = add <8 x i32> %22, %14
  %27 = add <8 x i32> %23, %15
  %index.next = add nuw i32 %index, 32
  %vec.ind.next = add <8 x i32> %vec.ind, splat (i32 32)
  %28 = icmp eq i32 %index.next, %n.vec
  br i1 %28, label %middle.block, label %vector.body, !llvm.loop !9

middle.block:                                     ; preds = %vector.body
  %bin.rdx = add <8 x i32> %25, %24
  %bin.rdx5 = add <8 x i32> %26, %bin.rdx
  %bin.rdx6 = add <8 x i32> %27, %bin.rdx5
  %29 = tail call i32 @llvm.vector.reduce.add.v8i32(<8 x i32> %bin.rdx6)
  %cmp.n = icmp eq i32 %3, %n.vec
  br i1 %cmp.n, label %execute.exit, label %vec.epilog.iter.check

vec.epilog.iter.check:                            ; preds = %middle.block
  %n.vec.remaining = and i32 %3, 24
  %min.epilog.iters.check = icmp eq i32 %n.vec.remaining, 0
  br i1 %min.epilog.iters.check, label %.lr.ph.i.preheader, label %vec.epilog.ph

vec.epilog.ph:                                    ; preds = %vec.epilog.iter.check, %vector.main.loop.iter.check
  %bc.resume.val = phi i32 [ %n.vec, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %bc.merge.rdx = phi i32 [ %29, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %n.vec8 = and i32 %3, -8
  %30 = insertelement <8 x i32> <i32 poison, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0>, i32 %bc.merge.rdx, i64 0
  %broadcast.splatinsert = insertelement <8 x i32> poison, i32 %bc.resume.val, i64 0
  %broadcast.splat = shufflevector <8 x i32> %broadcast.splatinsert, <8 x i32> poison, <8 x i32> zeroinitializer
  %induction = or disjoint <8 x i32> %broadcast.splat, <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7>
  br label %vec.epilog.vector.body

vec.epilog.vector.body:                           ; preds = %vec.epilog.vector.body, %vec.epilog.ph
  %index9 = phi i32 [ %bc.resume.val, %vec.epilog.ph ], [ %index.next12, %vec.epilog.vector.body ]
  %vec.ind10 = phi <8 x i32> [ %induction, %vec.epilog.ph ], [ %vec.ind.next13, %vec.epilog.vector.body ]
  %vec.phi11 = phi <8 x i32> [ %30, %vec.epilog.ph ], [ %36, %vec.epilog.vector.body ]
  %31 = icmp sgt <8 x i32> %vec.ind10, splat (i32 5)
  %32 = zext <8 x i1> %31 to <8 x i32>
  %33 = select <8 x i1> %31, <8 x i32> splat (i32 22), <8 x i32> splat (i32 20)
  %34 = add <8 x i32> %vec.phi11, splat (i32 10)
  %35 = add <8 x i32> %34, %32
  %36 = add <8 x i32> %35, %33
  %index.next12 = add nuw i32 %index9, 8
  %vec.ind.next13 = add <8 x i32> %vec.ind10, splat (i32 8)
  %37 = icmp eq i32 %index.next12, %n.vec8
  br i1 %37, label %vec.epilog.middle.block, label %vec.epilog.vector.body, !llvm.loop !10

vec.epilog.middle.block:                          ; preds = %vec.epilog.vector.body
  %38 = tail call i32 @llvm.vector.reduce.add.v8i32(<8 x i32> %36)
  %cmp.n14 = icmp eq i32 %3, %n.vec8
  br i1 %cmp.n14, label %execute.exit, label %.lr.ph.i.preheader

.lr.ph.i.preheader:                               ; preds = %vec.epilog.iter.check, %vec.epilog.middle.block, %iter.check
  %.ph = phi i32 [ 0, %iter.check ], [ %n.vec, %vec.epilog.iter.check ], [ %n.vec8, %vec.epilog.middle.block ]
  %.ph17 = phi i32 [ 0, %iter.check ], [ %29, %vec.epilog.iter.check ], [ %38, %vec.epilog.middle.block ]
  br label %.lr.ph.i

.lr.ph.i:                                         ; preds = %.lr.ph.i.preheader, %.lr.ph.i
  %39 = phi i32 [ %45, %.lr.ph.i ], [ %.ph, %.lr.ph.i.preheader ]
  %40 = phi i32 [ %44, %.lr.ph.i ], [ %.ph17, %.lr.ph.i.preheader ]
  %41 = icmp sgt i32 %39, 5
  %42 = zext i1 %41 to i32
  %.v.i = select i1 %41, i32 22, i32 20
  %spec.select.i = add i32 %40, 10
  %43 = add i32 %spec.select.i, %42
  %44 = add i32 %43, %.v.i
  %45 = add nuw i32 %39, 1
  %.not.i = icmp eq i32 %45, %3
  br i1 %.not.i, label %execute.exit, label %.lr.ph.i, !llvm.loop !11

execute.exit:                                     ; preds = %.lr.ph.i, %middle.block, %vec.epilog.middle.block, %1
  %.lcssa.i = phi i32 [ 0, %1 ], [ %29, %middle.block ], [ %38, %vec.epilog.middle.block ], [ %44, %.lr.ph.i ]
  %46 = getelementptr i8, ptr %0, i64 8
  %47 = load ptr, ptr %46, align 8
  store i32 %.lcssa.i, ptr %47, align 4
  ret void
}

; Function Attrs: nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: none)
define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %.not3.i.i = icmp eq i32 %3, 0
  br i1 %.not3.i.i, label %_mlir_ciface_execute.exit, label %iter.check

iter.check:                                       ; preds = %1
  %min.iters.check = icmp ult i32 %3, 8
  br i1 %min.iters.check, label %.lr.ph.i.i.preheader, label %vector.main.loop.iter.check

vector.main.loop.iter.check:                      ; preds = %iter.check
  %min.iters.check1 = icmp ult i32 %3, 32
  br i1 %min.iters.check1, label %vec.epilog.ph, label %vector.ph

vector.ph:                                        ; preds = %vector.main.loop.iter.check
  %n.vec = and i32 %3, -32
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i32 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %vec.ind = phi <8 x i32> [ <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7>, %vector.ph ], [ %vec.ind.next, %vector.body ]
  %vec.phi = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %24, %vector.body ]
  %vec.phi2 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %25, %vector.body ]
  %vec.phi3 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %26, %vector.body ]
  %vec.phi4 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %27, %vector.body ]
  %step.add = add <8 x i32> %vec.ind, splat (i32 8)
  %step.add.2 = add <8 x i32> %vec.ind, splat (i32 16)
  %step.add.3 = add <8 x i32> %vec.ind, splat (i32 24)
  %4 = icmp sgt <8 x i32> %vec.ind, splat (i32 5)
  %5 = icmp sgt <8 x i32> %step.add, splat (i32 5)
  %6 = icmp sgt <8 x i32> %step.add.2, splat (i32 5)
  %7 = icmp sgt <8 x i32> %step.add.3, splat (i32 5)
  %8 = zext <8 x i1> %4 to <8 x i32>
  %9 = zext <8 x i1> %5 to <8 x i32>
  %10 = zext <8 x i1> %6 to <8 x i32>
  %11 = zext <8 x i1> %7 to <8 x i32>
  %12 = select <8 x i1> %4, <8 x i32> splat (i32 22), <8 x i32> splat (i32 20)
  %13 = select <8 x i1> %5, <8 x i32> splat (i32 22), <8 x i32> splat (i32 20)
  %14 = select <8 x i1> %6, <8 x i32> splat (i32 22), <8 x i32> splat (i32 20)
  %15 = select <8 x i1> %7, <8 x i32> splat (i32 22), <8 x i32> splat (i32 20)
  %16 = add <8 x i32> %vec.phi, splat (i32 10)
  %17 = add <8 x i32> %vec.phi2, splat (i32 10)
  %18 = add <8 x i32> %vec.phi3, splat (i32 10)
  %19 = add <8 x i32> %vec.phi4, splat (i32 10)
  %20 = add <8 x i32> %16, %8
  %21 = add <8 x i32> %17, %9
  %22 = add <8 x i32> %18, %10
  %23 = add <8 x i32> %19, %11
  %24 = add <8 x i32> %20, %12
  %25 = add <8 x i32> %21, %13
  %26 = add <8 x i32> %22, %14
  %27 = add <8 x i32> %23, %15
  %index.next = add nuw i32 %index, 32
  %vec.ind.next = add <8 x i32> %vec.ind, splat (i32 32)
  %28 = icmp eq i32 %index.next, %n.vec
  br i1 %28, label %middle.block, label %vector.body, !llvm.loop !12

middle.block:                                     ; preds = %vector.body
  %bin.rdx = add <8 x i32> %25, %24
  %bin.rdx5 = add <8 x i32> %26, %bin.rdx
  %bin.rdx6 = add <8 x i32> %27, %bin.rdx5
  %29 = tail call i32 @llvm.vector.reduce.add.v8i32(<8 x i32> %bin.rdx6)
  %cmp.n = icmp eq i32 %3, %n.vec
  br i1 %cmp.n, label %_mlir_ciface_execute.exit, label %vec.epilog.iter.check

vec.epilog.iter.check:                            ; preds = %middle.block
  %n.vec.remaining = and i32 %3, 24
  %min.epilog.iters.check = icmp eq i32 %n.vec.remaining, 0
  br i1 %min.epilog.iters.check, label %.lr.ph.i.i.preheader, label %vec.epilog.ph

vec.epilog.ph:                                    ; preds = %vec.epilog.iter.check, %vector.main.loop.iter.check
  %bc.resume.val = phi i32 [ %n.vec, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %bc.merge.rdx = phi i32 [ %29, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %n.vec8 = and i32 %3, -8
  %30 = insertelement <8 x i32> <i32 poison, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0>, i32 %bc.merge.rdx, i64 0
  %broadcast.splatinsert = insertelement <8 x i32> poison, i32 %bc.resume.val, i64 0
  %broadcast.splat = shufflevector <8 x i32> %broadcast.splatinsert, <8 x i32> poison, <8 x i32> zeroinitializer
  %induction = or disjoint <8 x i32> %broadcast.splat, <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7>
  br label %vec.epilog.vector.body

vec.epilog.vector.body:                           ; preds = %vec.epilog.vector.body, %vec.epilog.ph
  %index9 = phi i32 [ %bc.resume.val, %vec.epilog.ph ], [ %index.next12, %vec.epilog.vector.body ]
  %vec.ind10 = phi <8 x i32> [ %induction, %vec.epilog.ph ], [ %vec.ind.next13, %vec.epilog.vector.body ]
  %vec.phi11 = phi <8 x i32> [ %30, %vec.epilog.ph ], [ %36, %vec.epilog.vector.body ]
  %31 = icmp sgt <8 x i32> %vec.ind10, splat (i32 5)
  %32 = zext <8 x i1> %31 to <8 x i32>
  %33 = select <8 x i1> %31, <8 x i32> splat (i32 22), <8 x i32> splat (i32 20)
  %34 = add <8 x i32> %vec.phi11, splat (i32 10)
  %35 = add <8 x i32> %34, %32
  %36 = add <8 x i32> %35, %33
  %index.next12 = add nuw i32 %index9, 8
  %vec.ind.next13 = add <8 x i32> %vec.ind10, splat (i32 8)
  %37 = icmp eq i32 %index.next12, %n.vec8
  br i1 %37, label %vec.epilog.middle.block, label %vec.epilog.vector.body, !llvm.loop !13

vec.epilog.middle.block:                          ; preds = %vec.epilog.vector.body
  %38 = tail call i32 @llvm.vector.reduce.add.v8i32(<8 x i32> %36)
  %cmp.n14 = icmp eq i32 %3, %n.vec8
  br i1 %cmp.n14, label %_mlir_ciface_execute.exit, label %.lr.ph.i.i.preheader

.lr.ph.i.i.preheader:                             ; preds = %vec.epilog.iter.check, %vec.epilog.middle.block, %iter.check
  %.ph = phi i32 [ 0, %iter.check ], [ %n.vec, %vec.epilog.iter.check ], [ %n.vec8, %vec.epilog.middle.block ]
  %.ph17 = phi i32 [ 0, %iter.check ], [ %29, %vec.epilog.iter.check ], [ %38, %vec.epilog.middle.block ]
  br label %.lr.ph.i.i

.lr.ph.i.i:                                       ; preds = %.lr.ph.i.i.preheader, %.lr.ph.i.i
  %39 = phi i32 [ %45, %.lr.ph.i.i ], [ %.ph, %.lr.ph.i.i.preheader ]
  %40 = phi i32 [ %44, %.lr.ph.i.i ], [ %.ph17, %.lr.ph.i.i.preheader ]
  %41 = icmp sgt i32 %39, 5
  %42 = zext i1 %41 to i32
  %.v.i.i = select i1 %41, i32 22, i32 20
  %spec.select.i.i = add i32 %40, 10
  %43 = add i32 %spec.select.i.i, %42
  %44 = add i32 %43, %.v.i.i
  %45 = add nuw i32 %39, 1
  %.not.i.i = icmp eq i32 %45, %3
  br i1 %.not.i.i, label %_mlir_ciface_execute.exit, label %.lr.ph.i.i, !llvm.loop !14

_mlir_ciface_execute.exit:                        ; preds = %.lr.ph.i.i, %middle.block, %vec.epilog.middle.block, %1
  %.lcssa.i.i = phi i32 [ 0, %1 ], [ %29, %middle.block ], [ %38, %vec.epilog.middle.block ], [ %44, %.lr.ph.i.i ]
  %46 = getelementptr i8, ptr %0, i64 8
  %47 = load ptr, ptr %46, align 8
  store i32 %.lcssa.i.i, ptr %47, align 4
  ret void
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i32 @llvm.vector.reduce.add.v8i32(<8 x i32>) #3

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
