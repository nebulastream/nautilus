; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i32 @execute(i32 %0) local_unnamed_addr #0 {
  %2 = icmp sgt i32 %0, 0
  br i1 %2, label %3, label %151

3:                                                ; preds = %1
  %.not = icmp eq i32 %0, 1
  br i1 %.not, label %151, label %4

4:                                                ; preds = %3
  %5 = icmp samesign ugt i32 %0, 2
  br i1 %5, label %6, label %151

6:                                                ; preds = %4
  %.not1 = icmp eq i32 %0, 3
  br i1 %.not1, label %151, label %7

7:                                                ; preds = %6
  %8 = icmp samesign ugt i32 %0, 4
  br i1 %8, label %9, label %151

9:                                                ; preds = %7
  %.not2 = icmp eq i32 %0, 5
  br i1 %.not2, label %151, label %10

10:                                               ; preds = %9
  %11 = icmp samesign ugt i32 %0, 6
  br i1 %11, label %12, label %151

12:                                               ; preds = %10
  %.not3 = icmp eq i32 %0, 7
  br i1 %.not3, label %151, label %13

13:                                               ; preds = %12
  %14 = icmp samesign ugt i32 %0, 8
  br i1 %14, label %15, label %151

15:                                               ; preds = %13
  %.not4 = icmp eq i32 %0, 9
  br i1 %.not4, label %151, label %16

16:                                               ; preds = %15
  %17 = icmp samesign ugt i32 %0, 10
  br i1 %17, label %18, label %151

18:                                               ; preds = %16
  %.not5 = icmp eq i32 %0, 11
  br i1 %.not5, label %151, label %19

19:                                               ; preds = %18
  %20 = icmp samesign ugt i32 %0, 12
  br i1 %20, label %21, label %151

21:                                               ; preds = %19
  %.not6 = icmp eq i32 %0, 13
  br i1 %.not6, label %151, label %22

22:                                               ; preds = %21
  %23 = icmp samesign ugt i32 %0, 14
  br i1 %23, label %24, label %151

24:                                               ; preds = %22
  %.not7 = icmp eq i32 %0, 15
  br i1 %.not7, label %151, label %25

25:                                               ; preds = %24
  %26 = icmp samesign ugt i32 %0, 16
  br i1 %26, label %27, label %151

27:                                               ; preds = %25
  %.not8 = icmp eq i32 %0, 17
  br i1 %.not8, label %151, label %28

28:                                               ; preds = %27
  %29 = icmp samesign ugt i32 %0, 18
  br i1 %29, label %30, label %151

30:                                               ; preds = %28
  %.not9 = icmp eq i32 %0, 19
  br i1 %.not9, label %151, label %31

31:                                               ; preds = %30
  %32 = icmp samesign ugt i32 %0, 20
  br i1 %32, label %33, label %151

33:                                               ; preds = %31
  %.not10 = icmp eq i32 %0, 21
  br i1 %.not10, label %151, label %34

34:                                               ; preds = %33
  %35 = icmp samesign ugt i32 %0, 22
  br i1 %35, label %36, label %151

36:                                               ; preds = %34
  %.not11 = icmp eq i32 %0, 23
  br i1 %.not11, label %151, label %37

37:                                               ; preds = %36
  %38 = icmp samesign ugt i32 %0, 24
  br i1 %38, label %39, label %151

39:                                               ; preds = %37
  %.not12 = icmp eq i32 %0, 25
  br i1 %.not12, label %151, label %40

40:                                               ; preds = %39
  %41 = icmp samesign ugt i32 %0, 26
  br i1 %41, label %42, label %151

42:                                               ; preds = %40
  %.not13 = icmp eq i32 %0, 27
  br i1 %.not13, label %151, label %43

43:                                               ; preds = %42
  %44 = icmp samesign ugt i32 %0, 28
  br i1 %44, label %45, label %151

45:                                               ; preds = %43
  %.not14 = icmp eq i32 %0, 29
  br i1 %.not14, label %151, label %46

46:                                               ; preds = %45
  %47 = icmp samesign ugt i32 %0, 30
  br i1 %47, label %48, label %151

48:                                               ; preds = %46
  %.not15 = icmp eq i32 %0, 31
  br i1 %.not15, label %151, label %49

49:                                               ; preds = %48
  %50 = icmp samesign ugt i32 %0, 32
  br i1 %50, label %51, label %151

