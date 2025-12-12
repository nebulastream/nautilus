; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nofree norecurse nosync nounwind memory(none)
define signext i32 @execute(i32 %0) local_unnamed_addr #0 {
  %2 = icmp sgt i32 %0, 1
  br i1 %2, label %iter.check, label %._crit_edge

iter.check:                                       ; preds = %1
  %3 = add nsw i32 %0, -1
  %min.iters.check = icmp ult i32 %0, 5
  br i1 %min.iters.check, label %.lr.ph.preheader, label %vector.main.loop.iter.check

vector.main.loop.iter.check:                      ; preds = %iter.check
  %min.iters.check4 = icmp ult i32 %0, 33
  br i1 %min.iters.check4, label %vec.epilog.ph, label %vector.ph

vector.ph:                                        ; preds = %vector.main.loop.iter.check
  %n.vec = and i32 %3, -32
  %4 = sub i32 %0, %n.vec
  %broadcast.splatinsert = insertelement <8 x i32> poison, i32 %0, i64 0
  %broadcast.splat = shufflevector <8 x i32> %broadcast.splatinsert, <8 x i32> poison, <8 x i32> zeroinitializer
  %induction = add nsw <8 x i32> %broadcast.splat, <i32 0, i32 -1, i32 -2, i32 -3, i32 -4, i32 -5, i32 -6, i32 -7>
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i32 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %vec.ind = phi <8 x i32> [ %induction, %vector.ph ], [ %vec.ind.next, %vector.body ]
  %vec.phi = phi <8 x i32> [ splat (i32 1), %vector.ph ], [ %5, %vector.body ]
  %vec.phi5 = phi <8 x i32> [ splat (i32 1), %vector.ph ], [ %6, %vector.body ]
  %vec.phi6 = phi <8 x i32> [ splat (i32 1), %vector.ph ], [ %7, %vector.body ]
  %vec.phi7 = phi <8 x i32> [ splat (i32 1), %vector.ph ], [ %8, %vector.body ]
  %step.add = add <8 x i32> %vec.ind, splat (i32 -8)
  %step.add.2 = add <8 x i32> %vec.ind, splat (i32 -16)
  %step.add.3 = add <8 x i32> %vec.ind, splat (i32 -24)
  %5 = mul <8 x i32> %vec.ind, %vec.phi
  %6 = mul <8 x i32> %step.add, %vec.phi5
  %7 = mul <8 x i32> %step.add.2, %vec.phi6
  %8 = mul <8 x i32> %step.add.3, %vec.phi7
  %index.next = add nuw i32 %index, 32
  %vec.ind.next = add <8 x i32> %vec.ind, splat (i32 -32)
  %9 = icmp eq i32 %index.next, %n.vec
  br i1 %9, label %middle.block, label %vector.body, !llvm.loop !1

middle.block:                                     ; preds = %vector.body
  %bin.rdx = mul <8 x i32> %6, %5
  %bin.rdx8 = mul <8 x i32> %7, %bin.rdx
  %bin.rdx9 = mul <8 x i32> %8, %bin.rdx8
  %10 = tail call i32 @llvm.vector.reduce.mul.v8i32(<8 x i32> %bin.rdx9)
  %cmp.n = icmp eq i32 %3, %n.vec
  br i1 %cmp.n, label %._crit_edge, label %vec.epilog.iter.check

vec.epilog.iter.check:                            ; preds = %middle.block
  %ind.end = sub i32 %0, %n.vec
  %n.vec.remaining = and i32 %3, 28
  %min.epilog.iters.check = icmp eq i32 %n.vec.remaining, 0
  br i1 %min.epilog.iters.check, label %.lr.ph.preheader, label %vec.epilog.ph

