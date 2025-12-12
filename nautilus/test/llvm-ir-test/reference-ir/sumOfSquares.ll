; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nofree norecurse nosync nounwind memory(none)
define signext i32 @execute(i32 %0) local_unnamed_addr #0 {
  %.not2 = icmp slt i32 %0, 1
  br i1 %.not2, label %._crit_edge, label %iter.check

iter.check:                                       ; preds = %1
  %min.iters.check = icmp ult i32 %0, 4
  %2 = zext nneg i32 %0 to i64
  %3 = add nsw i64 %2, -2147483647
  %4 = icmp ult i64 %3, -2147483646
  %or.cond = select i1 %min.iters.check, i1 true, i1 %4
  br i1 %or.cond, label %.lr.ph.preheader, label %vector.main.loop.iter.check

vector.main.loop.iter.check:                      ; preds = %iter.check
  %min.iters.check6 = icmp ult i32 %0, 32
  br i1 %min.iters.check6, label %vec.epilog.ph, label %vector.ph

vector.ph:                                        ; preds = %vector.main.loop.iter.check
  %n.vec = and i32 %0, 2147483616
  %5 = or disjoint i32 %n.vec, 1
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i32 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %vec.ind = phi <8 x i32> [ <i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8>, %vector.ph ], [ %vec.ind.next, %vector.body ]
  %vec.phi = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %10, %vector.body ]
  %vec.phi7 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %11, %vector.body ]
  %vec.phi8 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %12, %vector.body ]
  %vec.phi9 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %13, %vector.body ]
  %step.add = add <8 x i32> %vec.ind, splat (i32 8)
  %step.add.2 = add <8 x i32> %vec.ind, splat (i32 16)
  %step.add.3 = add <8 x i32> %vec.ind, splat (i32 24)
  %6 = mul <8 x i32> %vec.ind, %vec.ind
  %7 = mul <8 x i32> %step.add, %step.add
  %8 = mul <8 x i32> %step.add.2, %step.add.2
  %9 = mul <8 x i32> %step.add.3, %step.add.3
  %10 = add <8 x i32> %6, %vec.phi
  %11 = add <8 x i32> %7, %vec.phi7
  %12 = add <8 x i32> %8, %vec.phi8
  %13 = add <8 x i32> %9, %vec.phi9
  %index.next = add nuw i32 %index, 32
  %vec.ind.next = add <8 x i32> %vec.ind, splat (i32 32)
  %14 = icmp eq i32 %index.next, %n.vec
  br i1 %14, label %middle.block, label %vector.body, !llvm.loop !1

middle.block:                                     ; preds = %vector.body
  %bin.rdx = add <8 x i32> %11, %10
  %bin.rdx10 = add <8 x i32> %12, %bin.rdx
  %bin.rdx11 = add <8 x i32> %13, %bin.rdx10
  %15 = tail call i32 @llvm.vector.reduce.add.v8i32(<8 x i32> %bin.rdx11)
  %cmp.n = icmp eq i32 %0, %n.vec
  br i1 %cmp.n, label %._crit_edge, label %vec.epilog.iter.check

vec.epilog.iter.check:                            ; preds = %middle.block
  %ind.end = or disjoint i32 %n.vec, 1
  %n.vec.remaining = and i32 %0, 28
  %min.epilog.iters.check = icmp eq i32 %n.vec.remaining, 0
  br i1 %min.epilog.iters.check, label %.lr.ph.preheader, label %vec.epilog.ph

