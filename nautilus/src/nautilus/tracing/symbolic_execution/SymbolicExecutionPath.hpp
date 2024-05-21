
#include "nautilus/tracing/ExecutionTrace.hpp"
#include "nautilus/tracing/tag/Tag.hpp"
#include <ostream>

namespace nautilus::tracing {

/**
 * @brief The SymbolicExecutionPath represents all tags in one specific execution.
 */
class SymbolicExecutionPath {
public:
	void append(bool outcome);

	std::tuple<bool> operator[](uint64_t size);

	uint64_t getSize();

	friend std::ostream& operator<<(std::ostream& os, const SymbolicExecutionPath& path);

	[[maybe_unused]] std::vector<bool>& getPath();

	[[nodiscard]] const Snapshot& getFinalTag() const;

	void setFinalTag(const Snapshot& finalTag);

private:
	std::vector<bool> path;
	Snapshot finalTag;
};

} // namespace nautilus::tracing
