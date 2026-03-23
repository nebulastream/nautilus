; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:o-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "arm64-apple-darwin25.2.0"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: read)
define signext i32 @execute(ptr readonly %0, ptr readonly %1) local_unnamed_addr #0 {
  %3 = load <4 x i32>, ptr %0, align 16
  %4 = load <4 x i32>, ptr %1, align 16
  %5 = mul <4 x i32> %4, %3
  %6 = tail call i32 @llvm.vector.reduce.add.v4i32(<4 x i32> %5)
  ret i32 %6
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: read)
define signext i32 @_mlir_ciface_execute(ptr readonly %0, ptr readonly %1) local_unnamed_addr #0 {
  %3 = load <4 x i32>, ptr %0, align 16
  %4 = load <4 x i32>, ptr %1, align 16
  %5 = mul <4 x i32> %4, %3
  %6 = tail call i32 @llvm.vector.reduce.add.v4i32(<4 x i32> %5)
  ret i32 %6
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i32 @llvm.vector.reduce.add.v4i32(<4 x i32>) #1

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load ptr, ptr %5, align 8
  %7 = load <4 x i32>, ptr %3, align 16
  %8 = load <4 x i32>, ptr %6, align 16
  %9 = mul <4 x i32> %8, %7
  %10 = tail call i32 @llvm.vector.reduce.add.v4i32(<4 x i32> %9)
  %11 = getelementptr i8, ptr %0, i64 16
  %12 = load ptr, ptr %11, align 8
  store i32 %10, ptr %12, align 4
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = getelementptr i8, ptr %0, i64 8
  %5 = load ptr, ptr %4, align 8
  %6 = load ptr, ptr %5, align 8
  %7 = load <4 x i32>, ptr %3, align 16
  %8 = load <4 x i32>, ptr %6, align 16
  %9 = mul <4 x i32> %8, %7
  %10 = tail call i32 @llvm.vector.reduce.add.v4i32(<4 x i32> %9)
  %11 = getelementptr i8, ptr %0, i64 16
  %12 = load ptr, ptr %11, align 8
  store i32 %10, ptr %12, align 4
  ret void
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(argmem: read) "target-features" }
attributes #1 = { mustprogress nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #2 = { mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none) "target-features" }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