vec.epilog.ph:                                    ; preds = %vec.epilog.iter.check, %vector.main.loop.iter.check
  %vec.epilog.resume.val = phi i32 [ %n.vec, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %bc.resume.val = phi i32 [ %5, %vec.epilog.iter.check ], [ 1, %vector.main.loop.iter.check ]
  %bc.merge.rdx = phi i32 [ %15, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %n.vec13 = and i32 %0, 2147483644
  %16 = or disjoint i32 %n.vec13, 1
  %17 = insertelement <4 x i32> <i32 poison, i32 0, i32 0, i32 0>, i32 %bc.merge.rdx, i64 0
  %broadcast.splatinsert = insertelement <4 x i32> poison, i32 %bc.resume.val, i64 0
  %broadcast.splat = shufflevector <4 x i32> %broadcast.splatinsert, <4 x i32> poison, <4 x i32> zeroinitializer
  %induction = add <4 x i32> %broadcast.splat, <i32 0, i32 1, i32 2, i32 3>
  br label %vec.epilog.vector.body

vec.epilog.vector.body:                           ; preds = %vec.epilog.vector.body, %vec.epilog.ph
  %index14 = phi i32 [ %vec.epilog.resume.val, %vec.epilog.ph ], [ %index.next17, %vec.epilog.vector.body ]
  %vec.ind15 = phi <4 x i32> [ %induction, %vec.epilog.ph ], [ %vec.ind.next18, %vec.epilog.vector.body ]
  %vec.phi16 = phi <4 x i32> [ %17, %vec.epilog.ph ], [ %19, %vec.epilog.vector.body ]
  %18 = mul <4 x i32> %vec.ind15, %vec.ind15
  %19 = add <4 x i32> %18, %vec.phi16
  %index.next17 = add nuw i32 %index14, 4
  %vec.ind.next18 = add <4 x i32> %vec.ind15, splat (i32 4)
  %20 = icmp eq i32 %index.next17, %n.vec13
  br i1 %20, label %vec.epilog.middle.block, label %vec.epilog.vector.body, !llvm.loop !4

vec.epilog.middle.block:                          ; preds = %vec.epilog.vector.body
  %21 = tail call i32 @llvm.vector.reduce.add.v4i32(<4 x i32> %19)
  %cmp.n19 = icmp eq i32 %0, %n.vec13
  br i1 %cmp.n19, label %._crit_edge, label %.lr.ph.preheader

.lr.ph.preheader:                                 ; preds = %vec.epilog.iter.check, %vec.epilog.middle.block, %iter.check
  %.ph = phi i32 [ 1, %iter.check ], [ %ind.end, %vec.epilog.iter.check ], [ %16, %vec.epilog.middle.block ]
  %.ph24 = phi i32 [ 0, %iter.check ], [ %15, %vec.epilog.iter.check ], [ %21, %vec.epilog.middle.block ]
  br label %.lr.ph

.lr.ph:                                           ; preds = %.lr.ph.preheader, %.lr.ph
  %22 = phi i32 [ %26, %.lr.ph ], [ %.ph, %.lr.ph.preheader ]
  %23 = phi i32 [ %25, %.lr.ph ], [ %.ph24, %.lr.ph.preheader ]
  %24 = mul i32 %22, %22
  %25 = add i32 %24, %23
  %26 = add i32 %22, 1
  %.not = icmp sgt i32 %26, %0
  br i1 %.not, label %._crit_edge, label %.lr.ph, !llvm.loop !5

._crit_edge:                                      ; preds = %.lr.ph, %middle.block, %vec.epilog.middle.block, %1
  %.lcssa = phi i32 [ 0, %1 ], [ %15, %middle.block ], [ %21, %vec.epilog.middle.block ], [ %25, %.lr.ph ]
  ret i32 %.lcssa
}

; Function Attrs: nofree norecurse nosync nounwind memory(none)
define signext i32 @_mlir_ciface_execute(i32 %0) local_unnamed_addr #1 {
  %.not2.i = icmp slt i32 %0, 1
  br i1 %.not2.i, label %execute.exit, label %iter.check

iter.check:                                       ; preds = %1
  %min.iters.check = icmp ult i32 %0, 4
  %2 = zext nneg i32 %0 to i64
  %3 = add nsw i64 %2, -2147483647
  %4 = icmp ult i64 %3, -2147483646
  %or.cond = select i1 %min.iters.check, i1 true, i1 %4
  br i1 %or.cond, label %.lr.ph.i.preheader, label %vector.main.loop.iter.check

vector.main.loop.iter.check:                      ; preds = %iter.check
  %min.iters.check1 = icmp ult i32 %0, 32
  br i1 %min.iters.check1, label %vec.epilog.ph, label %vector.ph

vector.ph:                                        ; preds = %vector.main.loop.iter.check
  %n.vec = and i32 %0, 2147483616
  %5 = or disjoint i32 %n.vec, 1
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i32 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %vec.ind = phi <8 x i32> [ <i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8>, %vector.ph ], [ %vec.ind.next, %vector.body ]
  %vec.phi = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %10, %vector.body ]
  %vec.phi2 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %11, %vector.body ]
  %vec.phi3 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %12, %vector.body ]
  %vec.phi4 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %13, %vector.body ]
  %step.add = add <8 x i32> %vec.ind, splat (i32 8)
  %step.add.2 = add <8 x i32> %vec.ind, splat (i32 16)
  %step.add.3 = add <8 x i32> %vec.ind, splat (i32 24)
  %6 = mul <8 x i32> %vec.ind, %vec.ind
  %7 = mul <8 x i32> %step.add, %step.add
  %8 = mul <8 x i32> %step.add.2, %step.add.2
  %9 = mul <8 x i32> %step.add.3, %step.add.3
  %10 = add <8 x i32> %6, %vec.phi
  %11 = add <8 x i32> %7, %vec.phi2
  %12 = add <8 x i32> %8, %vec.phi3
  %13 = add <8 x i32> %9, %vec.phi4
  %index.next = add nuw i32 %index, 32
  %vec.ind.next = add <8 x i32> %vec.ind, splat (i32 32)
  %14 = icmp eq i32 %index.next, %n.vec
  br i1 %14, label %middle.block, label %vector.body, !llvm.loop !6

