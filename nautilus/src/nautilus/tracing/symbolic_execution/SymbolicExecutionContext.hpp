
#include "SymbolicExecutionPath.hpp"
#include "nautilus/tracing/ExecutionTrace.hpp"
#include "nautilus/tracing/tag/Tag.hpp"
#include <list>
#include <unordered_map>

namespace nautilus::tracing {
class TagRecorder;

class TraceContext;

/**
 * @brief The symbolic execution context supports the symbolic execution of functions.
 * In general it executes a function with dummy parameters and explores all possible execution paths.
 */
class SymbolicExecutionContext {
public:
	// The number of iterations we want to spend maximally to explore executions.
	static const uint64_t MAX_ITERATIONS = 100000;

	enum class MODE : const uint8_t { FOLLOW, RECORD };

	/**
	 * @brief Performs a symbolic execution of a CMP operation.
	 * Depending on all previous executions this function determines if a branch should be explored or not.
	 * @return the return value of this branch
	 */
	bool executeCMP(TagRecorder& tr);

	/**
	 * @brief Check if we should continue the symbolic execution or if we evaluated all possible execution passes.
	 * @return false if all execution passes trough a function have been evaluated.
	 */
	bool shouldContinue();

	/**
	 * @brief Initializes the next iteration of the symbolic execution.
	 */
	void next();

	/**
	 * @brief Returns the number of iterations of symbolic execution.
	 * @return uint64_t
	 */
	uint64_t getIterations() const;

	MODE getCurrentMode() const;

	/**
	 * @brief Records a new cmp operation
	 * @param tr TagRecorder
	 * @return
	 */
	bool record(const Snapshot& tag);

	bool shouldFollow();

	bool follow();

private:
	friend TraceContext;
	/**
	 * @brief Symbolic execution mode.
	 * That identifies if, we follow a previously recorded execution or if we record a new one.
	 */

	/**
	 * @brief Tag state
	 * This indicates if we visited a specific tag one or two times.
	 * If we already visited it two times, we can skip any further executions at this point.
	 */
	enum class TagState : const int8_t { FirstVisit, SecondVisit };
	std::unordered_map<Snapshot, TagState> tagMap;
	std::list<SymbolicExecutionPath> inflightExecutionPaths;
	MODE currentMode = MODE::RECORD;
	SymbolicExecutionPath currentExecutionPath = SymbolicExecutionPath();
	uint64_t currentOperation = 0;
	uint64_t iterations = 0;
};

} // namespace nautilus::tracing