vec.epilog.ph:                                    ; preds = %vec.epilog.iter.check, %vector.main.loop.iter.check
  %vec.epilog.resume.val = phi i32 [ %n.vec, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %bc.resume.val = phi i32 [ %4, %vec.epilog.iter.check ], [ %0, %vector.main.loop.iter.check ]
  %bc.merge.rdx = phi i32 [ %10, %vec.epilog.iter.check ], [ 1, %vector.main.loop.iter.check ]
  %n.vec11 = and i32 %3, -4
  %11 = sub i32 %0, %n.vec11
  %12 = insertelement <4 x i32> <i32 poison, i32 1, i32 1, i32 1>, i32 %bc.merge.rdx, i64 0
  %broadcast.splatinsert12 = insertelement <4 x i32> poison, i32 %bc.resume.val, i64 0
  %broadcast.splat13 = shufflevector <4 x i32> %broadcast.splatinsert12, <4 x i32> poison, <4 x i32> zeroinitializer
  %induction14 = add <4 x i32> %broadcast.splat13, <i32 0, i32 -1, i32 -2, i32 -3>
  br label %vec.epilog.vector.body

vec.epilog.vector.body:                           ; preds = %vec.epilog.vector.body, %vec.epilog.ph
  %index15 = phi i32 [ %vec.epilog.resume.val, %vec.epilog.ph ], [ %index.next18, %vec.epilog.vector.body ]
  %vec.ind16 = phi <4 x i32> [ %induction14, %vec.epilog.ph ], [ %vec.ind.next19, %vec.epilog.vector.body ]
  %vec.phi17 = phi <4 x i32> [ %12, %vec.epilog.ph ], [ %13, %vec.epilog.vector.body ]
  %13 = mul <4 x i32> %vec.ind16, %vec.phi17
  %index.next18 = add nuw i32 %index15, 4
  %vec.ind.next19 = add <4 x i32> %vec.ind16, splat (i32 -4)
  %14 = icmp eq i32 %index.next18, %n.vec11
  br i1 %14, label %vec.epilog.middle.block, label %vec.epilog.vector.body, !llvm.loop !4

vec.epilog.middle.block:                          ; preds = %vec.epilog.vector.body
  %15 = tail call i32 @llvm.vector.reduce.mul.v4i32(<4 x i32> %13)
  %cmp.n20 = icmp eq i32 %3, %n.vec11
  br i1 %cmp.n20, label %._crit_edge, label %.lr.ph.preheader

.lr.ph.preheader:                                 ; preds = %vec.epilog.iter.check, %vec.epilog.middle.block, %iter.check
  %.ph = phi i32 [ %0, %iter.check ], [ %ind.end, %vec.epilog.iter.check ], [ %11, %vec.epilog.middle.block ]
  %.ph23 = phi i32 [ 1, %iter.check ], [ %10, %vec.epilog.iter.check ], [ %15, %vec.epilog.middle.block ]
  br label %.lr.ph

.lr.ph:                                           ; preds = %.lr.ph.preheader, %.lr.ph
  %16 = phi i32 [ %19, %.lr.ph ], [ %.ph, %.lr.ph.preheader ]
  %17 = phi i32 [ %18, %.lr.ph ], [ %.ph23, %.lr.ph.preheader ]
  %18 = mul i32 %16, %17
  %19 = add nsw i32 %16, -1
  %20 = icmp samesign ugt i32 %16, 2
  br i1 %20, label %.lr.ph, label %._crit_edge, !llvm.loop !5

._crit_edge:                                      ; preds = %.lr.ph, %middle.block, %vec.epilog.middle.block, %1
  %.lcssa = phi i32 [ 1, %1 ], [ %10, %middle.block ], [ %15, %vec.epilog.middle.block ], [ %18, %.lr.ph ]
  ret i32 %.lcssa
}

; Function Attrs: nofree norecurse nosync nounwind memory(none)
define signext i32 @_mlir_ciface_execute(i32 %0) local_unnamed_addr #1 {
  %2 = icmp sgt i32 %0, 1
  br i1 %2, label %iter.check, label %execute.exit

iter.check:                                       ; preds = %1
  %3 = add nsw i32 %0, -1
  %min.iters.check = icmp ult i32 %0, 5
  br i1 %min.iters.check, label %.lr.ph.i.preheader, label %vector.main.loop.iter.check

vector.main.loop.iter.check:                      ; preds = %iter.check
  %min.iters.check1 = icmp ult i32 %0, 33
  br i1 %min.iters.check1, label %vec.epilog.ph, label %vector.ph

