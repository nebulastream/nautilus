; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nofree norecurse nosync nounwind memory(argmem: read)
define signext i32 @execute(ptr readonly %0, i32 %1) local_unnamed_addr #0 {
  %3 = icmp sgt i32 %1, 0
  br i1 %3, label %iter.check, label %._crit_edge

iter.check:                                       ; preds = %2
  %wide.trip.count = zext nneg i32 %1 to i64
  %min.iters.check = icmp ult i32 %1, 4
  br i1 %min.iters.check, label %.lr.ph.preheader, label %vector.main.loop.iter.check

vector.main.loop.iter.check:                      ; preds = %iter.check
  %min.iters.check6 = icmp ult i32 %1, 32
  br i1 %min.iters.check6, label %vec.epilog.ph, label %vector.ph

vector.ph:                                        ; preds = %vector.main.loop.iter.check
  %n.vec = and i64 %wide.trip.count, 2147483616
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i64 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %vec.phi = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %9, %vector.body ]
  %vec.phi7 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %10, %vector.body ]
  %vec.phi8 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %11, %vector.body ]
  %vec.phi9 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %12, %vector.body ]
  %4 = shl nuw nsw i64 %index, 2
  %5 = getelementptr i8, ptr %0, i64 %4
  %6 = getelementptr i8, ptr %5, i64 32
  %7 = getelementptr i8, ptr %5, i64 64
  %8 = getelementptr i8, ptr %5, i64 96
  %wide.load = load <8 x i32>, ptr %5, align 4
  %wide.load10 = load <8 x i32>, ptr %6, align 4
  %wide.load11 = load <8 x i32>, ptr %7, align 4
  %wide.load12 = load <8 x i32>, ptr %8, align 4
  %9 = add <8 x i32> %wide.load, %vec.phi
  %10 = add <8 x i32> %wide.load10, %vec.phi7
  %11 = add <8 x i32> %wide.load11, %vec.phi8
  %12 = add <8 x i32> %wide.load12, %vec.phi9
  %index.next = add nuw i64 %index, 32
  %13 = icmp eq i64 %index.next, %n.vec
  br i1 %13, label %middle.block, label %vector.body, !llvm.loop !1

middle.block:                                     ; preds = %vector.body
  %bin.rdx = add <8 x i32> %10, %9
  %bin.rdx13 = add <8 x i32> %11, %bin.rdx
  %bin.rdx14 = add <8 x i32> %12, %bin.rdx13
  %14 = tail call i32 @llvm.vector.reduce.add.v8i32(<8 x i32> %bin.rdx14)
  %cmp.n = icmp eq i64 %n.vec, %wide.trip.count
  br i1 %cmp.n, label %._crit_edge, label %vec.epilog.iter.check

vec.epilog.iter.check:                            ; preds = %middle.block
  %n.vec.remaining = and i64 %wide.trip.count, 28
  %min.epilog.iters.check = icmp eq i64 %n.vec.remaining, 0
  br i1 %min.epilog.iters.check, label %.lr.ph.preheader, label %vec.epilog.ph

