source_filename = "isolated-sub"
target triple = "x86_64-unknown-linux-gnu"

define i32 @same_name(i32 %a, i32 %b) {
entry:
  %result = sub i32 %a, %b
  ret i32 %result
}