51:                                               ; preds = %49
  %.not16 = icmp eq i32 %0, 33
  br i1 %.not16, label %151, label %52

52:                                               ; preds = %51
  %53 = icmp samesign ugt i32 %0, 34
  br i1 %53, label %54, label %151

54:                                               ; preds = %52
  %.not17 = icmp eq i32 %0, 35
  br i1 %.not17, label %151, label %55

55:                                               ; preds = %54
  %56 = icmp samesign ugt i32 %0, 36
  br i1 %56, label %57, label %151

57:                                               ; preds = %55
  %.not18 = icmp eq i32 %0, 37
  br i1 %.not18, label %151, label %58

58:                                               ; preds = %57
  %59 = icmp samesign ugt i32 %0, 38
  br i1 %59, label %60, label %151

60:                                               ; preds = %58
  %.not19 = icmp eq i32 %0, 39
  br i1 %.not19, label %151, label %61

61:                                               ; preds = %60
  %62 = icmp samesign ugt i32 %0, 40
  br i1 %62, label %63, label %151

63:                                               ; preds = %61
  %.not20 = icmp eq i32 %0, 41
  br i1 %.not20, label %151, label %64

64:                                               ; preds = %63
  %65 = icmp samesign ugt i32 %0, 42
  br i1 %65, label %66, label %151

66:                                               ; preds = %64
  %.not21 = icmp eq i32 %0, 43
  br i1 %.not21, label %151, label %67

67:                                               ; preds = %66
  %68 = icmp samesign ugt i32 %0, 44
  br i1 %68, label %69, label %151

69:                                               ; preds = %67
  %.not22 = icmp eq i32 %0, 45
  br i1 %.not22, label %151, label %70

70:                                               ; preds = %69
  %71 = icmp samesign ugt i32 %0, 46
  br i1 %71, label %72, label %151

72:                                               ; preds = %70
  %.not23 = icmp eq i32 %0, 47
  br i1 %.not23, label %151, label %73

73:                                               ; preds = %72
  %74 = icmp samesign ugt i32 %0, 48
  br i1 %74, label %75, label %151

75:                                               ; preds = %73
  %.not24 = icmp eq i32 %0, 49
  br i1 %.not24, label %151, label %76

76:                                               ; preds = %75
  %77 = icmp samesign ugt i32 %0, 50
  br i1 %77, label %78, label %151

78:                                               ; preds = %76
  %.not25 = icmp eq i32 %0, 51
  br i1 %.not25, label %151, label %79

79:                                               ; preds = %78
  %80 = icmp samesign ugt i32 %0, 52
  br i1 %80, label %81, label %151

81:                                               ; preds = %79
  %.not26 = icmp eq i32 %0, 53
  br i1 %.not26, label %151, label %82

82:                                               ; preds = %81
  %83 = icmp samesign ugt i32 %0, 54
  br i1 %83, label %84, label %151

84:                                               ; preds = %82
  %.not27 = icmp eq i32 %0, 55
  br i1 %.not27, label %151, label %85

85:                                               ; preds = %84
  %86 = icmp samesign ugt i32 %0, 56
  br i1 %86, label %87, label %151

87:                                               ; preds = %85
  %.not28 = icmp eq i32 %0, 57
  br i1 %.not28, label %151, label %88

88:                                               ; preds = %87
  %89 = icmp samesign ugt i32 %0, 58
  br i1 %89, label %90, label %151

90:                                               ; preds = %88
  %.not29 = icmp eq i32 %0, 59
  br i1 %.not29, label %151, label %91

91:                                               ; preds = %90
  %92 = icmp samesign ugt i32 %0, 60
  br i1 %92, label %93, label %151

93:                                               ; preds = %91
  %.not30 = icmp eq i32 %0, 61
  br i1 %.not30, label %151, label %94

94:                                               ; preds = %93
  %95 = icmp samesign ugt i32 %0, 62
  br i1 %95, label %96, label %151

96:                                               ; preds = %94
  %.not31 = icmp eq i32 %0, 63
  br i1 %.not31, label %151, label %97

97:                                               ; preds = %96
  %98 = icmp samesign ugt i32 %0, 64
  br i1 %98, label %99, label %151

99:                                               ; preds = %97
  %.not32 = icmp eq i32 %0, 65
  br i1 %.not32, label %151, label %100

100:                                              ; preds = %99
  %101 = icmp samesign ugt i32 %0, 66
  br i1 %101, label %102, label %151

