source_filename = "isolated-add"
target triple = "x86_64-unknown-linux-gnu"

define i32 @same_name(i32 %a, i32 %b) {
entry:
  %result = add i32 %a, %b
  ret i32 %result
}