vec.epilog.ph:                                    ; preds = %vec.epilog.iter.check, %vector.main.loop.iter.check
  %vec.epilog.resume.val = phi i64 [ %n.vec, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %bc.merge.rdx = phi i32 [ %14, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %n.vec16 = and i64 %wide.trip.count, 2147483644
  %15 = insertelement <4 x i32> <i32 poison, i32 0, i32 0, i32 0>, i32 %bc.merge.rdx, i64 0
  br label %vec.epilog.vector.body

vec.epilog.vector.body:                           ; preds = %vec.epilog.vector.body, %vec.epilog.ph
  %index17 = phi i64 [ %vec.epilog.resume.val, %vec.epilog.ph ], [ %index.next20, %vec.epilog.vector.body ]
  %vec.phi18 = phi <4 x i32> [ %15, %vec.epilog.ph ], [ %18, %vec.epilog.vector.body ]
  %16 = shl nuw nsw i64 %index17, 2
  %17 = getelementptr i8, ptr %0, i64 %16
  %wide.load19 = load <4 x i32>, ptr %17, align 4
  %18 = add <4 x i32> %wide.load19, %vec.phi18
  %index.next20 = add nuw i64 %index17, 4
  %19 = icmp eq i64 %index.next20, %n.vec16
  br i1 %19, label %vec.epilog.middle.block, label %vec.epilog.vector.body, !llvm.loop !4

vec.epilog.middle.block:                          ; preds = %vec.epilog.vector.body
  %20 = tail call i32 @llvm.vector.reduce.add.v4i32(<4 x i32> %18)
  %cmp.n21 = icmp eq i64 %n.vec16, %wide.trip.count
  br i1 %cmp.n21, label %._crit_edge, label %.lr.ph.preheader

.lr.ph.preheader:                                 ; preds = %vec.epilog.iter.check, %vec.epilog.middle.block, %iter.check
  %indvars.iv.ph = phi i64 [ 0, %iter.check ], [ %n.vec, %vec.epilog.iter.check ], [ %n.vec16, %vec.epilog.middle.block ]
  %.ph = phi i32 [ 0, %iter.check ], [ %14, %vec.epilog.iter.check ], [ %20, %vec.epilog.middle.block ]
  br label %.lr.ph

.lr.ph:                                           ; preds = %.lr.ph.preheader, %.lr.ph
  %indvars.iv = phi i64 [ %indvars.iv.next, %.lr.ph ], [ %indvars.iv.ph, %.lr.ph.preheader ]
  %21 = phi i32 [ %25, %.lr.ph ], [ %.ph, %.lr.ph.preheader ]
  %22 = shl nuw nsw i64 %indvars.iv, 2
  %23 = getelementptr i8, ptr %0, i64 %22
  %24 = load i32, ptr %23, align 4
  %25 = add i32 %24, %21
  %indvars.iv.next = add nuw nsw i64 %indvars.iv, 1
  %exitcond.not = icmp eq i64 %indvars.iv.next, %wide.trip.count
  br i1 %exitcond.not, label %._crit_edge, label %.lr.ph, !llvm.loop !5

._crit_edge:                                      ; preds = %.lr.ph, %middle.block, %vec.epilog.middle.block, %2
  %.lcssa = phi i32 [ 0, %2 ], [ %14, %middle.block ], [ %20, %vec.epilog.middle.block ], [ %25, %.lr.ph ]
  ret i32 %.lcssa
}

; Function Attrs: nofree norecurse nosync nounwind memory(argmem: read)
define signext i32 @_mlir_ciface_execute(ptr readonly %0, i32 %1) local_unnamed_addr #1 {
  %3 = icmp sgt i32 %1, 0
  br i1 %3, label %iter.check, label %execute.exit

iter.check:                                       ; preds = %2
  %wide.trip.count.i = zext nneg i32 %1 to i64
  %min.iters.check = icmp ult i32 %1, 4
  br i1 %min.iters.check, label %.lr.ph.i.preheader, label %vector.main.loop.iter.check

vector.main.loop.iter.check:                      ; preds = %iter.check
  %min.iters.check1 = icmp ult i32 %1, 32
  br i1 %min.iters.check1, label %vec.epilog.ph, label %vector.ph

vector.ph:                                        ; preds = %vector.main.loop.iter.check
  %n.vec = and i64 %wide.trip.count.i, 2147483616
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i64 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %vec.phi = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %9, %vector.body ]
  %vec.phi2 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %10, %vector.body ]
  %vec.phi3 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %11, %vector.body ]
  %vec.phi4 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %12, %vector.body ]
  %4 = shl nuw nsw i64 %index, 2
  %5 = getelementptr i8, ptr %0, i64 %4
  %6 = getelementptr i8, ptr %5, i64 32
  %7 = getelementptr i8, ptr %5, i64 64
  %8 = getelementptr i8, ptr %5, i64 96
  %wide.load = load <8 x i32>, ptr %5, align 4
  %wide.load5 = load <8 x i32>, ptr %6, align 4
  %wide.load6 = load <8 x i32>, ptr %7, align 4
  %wide.load7 = load <8 x i32>, ptr %8, align 4
  %9 = add <8 x i32> %wide.load, %vec.phi
  %10 = add <8 x i32> %wide.load5, %vec.phi2
  %11 = add <8 x i32> %wide.load6, %vec.phi3
  %12 = add <8 x i32> %wide.load7, %vec.phi4
  %index.next = add nuw i64 %index, 32
  %13 = icmp eq i64 %index.next, %n.vec
  br i1 %13, label %middle.block, label %vector.body, !llvm.loop !6