102:                                              ; preds = %100
  %.not33 = icmp eq i32 %0, 67
  br i1 %.not33, label %151, label %103

103:                                              ; preds = %102
  %104 = icmp samesign ugt i32 %0, 68
  br i1 %104, label %105, label %151

105:                                              ; preds = %103
  %.not34 = icmp eq i32 %0, 69
  br i1 %.not34, label %151, label %106

106:                                              ; preds = %105
  %107 = icmp samesign ugt i32 %0, 70
  br i1 %107, label %108, label %151

108:                                              ; preds = %106
  %.not35 = icmp eq i32 %0, 71
  br i1 %.not35, label %151, label %109

109:                                              ; preds = %108
  %110 = icmp samesign ugt i32 %0, 72
  br i1 %110, label %111, label %151

111:                                              ; preds = %109
  %.not36 = icmp eq i32 %0, 73
  br i1 %.not36, label %151, label %112

112:                                              ; preds = %111
  %113 = icmp samesign ugt i32 %0, 74
  br i1 %113, label %114, label %151

114:                                              ; preds = %112
  %.not37 = icmp eq i32 %0, 75
  br i1 %.not37, label %151, label %115

115:                                              ; preds = %114
  %116 = icmp samesign ugt i32 %0, 76
  br i1 %116, label %117, label %151

117:                                              ; preds = %115
  %.not38 = icmp eq i32 %0, 77
  br i1 %.not38, label %151, label %118

118:                                              ; preds = %117
  %119 = icmp samesign ugt i32 %0, 78
  br i1 %119, label %120, label %151

120:                                              ; preds = %118
  %.not39 = icmp eq i32 %0, 79
  br i1 %.not39, label %151, label %121

121:                                              ; preds = %120
  %122 = icmp samesign ugt i32 %0, 80
  br i1 %122, label %123, label %151

123:                                              ; preds = %121
  %.not40 = icmp eq i32 %0, 81
  br i1 %.not40, label %151, label %124

124:                                              ; preds = %123
  %125 = icmp samesign ugt i32 %0, 82
  br i1 %125, label %126, label %151

126:                                              ; preds = %124
  %.not41 = icmp eq i32 %0, 83
  br i1 %.not41, label %151, label %127

127:                                              ; preds = %126
  %128 = icmp samesign ugt i32 %0, 84
  br i1 %128, label %129, label %151

129:                                              ; preds = %127
  %.not42 = icmp eq i32 %0, 85
  br i1 %.not42, label %151, label %130

130:                                              ; preds = %129
  %131 = icmp samesign ugt i32 %0, 86
  br i1 %131, label %132, label %151

132:                                              ; preds = %130
  %.not43 = icmp eq i32 %0, 87
  br i1 %.not43, label %151, label %133

133:                                              ; preds = %132
  %134 = icmp samesign ugt i32 %0, 88
  br i1 %134, label %135, label %151

135:                                              ; preds = %133
  %.not44 = icmp eq i32 %0, 89
  br i1 %.not44, label %151, label %136

136:                                              ; preds = %135
  %137 = icmp samesign ugt i32 %0, 90
  br i1 %137, label %138, label %151

138:                                              ; preds = %136
  %.not45 = icmp eq i32 %0, 91
  br i1 %.not45, label %151, label %139

139:                                              ; preds = %138
  %140 = icmp samesign ugt i32 %0, 92
  br i1 %140, label %141, label %151

141:                                              ; preds = %139
  %.not46 = icmp eq i32 %0, 93
  br i1 %.not46, label %151, label %142

142:                                              ; preds = %141
  %143 = icmp samesign ugt i32 %0, 94
  br i1 %143, label %144, label %151

144:                                              ; preds = %142
  %.not47 = icmp eq i32 %0, 95
  br i1 %.not47, label %151, label %145

145:                                              ; preds = %144
  %146 = icmp samesign ugt i32 %0, 96
  br i1 %146, label %147, label %151

147:                                              ; preds = %145
  %.not48 = icmp eq i32 %0, 97
  br i1 %.not48, label %151, label %148

148:                                              ; preds = %147
  %149 = icmp samesign ugt i32 %0, 98
  br i1 %149, label %150, label %151

150:                                              ; preds = %148
  %.not49 = icmp eq i32 %0, 99
  %spec.select = select i1 %.not49, i32 99, i32 100
  br label %151