vector.ph:                                        ; preds = %vector.main.loop.iter.check
  %n.vec = and i32 %3, -32
  %4 = sub i32 %0, %n.vec
  %broadcast.splatinsert = insertelement <8 x i32> poison, i32 %0, i64 0
  %broadcast.splat = shufflevector <8 x i32> %broadcast.splatinsert, <8 x i32> poison, <8 x i32> zeroinitializer
  %induction = add nsw <8 x i32> %broadcast.splat, <i32 0, i32 -1, i32 -2, i32 -3, i32 -4, i32 -5, i32 -6, i32 -7>
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i32 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %vec.ind = phi <8 x i32> [ %induction, %vector.ph ], [ %vec.ind.next, %vector.body ]
  %vec.phi = phi <8 x i32> [ splat (i32 1), %vector.ph ], [ %5, %vector.body ]
  %vec.phi2 = phi <8 x i32> [ splat (i32 1), %vector.ph ], [ %6, %vector.body ]
  %vec.phi3 = phi <8 x i32> [ splat (i32 1), %vector.ph ], [ %7, %vector.body ]
  %vec.phi4 = phi <8 x i32> [ splat (i32 1), %vector.ph ], [ %8, %vector.body ]
  %step.add = add <8 x i32> %vec.ind, splat (i32 -8)
  %step.add.2 = add <8 x i32> %vec.ind, splat (i32 -16)
  %step.add.3 = add <8 x i32> %vec.ind, splat (i32 -24)
  %5 = mul <8 x i32> %vec.phi, %vec.ind
  %6 = mul <8 x i32> %vec.phi2, %step.add
  %7 = mul <8 x i32> %vec.phi3, %step.add.2
  %8 = mul <8 x i32> %vec.phi4, %step.add.3
  %index.next = add nuw i32 %index, 32
  %vec.ind.next = add <8 x i32> %vec.ind, splat (i32 -32)
  %9 = icmp eq i32 %index.next, %n.vec
  br i1 %9, label %middle.block, label %vector.body, !llvm.loop !6

middle.block:                                     ; preds = %vector.body
  %bin.rdx = mul <8 x i32> %6, %5
  %bin.rdx5 = mul <8 x i32> %7, %bin.rdx
  %bin.rdx6 = mul <8 x i32> %8, %bin.rdx5
  %10 = tail call i32 @llvm.vector.reduce.mul.v8i32(<8 x i32> %bin.rdx6)
  %cmp.n = icmp eq i32 %3, %n.vec
  br i1 %cmp.n, label %execute.exit, label %vec.epilog.iter.check

vec.epilog.iter.check:                            ; preds = %middle.block
  %ind.end = sub i32 %0, %n.vec
  %n.vec.remaining = and i32 %3, 28
  %min.epilog.iters.check = icmp eq i32 %n.vec.remaining, 0
  br i1 %min.epilog.iters.check, label %.lr.ph.i.preheader, label %vec.epilog.ph