middle.block:                                     ; preds = %vector.body
  %bin.rdx = add <8 x i32> %11, %10
  %bin.rdx5 = add <8 x i32> %12, %bin.rdx
  %bin.rdx6 = add <8 x i32> %13, %bin.rdx5
  %15 = tail call i32 @llvm.vector.reduce.add.v8i32(<8 x i32> %bin.rdx6)
  %cmp.n = icmp eq i32 %0, %n.vec
  br i1 %cmp.n, label %execute.exit, label %vec.epilog.iter.check

vec.epilog.iter.check:                            ; preds = %middle.block
  %ind.end = or disjoint i32 %n.vec, 1
  %n.vec.remaining = and i32 %0, 28
  %min.epilog.iters.check = icmp eq i32 %n.vec.remaining, 0
  br i1 %min.epilog.iters.check, label %.lr.ph.i.preheader, label %vec.epilog.ph

vec.epilog.ph:                                    ; preds = %vec.epilog.iter.check, %vector.main.loop.iter.check
  %vec.epilog.resume.val = phi i32 [ %n.vec, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %bc.resume.val = phi i32 [ %5, %vec.epilog.iter.check ], [ 1, %vector.main.loop.iter.check ]
  %bc.merge.rdx = phi i32 [ %15, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %n.vec8 = and i32 %0, 2147483644
  %16 = or disjoint i32 %n.vec8, 1
  %17 = insertelement <4 x i32> <i32 poison, i32 0, i32 0, i32 0>, i32 %bc.merge.rdx, i64 0
  %broadcast.splatinsert = insertelement <4 x i32> poison, i32 %bc.resume.val, i64 0
  %broadcast.splat = shufflevector <4 x i32> %broadcast.splatinsert, <4 x i32> poison, <4 x i32> zeroinitializer
  %induction = add <4 x i32> %broadcast.splat, <i32 0, i32 1, i32 2, i32 3>
  br label %vec.epilog.vector.body

vec.epilog.vector.body:                           ; preds = %vec.epilog.vector.body, %vec.epilog.ph
  %index9 = phi i32 [ %vec.epilog.resume.val, %vec.epilog.ph ], [ %index.next12, %vec.epilog.vector.body ]
  %vec.ind10 = phi <4 x i32> [ %induction, %vec.epilog.ph ], [ %vec.ind.next13, %vec.epilog.vector.body ]
  %vec.phi11 = phi <4 x i32> [ %17, %vec.epilog.ph ], [ %19, %vec.epilog.vector.body ]
  %18 = mul <4 x i32> %vec.ind10, %vec.ind10
  %19 = add <4 x i32> %18, %vec.phi11
  %index.next12 = add nuw i32 %index9, 4
  %vec.ind.next13 = add <4 x i32> %vec.ind10, splat (i32 4)
  %20 = icmp eq i32 %index.next12, %n.vec8
  br i1 %20, label %vec.epilog.middle.block, label %vec.epilog.vector.body, !llvm.loop !7

vec.epilog.middle.block:                          ; preds = %vec.epilog.vector.body
  %21 = tail call i32 @llvm.vector.reduce.add.v4i32(<4 x i32> %19)
  %cmp.n14 = icmp eq i32 %0, %n.vec8
  br i1 %cmp.n14, label %execute.exit, label %.lr.ph.i.preheader

.lr.ph.i.preheader:                               ; preds = %vec.epilog.iter.check, %vec.epilog.middle.block, %iter.check
  %.ph = phi i32 [ 1, %iter.check ], [ %ind.end, %vec.epilog.iter.check ], [ %16, %vec.epilog.middle.block ]
  %.ph19 = phi i32 [ 0, %iter.check ], [ %15, %vec.epilog.iter.check ], [ %21, %vec.epilog.middle.block ]
  br label %.lr.ph.i

.lr.ph.i:                                         ; preds = %.lr.ph.i.preheader, %.lr.ph.i
  %22 = phi i32 [ %26, %.lr.ph.i ], [ %.ph, %.lr.ph.i.preheader ]
  %23 = phi i32 [ %25, %.lr.ph.i ], [ %.ph19, %.lr.ph.i.preheader ]
  %24 = mul i32 %22, %22
  %25 = add i32 %24, %23
  %26 = add i32 %22, 1
  %.not.i = icmp sgt i32 %26, %0
  br i1 %.not.i, label %execute.exit, label %.lr.ph.i, !llvm.loop !8

execute.exit:                                     ; preds = %.lr.ph.i, %middle.block, %vec.epilog.middle.block, %1
  %.lcssa.i = phi i32 [ 0, %1 ], [ %15, %middle.block ], [ %21, %vec.epilog.middle.block ], [ %25, %.lr.ph.i ]
  ret i32 %.lcssa.i
}

; Function Attrs: nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: none)
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %.not2.i = icmp slt i32 %3, 1
  br i1 %.not2.i, label %execute.exit, label %iter.check