middle.block:                                     ; preds = %vector.body
  %bin.rdx = add <8 x i32> %10, %9
  %bin.rdx8 = add <8 x i32> %11, %bin.rdx
  %bin.rdx9 = add <8 x i32> %12, %bin.rdx8
  %14 = tail call i32 @llvm.vector.reduce.add.v8i32(<8 x i32> %bin.rdx9)
  %cmp.n = icmp eq i64 %n.vec, %wide.trip.count.i
  br i1 %cmp.n, label %execute.exit, label %vec.epilog.iter.check

vec.epilog.iter.check:                            ; preds = %middle.block
  %n.vec.remaining = and i64 %wide.trip.count.i, 28
  %min.epilog.iters.check = icmp eq i64 %n.vec.remaining, 0
  br i1 %min.epilog.iters.check, label %.lr.ph.i.preheader, label %vec.epilog.ph

vec.epilog.ph:                                    ; preds = %vec.epilog.iter.check, %vector.main.loop.iter.check
  %vec.epilog.resume.val = phi i64 [ %n.vec, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %bc.merge.rdx = phi i32 [ %14, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %n.vec11 = and i64 %wide.trip.count.i, 2147483644
  %15 = insertelement <4 x i32> <i32 poison, i32 0, i32 0, i32 0>, i32 %bc.merge.rdx, i64 0
  br label %vec.epilog.vector.body

vec.epilog.vector.body:                           ; preds = %vec.epilog.vector.body, %vec.epilog.ph
  %index12 = phi i64 [ %vec.epilog.resume.val, %vec.epilog.ph ], [ %index.next15, %vec.epilog.vector.body ]
  %vec.phi13 = phi <4 x i32> [ %15, %vec.epilog.ph ], [ %18, %vec.epilog.vector.body ]
  %16 = shl nuw nsw i64 %index12, 2
  %17 = getelementptr i8, ptr %0, i64 %16
  %wide.load14 = load <4 x i32>, ptr %17, align 4
  %18 = add <4 x i32> %wide.load14, %vec.phi13
  %index.next15 = add nuw i64 %index12, 4
  %19 = icmp eq i64 %index.next15, %n.vec11
  br i1 %19, label %vec.epilog.middle.block, label %vec.epilog.vector.body, !llvm.loop !7

vec.epilog.middle.block:                          ; preds = %vec.epilog.vector.body
  %20 = tail call i32 @llvm.vector.reduce.add.v4i32(<4 x i32> %18)
  %cmp.n16 = icmp eq i64 %n.vec11, %wide.trip.count.i
  br i1 %cmp.n16, label %execute.exit, label %.lr.ph.i.preheader

.lr.ph.i.preheader:                               ; preds = %vec.epilog.iter.check, %vec.epilog.middle.block, %iter.check
  %indvars.iv.i.ph = phi i64 [ 0, %iter.check ], [ %n.vec, %vec.epilog.iter.check ], [ %n.vec11, %vec.epilog.middle.block ]
  %.ph = phi i32 [ 0, %iter.check ], [ %14, %vec.epilog.iter.check ], [ %20, %vec.epilog.middle.block ]
  br label %.lr.ph.i

.lr.ph.i:                                         ; preds = %.lr.ph.i.preheader, %.lr.ph.i
  %indvars.iv.i = phi i64 [ %indvars.iv.next.i, %.lr.ph.i ], [ %indvars.iv.i.ph, %.lr.ph.i.preheader ]
  %21 = phi i32 [ %25, %.lr.ph.i ], [ %.ph, %.lr.ph.i.preheader ]
  %22 = shl nuw nsw i64 %indvars.iv.i, 2
  %23 = getelementptr i8, ptr %0, i64 %22
  %24 = load i32, ptr %23, align 4
  %25 = add i32 %24, %21
  %indvars.iv.next.i = add nuw nsw i64 %indvars.iv.i, 1
  %exitcond.not.i = icmp eq i64 %indvars.iv.next.i, %wide.trip.count.i
  br i1 %exitcond.not.i, label %execute.exit, label %.lr.ph.i, !llvm.loop !8

execute.exit:                                     ; preds = %.lr.ph.i, %middle.block, %vec.epilog.middle.block, %2
  %.lcssa.i = phi i32 [ 0, %2 ], [ %14, %middle.block ], [ %20, %vec.epilog.middle.block ], [ %25, %.lr.ph.i ]
  ret i32 %.lcssa.i
}

; Function Attrs: nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: none)
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load i32, ptr %5, align 4
  %7 = icmp sgt i32 %6, 0
  br i1 %7, label %iter.check, label %execute.exit