vec.epilog.ph:                                    ; preds = %vec.epilog.iter.check, %vector.main.loop.iter.check
  %vec.epilog.resume.val = phi i32 [ %n.vec, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %bc.resume.val = phi i32 [ %4, %vec.epilog.iter.check ], [ %0, %vector.main.loop.iter.check ]
  %bc.merge.rdx = phi i32 [ %10, %vec.epilog.iter.check ], [ 1, %vector.main.loop.iter.check ]
  %n.vec8 = and i32 %3, -4
  %11 = sub i32 %0, %n.vec8
  %12 = insertelement <4 x i32> <i32 poison, i32 1, i32 1, i32 1>, i32 %bc.merge.rdx, i64 0
  %broadcast.splatinsert9 = insertelement <4 x i32> poison, i32 %bc.resume.val, i64 0
  %broadcast.splat10 = shufflevector <4 x i32> %broadcast.splatinsert9, <4 x i32> poison, <4 x i32> zeroinitializer
  %induction11 = add <4 x i32> %broadcast.splat10, <i32 0, i32 -1, i32 -2, i32 -3>
  br label %vec.epilog.vector.body

vec.epilog.vector.body:                           ; preds = %vec.epilog.vector.body, %vec.epilog.ph
  %index12 = phi i32 [ %vec.epilog.resume.val, %vec.epilog.ph ], [ %index.next15, %vec.epilog.vector.body ]
  %vec.ind13 = phi <4 x i32> [ %induction11, %vec.epilog.ph ], [ %vec.ind.next16, %vec.epilog.vector.body ]
  %vec.phi14 = phi <4 x i32> [ %12, %vec.epilog.ph ], [ %13, %vec.epilog.vector.body ]
  %13 = mul <4 x i32> %vec.phi14, %vec.ind13
  %index.next15 = add nuw i32 %index12, 4
  %vec.ind.next16 = add <4 x i32> %vec.ind13, splat (i32 -4)
  %14 = icmp eq i32 %index.next15, %n.vec8
  br i1 %14, label %vec.epilog.middle.block, label %vec.epilog.vector.body, !llvm.loop !7

vec.epilog.middle.block:                          ; preds = %vec.epilog.vector.body
  %15 = tail call i32 @llvm.vector.reduce.mul.v4i32(<4 x i32> %13)
  %cmp.n17 = icmp eq i32 %3, %n.vec8
  br i1 %cmp.n17, label %execute.exit, label %.lr.ph.i.preheader

.lr.ph.i.preheader:                               ; preds = %vec.epilog.iter.check, %vec.epilog.middle.block, %iter.check
  %.ph = phi i32 [ %0, %iter.check ], [ %ind.end, %vec.epilog.iter.check ], [ %11, %vec.epilog.middle.block ]
  %.ph20 = phi i32 [ 1, %iter.check ], [ %10, %vec.epilog.iter.check ], [ %15, %vec.epilog.middle.block ]
  br label %.lr.ph.i

.lr.ph.i:                                         ; preds = %.lr.ph.i.preheader, %.lr.ph.i
  %16 = phi i32 [ %19, %.lr.ph.i ], [ %.ph, %.lr.ph.i.preheader ]
  %17 = phi i32 [ %18, %.lr.ph.i ], [ %.ph20, %.lr.ph.i.preheader ]
  %18 = mul i32 %17, %16
  %19 = add nsw i32 %16, -1
  %20 = icmp samesign ugt i32 %16, 2
  br i1 %20, label %.lr.ph.i, label %execute.exit, !llvm.loop !8

execute.exit:                                     ; preds = %.lr.ph.i, %middle.block, %vec.epilog.middle.block, %1
  %.lcssa.i = phi i32 [ 1, %1 ], [ %10, %middle.block ], [ %15, %vec.epilog.middle.block ], [ %18, %.lr.ph.i ]
  ret i32 %.lcssa.i
}

; Function Attrs: nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: none)
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = icmp sgt i32 %3, 1
  br i1 %4, label %iter.check, label %execute.exit

iter.check:                                       ; preds = %1
  %5 = add nsw i32 %3, -1
  %min.iters.check = icmp ult i32 %3, 5
  br i1 %min.iters.check, label %.lr.ph.i.preheader, label %vector.main.loop.iter.check

vector.main.loop.iter.check:                      ; preds = %iter.check
  %min.iters.check1 = icmp ult i32 %3, 33
  br i1 %min.iters.check1, label %vec.epilog.ph, label %vector.ph

vector.ph:                                        ; preds = %vector.main.loop.iter.check
  %n.vec = and i32 %5, -32
  %6 = sub i32 %3, %n.vec
  %broadcast.splatinsert = insertelement <8 x i32> poison, i32 %3, i64 0
  %broadcast.splat = shufflevector <8 x i32> %broadcast.splatinsert, <8 x i32> poison, <8 x i32> zeroinitializer
  %induction = add nsw <8 x i32> %broadcast.splat, <i32 0, i32 -1, i32 -2, i32 -3, i32 -4, i32 -5, i32 -6, i32 -7>
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i32 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %vec.ind = phi <8 x i32> [ %induction, %vector.ph ], [ %vec.ind.next, %vector.body ]
  %vec.phi = phi <8 x i32> [ splat (i32 1), %vector.ph ], [ %7, %vector.body ]
  %vec.phi2 = phi <8 x i32> [ splat (i32 1), %vector.ph ], [ %8, %vector.body ]
  %vec.phi3 = phi <8 x i32> [ splat (i32 1), %vector.ph ], [ %9, %vector.body ]
  %vec.phi4 = phi <8 x i32> [ splat (i32 1), %vector.ph ], [ %10, %vector.body ]
  %step.add = add <8 x i32> %vec.ind, splat (i32 -8)
  %step.add.2 = add <8 x i32> %vec.ind, splat (i32 -16)
  %step.add.3 = add <8 x i32> %vec.ind, splat (i32 -24)
  %7 = mul <8 x i32> %vec.phi, %vec.ind
  %8 = mul <8 x i32> %vec.phi2, %step.add
  %9 = mul <8 x i32> %vec.phi3, %step.add.2
  %10 = mul <8 x i32> %vec.phi4, %step.add.3
  %index.next = add nuw i32 %index, 32
  %vec.ind.next = add <8 x i32> %vec.ind, splat (i32 -32)
  %11 = icmp eq i32 %index.next, %n.vec
  br i1 %11, label %middle.block, label %vector.body, !llvm.loop !9

