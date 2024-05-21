
#include <nautilus/tracing/symbolic_execution/SymbolicExecutionPath.hpp>

namespace nautilus::tracing {
void SymbolicExecutionPath::append(bool outcome) {
	path.emplace_back(outcome);
}

std::tuple<bool> SymbolicExecutionPath::operator[](uint64_t size) {
	return path[size];
}

uint64_t SymbolicExecutionPath::getSize() {
	return path.size();
}

std::ostream& operator<<(std::ostream& os, const SymbolicExecutionPath& path) {
	os << "[";
	for (auto p : path.path) {
		os << p << ",";
	}
	os << "]";
	return os;
}

const Snapshot& SymbolicExecutionPath::getFinalTag() const {
	return this->finalTag;
}

void SymbolicExecutionPath::setFinalTag(const Snapshot& t) {
	this->finalTag = t;
};

std::vector<bool>& SymbolicExecutionPath::getPath() {
	return path;
}

} // namespace nautilus::tracing