iter.check:                                       ; preds = %1
  %min.iters.check = icmp ult i32 %3, 4
  %4 = zext nneg i32 %3 to i64
  %5 = add nsw i64 %4, -2147483647
  %6 = icmp ult i64 %5, -2147483646
  %or.cond = select i1 %min.iters.check, i1 true, i1 %6
  br i1 %or.cond, label %.lr.ph.i.preheader, label %vector.main.loop.iter.check

vector.main.loop.iter.check:                      ; preds = %iter.check
  %min.iters.check1 = icmp ult i32 %3, 32
  br i1 %min.iters.check1, label %vec.epilog.ph, label %vector.ph

vector.ph:                                        ; preds = %vector.main.loop.iter.check
  %n.vec = and i32 %3, 2147483616
  %7 = or disjoint i32 %n.vec, 1
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i32 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %vec.ind = phi <8 x i32> [ <i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8>, %vector.ph ], [ %vec.ind.next, %vector.body ]
  %vec.phi = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %12, %vector.body ]
  %vec.phi2 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %13, %vector.body ]
  %vec.phi3 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %14, %vector.body ]
  %vec.phi4 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %15, %vector.body ]
  %step.add = add <8 x i32> %vec.ind, splat (i32 8)
  %step.add.2 = add <8 x i32> %vec.ind, splat (i32 16)
  %step.add.3 = add <8 x i32> %vec.ind, splat (i32 24)
  %8 = mul <8 x i32> %vec.ind, %vec.ind
  %9 = mul <8 x i32> %step.add, %step.add
  %10 = mul <8 x i32> %step.add.2, %step.add.2
  %11 = mul <8 x i32> %step.add.3, %step.add.3
  %12 = add <8 x i32> %8, %vec.phi
  %13 = add <8 x i32> %9, %vec.phi2
  %14 = add <8 x i32> %10, %vec.phi3
  %15 = add <8 x i32> %11, %vec.phi4
  %index.next = add nuw i32 %index, 32
  %vec.ind.next = add <8 x i32> %vec.ind, splat (i32 32)
  %16 = icmp eq i32 %index.next, %n.vec
  br i1 %16, label %middle.block, label %vector.body, !llvm.loop !9