middle.block:                                     ; preds = %vector.body
  %bin.rdx = mul <8 x i32> %8, %7
  %bin.rdx5 = mul <8 x i32> %9, %bin.rdx
  %bin.rdx6 = mul <8 x i32> %10, %bin.rdx5
  %12 = tail call i32 @llvm.vector.reduce.mul.v8i32(<8 x i32> %bin.rdx6)
  %cmp.n = icmp eq i32 %5, %n.vec
  br i1 %cmp.n, label %execute.exit, label %vec.epilog.iter.check

vec.epilog.iter.check:                            ; preds = %middle.block
  %ind.end = sub i32 %3, %n.vec
  %n.vec.remaining = and i32 %5, 28
  %min.epilog.iters.check = icmp eq i32 %n.vec.remaining, 0
  br i1 %min.epilog.iters.check, label %.lr.ph.i.preheader, label %vec.epilog.ph

vec.epilog.ph:                                    ; preds = %vec.epilog.iter.check, %vector.main.loop.iter.check
  %vec.epilog.resume.val = phi i32 [ %n.vec, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %bc.resume.val = phi i32 [ %6, %vec.epilog.iter.check ], [ %3, %vector.main.loop.iter.check ]
  %bc.merge.rdx = phi i32 [ %12, %vec.epilog.iter.check ], [ 1, %vector.main.loop.iter.check ]
  %n.vec8 = and i32 %5, -4
  %13 = sub i32 %3, %n.vec8
  %14 = insertelement <4 x i32> <i32 poison, i32 1, i32 1, i32 1>, i32 %bc.merge.rdx, i64 0
  %broadcast.splatinsert9 = insertelement <4 x i32> poison, i32 %bc.resume.val, i64 0
  %broadcast.splat10 = shufflevector <4 x i32> %broadcast.splatinsert9, <4 x i32> poison, <4 x i32> zeroinitializer
  %induction11 = add <4 x i32> %broadcast.splat10, <i32 0, i32 -1, i32 -2, i32 -3>
  br label %vec.epilog.vector.body

vec.epilog.vector.body:                           ; preds = %vec.epilog.vector.body, %vec.epilog.ph
  %index12 = phi i32 [ %vec.epilog.resume.val, %vec.epilog.ph ], [ %index.next15, %vec.epilog.vector.body ]
  %vec.ind13 = phi <4 x i32> [ %induction11, %vec.epilog.ph ], [ %vec.ind.next16, %vec.epilog.vector.body ]
  %vec.phi14 = phi <4 x i32> [ %14, %vec.epilog.ph ], [ %15, %vec.epilog.vector.body ]
  %15 = mul <4 x i32> %vec.phi14, %vec.ind13
  %index.next15 = add nuw i32 %index12, 4
  %vec.ind.next16 = add <4 x i32> %vec.ind13, splat (i32 -4)
  %16 = icmp eq i32 %index.next15, %n.vec8
  br i1 %16, label %vec.epilog.middle.block, label %vec.epilog.vector.body, !llvm.loop !10

vec.epilog.middle.block:                          ; preds = %vec.epilog.vector.body
  %17 = tail call i32 @llvm.vector.reduce.mul.v4i32(<4 x i32> %15)
  %cmp.n17 = icmp eq i32 %5, %n.vec8
  br i1 %cmp.n17, label %execute.exit, label %.lr.ph.i.preheader

.lr.ph.i.preheader:                               ; preds = %vec.epilog.iter.check, %vec.epilog.middle.block, %iter.check
  %.ph = phi i32 [ %3, %iter.check ], [ %ind.end, %vec.epilog.iter.check ], [ %13, %vec.epilog.middle.block ]
  %.ph20 = phi i32 [ 1, %iter.check ], [ %12, %vec.epilog.iter.check ], [ %17, %vec.epilog.middle.block ]
  br label %.lr.ph.i

.lr.ph.i:                                         ; preds = %.lr.ph.i.preheader, %.lr.ph.i
  %18 = phi i32 [ %21, %.lr.ph.i ], [ %.ph, %.lr.ph.i.preheader ]
  %19 = phi i32 [ %20, %.lr.ph.i ], [ %.ph20, %.lr.ph.i.preheader ]
  %20 = mul i32 %19, %18
  %21 = add nsw i32 %18, -1
  %22 = icmp samesign ugt i32 %18, 2
  br i1 %22, label %.lr.ph.i, label %execute.exit, !llvm.loop !11

execute.exit:                                     ; preds = %.lr.ph.i, %middle.block, %vec.epilog.middle.block, %1
  %.lcssa.i = phi i32 [ 1, %1 ], [ %12, %middle.block ], [ %17, %vec.epilog.middle.block ], [ %20, %.lr.ph.i ]
  %23 = getelementptr i8, ptr %0, i64 8
  %24 = load ptr, ptr %23, align 8
  store i32 %.lcssa.i, ptr %24, align 4
  ret void
}