151:                                              ; preds = %150, %148, %147, %145, %144, %142, %141, %139, %138, %136, %135, %133, %132, %130, %129, %127, %126, %124, %123, %121, %120, %118, %117, %115, %114, %112, %111, %109, %108, %106, %105, %103, %102, %100, %99, %97, %96, %94, %93, %91, %90, %88, %87, %85, %84, %82, %81, %79, %78, %76, %75, %73, %72, %70, %69, %67, %66, %64, %63, %61, %60, %58, %57, %55, %54, %52, %51, %49, %48, %46, %45, %43, %42, %40, %39, %37, %36, %34, %33, %31, %30, %28, %27, %25, %24, %22, %21, %19, %18, %16, %15, %13, %12, %10, %9, %7, %6, %4, %3, %1
  %152 = phi i32 [ 98, %148 ], [ 97, %147 ], [ 96, %145 ], [ 95, %144 ], [ 94, %142 ], [ 93, %141 ], [ 92, %139 ], [ 91, %138 ], [ 90, %136 ], [ 89, %135 ], [ 88, %133 ], [ 87, %132 ], [ 86, %130 ], [ 85, %129 ], [ 84, %127 ], [ 83, %126 ], [ 82, %124 ], [ 81, %123 ], [ 80, %121 ], [ 79, %120 ], [ 78, %118 ], [ 77, %117 ], [ 76, %115 ], [ 75, %114 ], [ 74, %112 ], [ 73, %111 ], [ 72, %109 ], [ 71, %108 ], [ 70, %106 ], [ 69, %105 ], [ 68, %103 ], [ 67, %102 ], [ 66, %100 ], [ 65, %99 ], [ 64, %97 ], [ 63, %96 ], [ 62, %94 ], [ 61, %93 ], [ 60, %91 ], [ 59, %90 ], [ 58, %88 ], [ 57, %87 ], [ 56, %85 ], [ 55, %84 ], [ 54, %82 ], [ 53, %81 ], [ 52, %79 ], [ 51, %78 ], [ 50, %76 ], [ 49, %75 ], [ 48, %73 ], [ 47, %72 ], [ 46, %70 ], [ 45, %69 ], [ 44, %67 ], [ 43, %66 ], [ 42, %64 ], [ 41, %63 ], [ 40, %61 ], [ 39, %60 ], [ 38, %58 ], [ 37, %57 ], [ 36, %55 ], [ 35, %54 ], [ 34, %52 ], [ 33, %51 ], [ 32, %49 ], [ 31, %48 ], [ 30, %46 ], [ 29, %45 ], [ 28, %43 ], [ 27, %42 ], [ 26, %40 ], [ 25, %39 ], [ 24, %37 ], [ 23, %36 ], [ 22, %34 ], [ 21, %33 ], [ 20, %31 ], [ 19, %30 ], [ 18, %28 ], [ 17, %27 ], [ 16, %25 ], [ 15, %24 ], [ 14, %22 ], [ 13, %21 ], [ 12, %19 ], [ 11, %18 ], [ 10, %16 ], [ 9, %15 ], [ 8, %13 ], [ 7, %12 ], [ 6, %10 ], [ 5, %9 ], [ 4, %7 ], [ 3, %6 ], [ 2, %4 ], [ 1, %3 ], [ 0, %1 ], [ %spec.select, %150 ]
  ret i32 %152
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(none)
define signext i32 @_mlir_ciface_execute(i32 %0) local_unnamed_addr #1 {
  %2 = tail call i32 @execute(i32 %0)
  ret i32 %2
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = tail call i32 @execute(i32 %3)
  %5 = getelementptr i8, ptr %0, i64 8
  %6 = load ptr, ptr %5, align 8
  store i32 %4, ptr %6, align 4
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none)
define void @_mlir__mlir_ciface_execute(ptr readonly %0) local_unnamed_addr #2 {
  %2 = load ptr, ptr %0, align 8
  %3 = load i32, ptr %2, align 4
  %4 = tail call i32 @execute(i32 %3)
  %5 = getelementptr i8, ptr %0, i64 8
  %6 = load ptr, ptr %5, align 8
  store i32 %4, ptr %6, align 4
  ret void
}

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
attributes #1 = { mustprogress nofree norecurse nosync nounwind willreturn memory(none) }
attributes #2 = { mustprogress nofree norecurse nosync nounwind willreturn memory(readwrite, inaccessiblemem: none) }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