middle.block:                                     ; preds = %vector.body
  %bin.rdx = add <8 x i32> %13, %12
  %bin.rdx5 = add <8 x i32> %14, %bin.rdx
  %bin.rdx6 = add <8 x i32> %15, %bin.rdx5
  %17 = tail call i32 @llvm.vector.reduce.add.v8i32(<8 x i32> %bin.rdx6)
  %cmp.n = icmp eq i32 %3, %n.vec
  br i1 %cmp.n, label %execute.exit, label %vec.epilog.iter.check

vec.epilog.iter.check:                            ; preds = %middle.block
  %ind.end = or disjoint i32 %n.vec, 1
  %n.vec.remaining = and i32 %3, 28
  %min.epilog.iters.check = icmp eq i32 %n.vec.remaining, 0
  br i1 %min.epilog.iters.check, label %.lr.ph.i.preheader, label %vec.epilog.ph

vec.epilog.ph:                                    ; preds = %vec.epilog.iter.check, %vector.main.loop.iter.check
  %vec.epilog.resume.val = phi i32 [ %n.vec, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %bc.resume.val = phi i32 [ %7, %vec.epilog.iter.check ], [ 1, %vector.main.loop.iter.check ]
  %bc.merge.rdx = phi i32 [ %17, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %n.vec8 = and i32 %3, 2147483644
  %18 = or disjoint i32 %n.vec8, 1
  %19 = insertelement <4 x i32> <i32 poison, i32 0, i32 0, i32 0>, i32 %bc.merge.rdx, i64 0
  %broadcast.splatinsert = insertelement <4 x i32> poison, i32 %bc.resume.val, i64 0
  %broadcast.splat = shufflevector <4 x i32> %broadcast.splatinsert, <4 x i32> poison, <4 x i32> zeroinitializer
  %induction = add <4 x i32> %broadcast.splat, <i32 0, i32 1, i32 2, i32 3>
  br label %vec.epilog.vector.body

vec.epilog.vector.body:                           ; preds = %vec.epilog.vector.body, %vec.epilog.ph
  %index9 = phi i32 [ %vec.epilog.resume.val, %vec.epilog.ph ], [ %index.next12, %vec.epilog.vector.body ]
  %vec.ind10 = phi <4 x i32> [ %induction, %vec.epilog.ph ], [ %vec.ind.next13, %vec.epilog.vector.body ]
  %vec.phi11 = phi <4 x i32> [ %19, %vec.epilog.ph ], [ %21, %vec.epilog.vector.body ]
  %20 = mul <4 x i32> %vec.ind10, %vec.ind10
  %21 = add <4 x i32> %20, %vec.phi11
  %index.next12 = add nuw i32 %index9, 4
  %vec.ind.next13 = add <4 x i32> %vec.ind10, splat (i32 4)
  %22 = icmp eq i32 %index.next12, %n.vec8
  br i1 %22, label %vec.epilog.middle.block, label %vec.epilog.vector.body, !llvm.loop !10

vec.epilog.middle.block:                          ; preds = %vec.epilog.vector.body
  %23 = tail call i32 @llvm.vector.reduce.add.v4i32(<4 x i32> %21)
  %cmp.n14 = icmp eq i32 %3, %n.vec8
  br i1 %cmp.n14, label %execute.exit, label %.lr.ph.i.preheader

.lr.ph.i.preheader:                               ; preds = %vec.epilog.iter.check, %vec.epilog.middle.block, %iter.check
  %.ph = phi i32 [ 1, %iter.check ], [ %ind.end, %vec.epilog.iter.check ], [ %18, %vec.epilog.middle.block ]
  %.ph19 = phi i32 [ 0, %iter.check ], [ %17, %vec.epilog.iter.check ], [ %23, %vec.epilog.middle.block ]
  br label %.lr.ph.i

.lr.ph.i:                                         ; preds = %.lr.ph.i.preheader, %.lr.ph.i
  %24 = phi i32 [ %28, %.lr.ph.i ], [ %.ph, %.lr.ph.i.preheader ]
  %25 = phi i32 [ %27, %.lr.ph.i ], [ %.ph19, %.lr.ph.i.preheader ]
  %26 = mul i32 %24, %24
  %27 = add i32 %26, %25
  %28 = add i32 %24, 1
  %.not.i = icmp sgt i32 %28, %3
  br i1 %.not.i, label %execute.exit, label %.lr.ph.i, !llvm.loop !11

execute.exit:                                     ; preds = %.lr.ph.i, %middle.block, %vec.epilog.middle.block, %1
  %.lcssa.i = phi i32 [ 0, %1 ], [ %17, %middle.block ], [ %23, %vec.epilog.middle.block ], [ %27, %.lr.ph.i ]
  %29 = getelementptr i8, ptr %0, i64 8
  %30 = load ptr, ptr %29, align 8
  store i32 %.lcssa.i, ptr %30, align 4
  ret void
}

; Function Attrs: nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: none)
define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %.not2.i.i = icmp slt i32 %3, 1
  br i1 %.not2.i.i, label %_mlir_ciface_execute.exit, label %iter.check