; Function Attrs: nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: none)
define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = icmp sgt i32 %3, 1
  br i1 %4, label %iter.check, label %_mlir_ciface_execute.exit

iter.check:                                       ; preds = %1
  %5 = add nsw i32 %3, -1
  %min.iters.check = icmp ult i32 %3, 5
  br i1 %min.iters.check, label %.lr.ph.i.i.preheader, label %vector.main.loop.iter.check

vector.main.loop.iter.check:                      ; preds = %iter.check
  %min.iters.check1 = icmp ult i32 %3, 33
  br i1 %min.iters.check1, label %vec.epilog.ph, label %vector.ph

vector.ph:                                        ; preds = %vector.main.loop.iter.check
  %n.vec = and i32 %5, -32
  %6 = sub i32 %3, %n.vec
  %broadcast.splatinsert = insertelement <8 x i32> poison, i32 %3, i64 0
  %broadcast.splat = shufflevector <8 x i32> %broadcast.splatinsert, <8 x i32> poison, <8 x i32> zeroinitializer
  %induction = add nsw <8 x i32> %broadcast.splat, <i32 0, i32 -1, i32 -2, i32 -3, i32 -4, i32 -5, i32 -6, i32 -7>
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i32 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %vec.ind = phi <8 x i32> [ %induction, %vector.ph ], [ %vec.ind.next, %vector.body ]
  %vec.phi = phi <8 x i32> [ splat (i32 1), %vector.ph ], [ %7, %vector.body ]
  %vec.phi2 = phi <8 x i32> [ splat (i32 1), %vector.ph ], [ %8, %vector.body ]
  %vec.phi3 = phi <8 x i32> [ splat (i32 1), %vector.ph ], [ %9, %vector.body ]
  %vec.phi4 = phi <8 x i32> [ splat (i32 1), %vector.ph ], [ %10, %vector.body ]
  %step.add = add <8 x i32> %vec.ind, splat (i32 -8)
  %step.add.2 = add <8 x i32> %vec.ind, splat (i32 -16)
  %step.add.3 = add <8 x i32> %vec.ind, splat (i32 -24)
  %7 = mul <8 x i32> %vec.phi, %vec.ind
  %8 = mul <8 x i32> %vec.phi2, %step.add
  %9 = mul <8 x i32> %vec.phi3, %step.add.2
  %10 = mul <8 x i32> %vec.phi4, %step.add.3
  %index.next = add nuw i32 %index, 32
  %vec.ind.next = add <8 x i32> %vec.ind, splat (i32 -32)
  %11 = icmp eq i32 %index.next, %n.vec
  br i1 %11, label %middle.block, label %vector.body, !llvm.loop !12

middle.block:                                     ; preds = %vector.body
  %bin.rdx = mul <8 x i32> %8, %7
  %bin.rdx5 = mul <8 x i32> %9, %bin.rdx
  %bin.rdx6 = mul <8 x i32> %10, %bin.rdx5
  %12 = tail call i32 @llvm.vector.reduce.mul.v8i32(<8 x i32> %bin.rdx6)
  %cmp.n = icmp eq i32 %5, %n.vec
  br i1 %cmp.n, label %_mlir_ciface_execute.exit, label %vec.epilog.iter.check

vec.epilog.iter.check:                            ; preds = %middle.block
  %ind.end = sub i32 %3, %n.vec
  %n.vec.remaining = and i32 %5, 28
  %min.epilog.iters.check = icmp eq i32 %n.vec.remaining, 0
  br i1 %min.epilog.iters.check, label %.lr.ph.i.i.preheader, label %vec.epilog.ph

