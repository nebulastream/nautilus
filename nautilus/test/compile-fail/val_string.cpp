// EXPECT-ERROR: val<T> requires an arithmetic, bool, enum or pointer type
#include <nautilus/val.hpp>
#include <string>

void f() {
	nautilus::val<std::string> s;
	(void) s;
}