iter.check:                                       ; preds = %1
  %wide.trip.count.i = zext nneg i32 %6 to i64
  %min.iters.check = icmp ult i32 %6, 4
  br i1 %min.iters.check, label %.lr.ph.i.preheader, label %vector.main.loop.iter.check

vector.main.loop.iter.check:                      ; preds = %iter.check
  %min.iters.check1 = icmp ult i32 %6, 32
  br i1 %min.iters.check1, label %vec.epilog.ph, label %vector.ph

vector.ph:                                        ; preds = %vector.main.loop.iter.check
  %n.vec = and i64 %wide.trip.count.i, 2147483616
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i64 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %vec.phi = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %13, %vector.body ]
  %vec.phi2 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %14, %vector.body ]
  %vec.phi3 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %15, %vector.body ]
  %vec.phi4 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %16, %vector.body ]
  %8 = shl nuw nsw i64 %index, 2
  %9 = getelementptr i8, ptr %3, i64 %8
  %10 = getelementptr i8, ptr %9, i64 32
  %11 = getelementptr i8, ptr %9, i64 64
  %12 = getelementptr i8, ptr %9, i64 96
  %wide.load = load <8 x i32>, ptr %9, align 4
  %wide.load5 = load <8 x i32>, ptr %10, align 4
  %wide.load6 = load <8 x i32>, ptr %11, align 4
  %wide.load7 = load <8 x i32>, ptr %12, align 4
  %13 = add <8 x i32> %wide.load, %vec.phi
  %14 = add <8 x i32> %wide.load5, %vec.phi2
  %15 = add <8 x i32> %wide.load6, %vec.phi3
  %16 = add <8 x i32> %wide.load7, %vec.phi4
  %index.next = add nuw i64 %index, 32
  %17 = icmp eq i64 %index.next, %n.vec
  br i1 %17, label %middle.block, label %vector.body, !llvm.loop !9

middle.block:                                     ; preds = %vector.body
  %bin.rdx = add <8 x i32> %14, %13
  %bin.rdx8 = add <8 x i32> %15, %bin.rdx
  %bin.rdx9 = add <8 x i32> %16, %bin.rdx8
  %18 = tail call i32 @llvm.vector.reduce.add.v8i32(<8 x i32> %bin.rdx9)
  %cmp.n = icmp eq i64 %n.vec, %wide.trip.count.i
  br i1 %cmp.n, label %execute.exit, label %vec.epilog.iter.check