iter.check:                                       ; preds = %1
  %min.iters.check = icmp ult i32 %3, 4
  %4 = zext nneg i32 %3 to i64
  %5 = add nsw i64 %4, -2147483647
  %6 = icmp ult i64 %5, -2147483646
  %or.cond = select i1 %min.iters.check, i1 true, i1 %6
  br i1 %or.cond, label %.lr.ph.i.i.preheader, label %vector.main.loop.iter.check

vector.main.loop.iter.check:                      ; preds = %iter.check
  %min.iters.check1 = icmp ult i32 %3, 32
  br i1 %min.iters.check1, label %vec.epilog.ph, label %vector.ph

vector.ph:                                        ; preds = %vector.main.loop.iter.check
  %n.vec = and i32 %3, 2147483616
  %7 = or disjoint i32 %n.vec, 1
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i32 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %vec.ind = phi <8 x i32> [ <i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8>, %vector.ph ], [ %vec.ind.next, %vector.body ]
  %vec.phi = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %12, %vector.body ]
  %vec.phi2 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %13, %vector.body ]
  %vec.phi3 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %14, %vector.body ]
  %vec.phi4 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %15, %vector.body ]
  %step.add = add <8 x i32> %vec.ind, splat (i32 8)
  %step.add.2 = add <8 x i32> %vec.ind, splat (i32 16)
  %step.add.3 = add <8 x i32> %vec.ind, splat (i32 24)
  %8 = mul <8 x i32> %vec.ind, %vec.ind
  %9 = mul <8 x i32> %step.add, %step.add
  %10 = mul <8 x i32> %step.add.2, %step.add.2
  %11 = mul <8 x i32> %step.add.3, %step.add.3
  %12 = add <8 x i32> %8, %vec.phi
  %13 = add <8 x i32> %9, %vec.phi2
  %14 = add <8 x i32> %10, %vec.phi3
  %15 = add <8 x i32> %11, %vec.phi4
  %index.next = add nuw i32 %index, 32
  %vec.ind.next = add <8 x i32> %vec.ind, splat (i32 32)
  %16 = icmp eq i32 %index.next, %n.vec
  br i1 %16, label %middle.block, label %vector.body, !llvm.loop !12

middle.block:                                     ; preds = %vector.body
  %bin.rdx = add <8 x i32> %13, %12
  %bin.rdx5 = add <8 x i32> %14, %bin.rdx
  %bin.rdx6 = add <8 x i32> %15, %bin.rdx5
  %17 = tail call i32 @llvm.vector.reduce.add.v8i32(<8 x i32> %bin.rdx6)
  %cmp.n = icmp eq i32 %3, %n.vec
  br i1 %cmp.n, label %_mlir_ciface_execute.exit, label %vec.epilog.iter.check

vec.epilog.iter.check:                            ; preds = %middle.block
  %ind.end = or disjoint i32 %n.vec, 1
  %n.vec.remaining = and i32 %3, 28
  %min.epilog.iters.check = icmp eq i32 %n.vec.remaining, 0
  br i1 %min.epilog.iters.check, label %.lr.ph.i.i.preheader, label %vec.epilog.ph

