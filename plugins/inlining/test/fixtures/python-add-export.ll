; Representative Numba-generated UDF module. The exported wrapper has a
; straightforward ABI suitable for nautilus::invoke<int64_t>(...).
source_filename = "<string>"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

define internal noundef i32 @_ZN8__main__3addB2v1B52c8tJTIeFIjxB2IKSgI4CrvQClUYkACQB1EiFSRRB9GgCAA_3d_3dExx(
    ptr noalias writeonly captures(none) initializes((0, 8)) %retptr,
    ptr noalias readnone captures(none) %excinfo,
    i64 %arg.a,
    i64 %arg.b) local_unnamed_addr #0 {
B0:
  %.6 = add nsw i64 %arg.b, %arg.a
  store i64 %.6, ptr %retptr, align 8
  ret i32 0
}

define internal i64 @cfunc._ZN8__main__3addB2v1B52c8tJTIeFIjxB2IKSgI4CrvQClUYkACQB1EiFSRRB9GgCAA_3d_3dExx(
    i64 %.1,
    i64 %.2) local_unnamed_addr #1 {
entry:
  %.4 = alloca i64, align 8
  %.8 = call i32 @_ZN8__main__3addB2v1B52c8tJTIeFIjxB2IKSgI4CrvQClUYkACQB1EiFSRRB9GgCAA_3d_3dExx(
      ptr nonnull %.4, ptr nonnull poison, i64 %.1, i64 %.2) #2
  %.18 = load i64, ptr %.4, align 8
  ret i64 %.18
}

define i64 @add_export(i64 %a, i64 %b) {
entry:
  %result = call i64 @cfunc._ZN8__main__3addB2v1B52c8tJTIeFIjxB2IKSgI4CrvQClUYkACQB1EiFSRRB9GgCAA_3d_3dExx(
      i64 %a, i64 %b)
  ret i64 %result
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: write) }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
attributes #2 = { noinline }