vec.epilog.iter.check:                            ; preds = %middle.block
  %n.vec.remaining = and i64 %wide.trip.count.i, 28
  %min.epilog.iters.check = icmp eq i64 %n.vec.remaining, 0
  br i1 %min.epilog.iters.check, label %.lr.ph.i.preheader, label %vec.epilog.ph

vec.epilog.ph:                                    ; preds = %vec.epilog.iter.check, %vector.main.loop.iter.check
  %vec.epilog.resume.val = phi i64 [ %n.vec, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %bc.merge.rdx = phi i32 [ %18, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %n.vec11 = and i64 %wide.trip.count.i, 2147483644
  %19 = insertelement <4 x i32> <i32 poison, i32 0, i32 0, i32 0>, i32 %bc.merge.rdx, i64 0
  br label %vec.epilog.vector.body

vec.epilog.vector.body:                           ; preds = %vec.epilog.vector.body, %vec.epilog.ph
  %index12 = phi i64 [ %vec.epilog.resume.val, %vec.epilog.ph ], [ %index.next15, %vec.epilog.vector.body ]
  %vec.phi13 = phi <4 x i32> [ %19, %vec.epilog.ph ], [ %22, %vec.epilog.vector.body ]
  %20 = shl nuw nsw i64 %index12, 2
  %21 = getelementptr i8, ptr %3, i64 %20
  %wide.load14 = load <4 x i32>, ptr %21, align 4
  %22 = add <4 x i32> %wide.load14, %vec.phi13
  %index.next15 = add nuw i64 %index12, 4
  %23 = icmp eq i64 %index.next15, %n.vec11
  br i1 %23, label %vec.epilog.middle.block, label %vec.epilog.vector.body, !llvm.loop !10

vec.epilog.middle.block:                          ; preds = %vec.epilog.vector.body
  %24 = tail call i32 @llvm.vector.reduce.add.v4i32(<4 x i32> %22)
  %cmp.n16 = icmp eq i64 %n.vec11, %wide.trip.count.i
  br i1 %cmp.n16, label %execute.exit, label %.lr.ph.i.preheader

.lr.ph.i.preheader:                               ; preds = %vec.epilog.iter.check, %vec.epilog.middle.block, %iter.check
  %indvars.iv.i.ph = phi i64 [ 0, %iter.check ], [ %n.vec, %vec.epilog.iter.check ], [ %n.vec11, %vec.epilog.middle.block ]
  %.ph = phi i32 [ 0, %iter.check ], [ %18, %vec.epilog.iter.check ], [ %24, %vec.epilog.middle.block ]
  br label %.lr.ph.i

.lr.ph.i:                                         ; preds = %.lr.ph.i.preheader, %.lr.ph.i
  %indvars.iv.i = phi i64 [ %indvars.iv.next.i, %.lr.ph.i ], [ %indvars.iv.i.ph, %.lr.ph.i.preheader ]
  %25 = phi i32 [ %29, %.lr.ph.i ], [ %.ph, %.lr.ph.i.preheader ]
  %26 = shl nuw nsw i64 %indvars.iv.i, 2
  %27 = getelementptr i8, ptr %3, i64 %26
  %28 = load i32, ptr %27, align 4
  %29 = add i32 %28, %25
  %indvars.iv.next.i = add nuw nsw i64 %indvars.iv.i, 1
  %exitcond.not.i = icmp eq i64 %indvars.iv.next.i, %wide.trip.count.i
  br i1 %exitcond.not.i, label %execute.exit, label %.lr.ph.i, !llvm.loop !11

execute.exit:                                     ; preds = %.lr.ph.i, %middle.block, %vec.epilog.middle.block, %1
  %.lcssa.i = phi i32 [ 0, %1 ], [ %18, %middle.block ], [ %24, %vec.epilog.middle.block ], [ %29, %.lr.ph.i ]
  %30 = getelementptr i8, ptr %0, i64 16
  %31 = load ptr, ptr %30, align 8
  store i32 %.lcssa.i, ptr %31, align 4
  ret void
}

; Function Attrs: nofree norecurse nosync nounwind memory(readwrite, inaccessiblemem: none)
define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load i32, ptr %5, align 4
  %7 = icmp sgt i32 %6, 0
  br i1 %7, label %iter.check, label %_mlir_ciface_execute.exit

iter.check:                                       ; preds = %1
  %wide.trip.count.i.i = zext nneg i32 %6 to i64
  %min.iters.check = icmp ult i32 %6, 4
  br i1 %min.iters.check, label %.lr.ph.i.i.preheader, label %vector.main.loop.iter.check

vector.main.loop.iter.check:                      ; preds = %iter.check
  %min.iters.check1 = icmp ult i32 %6, 32
  br i1 %min.iters.check1, label %vec.epilog.ph, label %vector.ph

vector.ph:                                        ; preds = %vector.main.loop.iter.check
  %n.vec = and i64 %wide.trip.count.i.i, 2147483616
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %vector.ph
  %index = phi i64 [ 0, %vector.ph ], [ %index.next, %vector.body ]
  %vec.phi = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %13, %vector.body ]
  %vec.phi2 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %14, %vector.body ]
  %vec.phi3 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %15, %vector.body ]
  %vec.phi4 = phi <8 x i32> [ zeroinitializer, %vector.ph ], [ %16, %vector.body ]
  %8 = shl nuw nsw i64 %index, 2
  %9 = getelementptr i8, ptr %3, i64 %8
  %10 = getelementptr i8, ptr %9, i64 32
  %11 = getelementptr i8, ptr %9, i64 64
  %12 = getelementptr i8, ptr %9, i64 96
  %wide.load = load <8 x i32>, ptr %9, align 4
  %wide.load5 = load <8 x i32>, ptr %10, align 4
  %wide.load6 = load <8 x i32>, ptr %11, align 4
  %wide.load7 = load <8 x i32>, ptr %12, align 4
  %13 = add <8 x i32> %wide.load, %vec.phi
  %14 = add <8 x i32> %wide.load5, %vec.phi2
  %15 = add <8 x i32> %wide.load6, %vec.phi3
  %16 = add <8 x i32> %wide.load7, %vec.phi4
  %index.next = add nuw i64 %index, 32
  %17 = icmp eq i64 %index.next, %n.vec
  br i1 %17, label %middle.block, label %vector.body, !llvm.loop !12

