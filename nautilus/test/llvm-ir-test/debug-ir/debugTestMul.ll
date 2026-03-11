; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

define signext i32 @execute(i32 %0, i32 %1) !dbg !3 {
  %3 = mul i32 %0, %1, !dbg !6
  ret i32 %3, !dbg !6
}

define signext i32 @_mlir_ciface_execute(i32 %0, i32 %1) {
  %3 = call i32 @execute(i32 %0, i32 %1)
  ret i32 %3
}

define void @_mlir_execute(ptr %0) {
  %2 = getelementptr ptr, ptr %0, i64 0
  %3 = load ptr, ptr %2, align 8
  %4 = load i32, ptr %3, align 4
  %5 = getelementptr ptr, ptr %0, i64 1
  %6 = load ptr, ptr %5, align 8
  %7 = load i32, ptr %6, align 4
  %8 = call i32 @execute(i32 %4, i32 %7)
  %9 = getelementptr ptr, ptr %0, i64 2
  %10 = load ptr, ptr %9, align 8
  store i32 %8, ptr %10, align 4
  ret void
}

define void @_mlir__mlir_ciface_execute(ptr %0) {
  %2 = getelementptr ptr, ptr %0, i64 0
  %3 = load ptr, ptr %2, align 8
  %4 = load i32, ptr %3, align 4
  %5 = getelementptr ptr, ptr %0, i64 1
  %6 = load ptr, ptr %5, align 8
  %7 = load i32, ptr %6, align 4
  %8 = call i32 @_mlir_ciface_execute(i32 %4, i32 %7)
  %9 = getelementptr ptr, ptr %0, i64 2
  %10 = load ptr, ptr %9, align 8
  store i32 %8, ptr %10, align 4
  ret void
}


!llvm.module.flags = !{!0}
!llvm.dbg.cu = !{!1}

!0 = !{i32 2, !"Debug Info Version", i32 3}
!1 = distinct !DICompileUnit(language: DW_LANG_C_plus_plus, file: !2, producer: "nautilus", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug)
!2 = !DIFile(filename: "nautilus_jit.cpp", directory: ".")
!3 = distinct !DISubprogram(name: "execute", linkageName: "execute", scope: null, file: !2, line: 1, type: !4, scopeLine: 1, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !1)
!4 = !DISubroutineType(types: !5)
!5 = !{}
!6 = !DILocation(line: 1, scope: !3)
!7 = !DILocalVariable(name: "v0", scope: !3, file: !2, line: 1, type: !8)
!8 = !DIBasicType(name: "i32", size: 32, encoding: DW_ATE_signed)