vec.epilog.ph:                                    ; preds = %vec.epilog.iter.check, %vector.main.loop.iter.check
  %vec.epilog.resume.val = phi i32 [ %n.vec, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %bc.resume.val = phi i32 [ %6, %vec.epilog.iter.check ], [ %3, %vector.main.loop.iter.check ]
  %bc.merge.rdx = phi i32 [ %12, %vec.epilog.iter.check ], [ 1, %vector.main.loop.iter.check ]
  %n.vec8 = and i32 %5, -4
  %13 = sub i32 %3, %n.vec8
  %14 = insertelement <4 x i32> <i32 poison, i32 1, i32 1, i32 1>, i32 %bc.merge.rdx, i64 0
  %broadcast.splatinsert9 = insertelement <4 x i32> poison, i32 %bc.resume.val, i64 0
  %broadcast.splat10 = shufflevector <4 x i32> %broadcast.splatinsert9, <4 x i32> poison, <4 x i32> zeroinitializer
  %induction11 = add <4 x i32> %broadcast.splat10, <i32 0, i32 -1, i32 -2, i32 -3>
  br label %vec.epilog.vector.body

vec.epilog.vector.body:                           ; preds = %vec.epilog.vector.body, %vec.epilog.ph
  %index12 = phi i32 [ %vec.epilog.resume.val, %vec.epilog.ph ], [ %index.next15, %vec.epilog.vector.body ]
  %vec.ind13 = phi <4 x i32> [ %induction11, %vec.epilog.ph ], [ %vec.ind.next16, %vec.epilog.vector.body ]
  %vec.phi14 = phi <4 x i32> [ %14, %vec.epilog.ph ], [ %15, %vec.epilog.vector.body ]
  %15 = mul <4 x i32> %vec.phi14, %vec.ind13
  %index.next15 = add nuw i32 %index12, 4
  %vec.ind.next16 = add <4 x i32> %vec.ind13, splat (i32 -4)
  %16 = icmp eq i32 %index.next15, %n.vec8
  br i1 %16, label %vec.epilog.middle.block, label %vec.epilog.vector.body, !llvm.loop !13

vec.epilog.middle.block:                          ; preds = %vec.epilog.vector.body
  %17 = tail call i32 @llvm.vector.reduce.mul.v4i32(<4 x i32> %15)
  %cmp.n17 = icmp eq i32 %5, %n.vec8
  br i1 %cmp.n17, label %_mlir_ciface_execute.exit, label %.lr.ph.i.i.preheader

.lr.ph.i.i.preheader:                             ; preds = %vec.epilog.iter.check, %vec.epilog.middle.block, %iter.check
  %.ph = phi i32 [ %3, %iter.check ], [ %ind.end, %vec.epilog.iter.check ], [ %13, %vec.epilog.middle.block ]
  %.ph20 = phi i32 [ 1, %iter.check ], [ %12, %vec.epilog.iter.check ], [ %17, %vec.epilog.middle.block ]
  br label %.lr.ph.i.i

.lr.ph.i.i:                                       ; preds = %.lr.ph.i.i.preheader, %.lr.ph.i.i
  %18 = phi i32 [ %21, %.lr.ph.i.i ], [ %.ph, %.lr.ph.i.i.preheader ]
  %19 = phi i32 [ %20, %.lr.ph.i.i ], [ %.ph20, %.lr.ph.i.i.preheader ]
  %20 = mul i32 %19, %18
  %21 = add nsw i32 %18, -1
  %22 = icmp samesign ugt i32 %18, 2
  br i1 %22, label %.lr.ph.i.i, label %_mlir_ciface_execute.exit, !llvm.loop !14

_mlir_ciface_execute.exit:                        ; preds = %.lr.ph.i.i, %middle.block, %vec.epilog.middle.block, %1
  %.lcssa.i.i = phi i32 [ 1, %1 ], [ %12, %middle.block ], [ %17, %vec.epilog.middle.block ], [ %20, %.lr.ph.i.i ]
  %23 = getelementptr i8, ptr %0, i64 8
  %24 = load ptr, ptr %23, align 8
  store i32 %.lcssa.i.i, ptr %24, align 4
  ret void
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i32 @llvm.vector.reduce.mul.v8i32(<8 x i32>) #3

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i32 @llvm.vector.reduce.mul.v4i32(<4 x i32>) #3

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