vec.epilog.ph:                                    ; preds = %vec.epilog.iter.check, %vector.main.loop.iter.check
  %vec.epilog.resume.val = phi i32 [ %n.vec, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %bc.resume.val = phi i32 [ %7, %vec.epilog.iter.check ], [ 1, %vector.main.loop.iter.check ]
  %bc.merge.rdx = phi i32 [ %17, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %n.vec8 = and i32 %3, 2147483644
  %18 = or disjoint i32 %n.vec8, 1
  %19 = insertelement <4 x i32> <i32 poison, i32 0, i32 0, i32 0>, i32 %bc.merge.rdx, i64 0
  %broadcast.splatinsert = insertelement <4 x i32> poison, i32 %bc.resume.val, i64 0
  %broadcast.splat = shufflevector <4 x i32> %broadcast.splatinsert, <4 x i32> poison, <4 x i32> zeroinitializer
  %induction = add <4 x i32> %broadcast.splat, <i32 0, i32 1, i32 2, i32 3>
  br label %vec.epilog.vector.body

vec.epilog.vector.body:                           ; preds = %vec.epilog.vector.body, %vec.epilog.ph
  %index9 = phi i32 [ %vec.epilog.resume.val, %vec.epilog.ph ], [ %index.next12, %vec.epilog.vector.body ]
  %vec.ind10 = phi <4 x i32> [ %induction, %vec.epilog.ph ], [ %vec.ind.next13, %vec.epilog.vector.body ]
  %vec.phi11 = phi <4 x i32> [ %19, %vec.epilog.ph ], [ %21, %vec.epilog.vector.body ]
  %20 = mul <4 x i32> %vec.ind10, %vec.ind10
  %21 = add <4 x i32> %20, %vec.phi11
  %index.next12 = add nuw i32 %index9, 4
  %vec.ind.next13 = add <4 x i32> %vec.ind10, splat (i32 4)
  %22 = icmp eq i32 %index.next12, %n.vec8
  br i1 %22, label %vec.epilog.middle.block, label %vec.epilog.vector.body, !llvm.loop !13

vec.epilog.middle.block:                          ; preds = %vec.epilog.vector.body
  %23 = tail call i32 @llvm.vector.reduce.add.v4i32(<4 x i32> %21)
  %cmp.n14 = icmp eq i32 %3, %n.vec8
  br i1 %cmp.n14, label %_mlir_ciface_execute.exit, label %.lr.ph.i.i.preheader

.lr.ph.i.i.preheader:                             ; preds = %vec.epilog.iter.check, %vec.epilog.middle.block, %iter.check
  %.ph = phi i32 [ 1, %iter.check ], [ %ind.end, %vec.epilog.iter.check ], [ %18, %vec.epilog.middle.block ]
  %.ph19 = phi i32 [ 0, %iter.check ], [ %17, %vec.epilog.iter.check ], [ %23, %vec.epilog.middle.block ]
  br label %.lr.ph.i.i

.lr.ph.i.i:                                       ; preds = %.lr.ph.i.i.preheader, %.lr.ph.i.i
  %24 = phi i32 [ %28, %.lr.ph.i.i ], [ %.ph, %.lr.ph.i.i.preheader ]
  %25 = phi i32 [ %27, %.lr.ph.i.i ], [ %.ph19, %.lr.ph.i.i.preheader ]
  %26 = mul i32 %24, %24
  %27 = add i32 %26, %25
  %28 = add i32 %24, 1
  %.not.i.i = icmp sgt i32 %28, %3
  br i1 %.not.i.i, label %_mlir_ciface_execute.exit, label %.lr.ph.i.i, !llvm.loop !14

_mlir_ciface_execute.exit:                        ; preds = %.lr.ph.i.i, %middle.block, %vec.epilog.middle.block, %1
  %.lcssa.i.i = phi i32 [ 0, %1 ], [ %17, %middle.block ], [ %23, %vec.epilog.middle.block ], [ %27, %.lr.ph.i.i ]
  %29 = getelementptr i8, ptr %0, i64 8
  %30 = load ptr, ptr %29, align 8
  store i32 %.lcssa.i.i, ptr %30, align 4
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
!5 = distinct !{!5, !2}
!6 = distinct !{!6, !2, !3}
!7 = distinct !{!7, !2, !3}
!8 = distinct !{!8, !2}
!9 = distinct !{!9, !2, !3}
!10 = distinct !{!10, !2, !3}
!11 = distinct !{!11, !2}
!12 = distinct !{!12, !2, !3}
!13 = distinct !{!13, !2, !3}
!14 = distinct !{!14, !2}