middle.block:                                     ; preds = %vector.body
  %bin.rdx = add <8 x i32> %14, %13
  %bin.rdx8 = add <8 x i32> %15, %bin.rdx
  %bin.rdx9 = add <8 x i32> %16, %bin.rdx8
  %18 = tail call i32 @llvm.vector.reduce.add.v8i32(<8 x i32> %bin.rdx9)
  %cmp.n = icmp eq i64 %n.vec, %wide.trip.count.i.i
  br i1 %cmp.n, label %_mlir_ciface_execute.exit, label %vec.epilog.iter.check

vec.epilog.iter.check:                            ; preds = %middle.block
  %n.vec.remaining = and i64 %wide.trip.count.i.i, 28
  %min.epilog.iters.check = icmp eq i64 %n.vec.remaining, 0
  br i1 %min.epilog.iters.check, label %.lr.ph.i.i.preheader, label %vec.epilog.ph

vec.epilog.ph:                                    ; preds = %vec.epilog.iter.check, %vector.main.loop.iter.check
  %vec.epilog.resume.val = phi i64 [ %n.vec, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %bc.merge.rdx = phi i32 [ %18, %vec.epilog.iter.check ], [ 0, %vector.main.loop.iter.check ]
  %n.vec11 = and i64 %wide.trip.count.i.i, 2147483644
  %19 = insertelement <4 x i32> <i32 poison, i32 0, i32 0, i32 0>, i32 %bc.merge.rdx, i64 0
  br label %vec.epilog.vector.body

vec.epilog.vector.body:                           ; preds = %vec.epilog.vector.body, %vec.epilog.ph
  %index12 = phi i64 [ %vec.epilog.resume.val, %vec.epilog.ph ], [ %index.next15, %vec.epilog.vector.body ]
  %vec.phi13 = phi <4 x i32> [ %19, %vec.epilog.ph ], [ %22, %vec.epilog.vector.body ]
  %20 = shl nuw nsw i64 %index12, 2
  %21 = getelementptr i8, ptr %3, i64 %20
  %wide.load14 = load <4 x i32>, ptr %21, align 4
  %22 = add <4 x i32> %wide.load14, %vec.phi13
  %index.next15 = add nuw i64 %index12, 4
  %23 = icmp eq i64 %index.next15, %n.vec11
  br i1 %23, label %vec.epilog.middle.block, label %vec.epilog.vector.body, !llvm.loop !13

vec.epilog.middle.block:                          ; preds = %vec.epilog.vector.body
  %24 = tail call i32 @llvm.vector.reduce.add.v4i32(<4 x i32> %22)
  %cmp.n16 = icmp eq i64 %n.vec11, %wide.trip.count.i.i
  br i1 %cmp.n16, label %_mlir_ciface_execute.exit, label %.lr.ph.i.i.preheader

.lr.ph.i.i.preheader:                             ; preds = %vec.epilog.iter.check, %vec.epilog.middle.block, %iter.check
  %indvars.iv.i.i.ph = phi i64 [ 0, %iter.check ], [ %n.vec, %vec.epilog.iter.check ], [ %n.vec11, %vec.epilog.middle.block ]
  %.ph = phi i32 [ 0, %iter.check ], [ %18, %vec.epilog.iter.check ], [ %24, %vec.epilog.middle.block ]
  br label %.lr.ph.i.i

.lr.ph.i.i:                                       ; preds = %.lr.ph.i.i.preheader, %.lr.ph.i.i
  %indvars.iv.i.i = phi i64 [ %indvars.iv.next.i.i, %.lr.ph.i.i ], [ %indvars.iv.i.i.ph, %.lr.ph.i.i.preheader ]
  %25 = phi i32 [ %29, %.lr.ph.i.i ], [ %.ph, %.lr.ph.i.i.preheader ]
  %26 = shl nuw nsw i64 %indvars.iv.i.i, 2
  %27 = getelementptr i8, ptr %3, i64 %26
  %28 = load i32, ptr %27, align 4
  %29 = add i32 %28, %25
  %indvars.iv.next.i.i = add nuw nsw i64 %indvars.iv.i.i, 1
  %exitcond.not.i.i = icmp eq i64 %indvars.iv.next.i.i, %wide.trip.count.i.i
  br i1 %exitcond.not.i.i, label %_mlir_ciface_execute.exit, label %.lr.ph.i.i, !llvm.loop !14

_mlir_ciface_execute.exit:                        ; preds = %.lr.ph.i.i, %middle.block, %vec.epilog.middle.block, %1
  %.lcssa.i.i = phi i32 [ 0, %1 ], [ %18, %middle.block ], [ %24, %vec.epilog.middle.block ], [ %29, %.lr.ph.i.i ]
  %30 = getelementptr i8, ptr %0, i64 16
  %31 = load ptr, ptr %30, align 8
  store i32 %.lcssa.i.i, ptr %31, align 4
  ret void
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i32 @llvm.vector.reduce.add.v8i32(<8 x i32>) #3

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i32 @llvm.vector.reduce.add.v4i32(<4 x i32>) #3

attributes #0 = { nofree norecurse nosync nounwind memory(argmem: read) }
attributes #1 = { nofree norecurse nosync nounwind memory(argmem: read) }
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
