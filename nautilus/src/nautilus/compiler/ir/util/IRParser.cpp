#include "nautilus/compiler/ir/util/IRParser.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockArgument.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockInvocation.hpp"
#include "nautilus/compiler/ir/operations/AllocaOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/AddOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/DivOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/ModOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/MulOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/SubOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/BinaryCompOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/NegateOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/ShiftOperation.hpp"
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/CastOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstBooleanOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstFloatOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstIntOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstPtrOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionAddressOfOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/IfOperation.hpp"
#include "nautilus/compiler/ir/operations/IndirectCallOperation.hpp"
#include "nautilus/compiler/ir/operations/LoadOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/AndOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/CompareOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/NotOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/OrOperation.hpp"
#include "nautilus/compiler/ir/operations/ProxyCallOperation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include "nautilus/compiler/ir/operations/SelectOperation.hpp"
#include "nautilus/compiler/ir/operations/StoreOperation.hpp"
#include "nautilus/compiler/ir/passes/IRVerifier.hpp"
#include "nautilus/compiler/ir/util/ControlFlowUtil.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <charconv>
#include <cstdlib>
#include <dlfcn.h>
#include <fmt/format.h>
#include <limits>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace nautilus::compiler::ir {

namespace {

/// Cursor over a single line of serialized IR with position tracking for
/// error messages. All parse helpers throw RuntimeException on mismatch.
struct LineCursor {
	std::string_view text;
	size_t lineNumber;
	size_t pos = 0;

	[[noreturn]] void fail(const std::string& message) const {
		throw RuntimeException(
		    fmt::format("IR parse error in line {} at column {}: {} (line: '{}')", lineNumber, pos + 1, message, text));
	}

	void skipWs() {
		while (pos < text.size() && (text[pos] == ' ' || text[pos] == '\t')) {
			pos++;
		}
	}

	bool atEnd() const {
		return pos >= text.size();
	}

	char peek(size_t offset = 0) const {
		return pos + offset < text.size() ? text[pos + offset] : '\0';
	}

	bool tryConsume(std::string_view token) {
		if (text.substr(pos, token.size()) == token) {
			pos += token.size();
			return true;
		}
		return false;
	}

	void expect(std::string_view token) {
		if (!tryConsume(token)) {
			fail(fmt::format("expected '{}'", token));
		}
	}

	void expectEnd() {
		skipWs();
		if (!atEnd()) {
			fail("unexpected trailing characters");
		}
	}

	template <typename T>
	T parseNumber() {
		// Integral only: libc++ does not implement std::from_chars for
		// floating-point types (see parseDouble).
		static_assert(std::is_integral_v<T>);
		T value {};
		auto [ptr, ec] = std::from_chars(text.data() + pos, text.data() + text.size(), value);
		if (ec != std::errc()) {
			fail("expected a number");
		}
		pos = static_cast<size_t>(ptr - text.data());
		return value;
	}

	/// Parses a floating-point number via strtod on a bounded copy, since
	/// std::from_chars for doubles is unavailable in libc++.
	double parseDouble() {
		const size_t start = pos;
		while (pos < text.size() && text[pos] != ')' && text[pos] != ' ' && text[pos] != '\t') {
			pos++;
		}
		const std::string token(text.substr(start, pos - start));
		char* end = nullptr;
		const double value = std::strtod(token.c_str(), &end);
		if (token.empty() || end != token.c_str() + token.size()) {
			fail("expected a floating-point number");
		}
		return value;
	}

	/// Parses a `$<id>` value reference and returns the raw id.
	uint32_t parseValueRef() {
		expect("$");
		return parseNumber<uint32_t>();
	}

	/// Reads characters until whitespace or one of @p extraDelimiters.
	std::string_view parseWord(std::string_view extraDelimiters = {}) {
		const size_t start = pos;
		while (pos < text.size() && text[pos] != ' ' && text[pos] != '\t' &&
		       extraDelimiters.find(text[pos]) == std::string_view::npos) {
			pos++;
		}
		if (pos == start) {
			fail("expected a token");
		}
		return text.substr(start, pos - start);
	}

	/// Parses a `"..."` string with backslash escapes (see IRSerializationUtil).
	std::string parseQuoted() {
		expect("\"");
		std::string out;
		while (true) {
			if (atEnd()) {
				fail("unterminated string literal");
			}
			const char c = text[pos++];
			if (c == '"') {
				return out;
			}
			if (c == '\\') {
				if (atEnd()) {
					fail("unterminated escape sequence");
				}
				out.push_back(text[pos++]);
			} else {
				out.push_back(c);
			}
		}
	}

	Type parseType() {
		static const std::unordered_map<std::string_view, Type> types = {
		    {"void", Type::v},  {"bool", Type::b},  {"i8", Type::i8},     {"i16", Type::i16},   {"i32", Type::i32},
		    {"i64", Type::i64}, {"ui8", Type::ui8}, {"ui16", Type::ui16}, {"ui32", Type::ui32}, {"ui64", Type::ui64},
		    {"f32", Type::f32}, {"f64", Type::f64}, {"ptr", Type::ptr}};
		const auto word = parseWord(",():?");
		if (auto it = types.find(word); it != types.end()) {
			return it->second;
		}
		fail(fmt::format("unknown type '{}'", word));
	}

	/// Parses the ` :<type>` trailer every operation line ends with.
	Type parseTrailingStamp() {
		skipWs();
		expect(":");
		const auto type = parseType();
		expectEnd();
		return type;
	}
};

struct BlockSection {
	BasicBlock* block = nullptr;
	std::vector<std::pair<uint32_t, BasicBlockArgument*>> arguments;
	std::vector<LineCursor> operationLines;
};

class IRTextParser {
public:
	IRTextParser(std::string_view text, std::shared_ptr<IRGraph> graph, const IRSymbolResolverFn& symbolResolver)
	    : graph(std::move(graph)), symbolResolver(symbolResolver) {
		size_t lineNumber = 1;
		size_t start = 0;
		while (start <= text.size()) {
			size_t end = text.find('\n', start);
			if (end == std::string_view::npos) {
				end = text.size();
			}
			auto line = text.substr(start, end - start);
			// Trim trailing carriage returns and whitespace-only lines.
			while (!line.empty() && (line.back() == '\r' || line.back() == ' ' || line.back() == '\t')) {
				line.remove_suffix(1);
			}
			size_t indent = 0;
			while (indent < line.size() && (line[indent] == ' ' || line[indent] == '\t')) {
				indent++;
			}
			if (indent < line.size()) {
				lines.push_back(LineCursor {line, lineNumber, indent});
			}
			lineNumber++;
			if (end == text.size()) {
				break;
			}
			start = end + 1;
		}
	}

	std::shared_ptr<IRGraph> parse() {
		if (lines.empty()) {
			throw RuntimeException("IR parse error: input is empty");
		}
		size_t index = 0;
		lines[index].expect("nautilus {");
		lines[index].expectEnd();
		index++;
		bool closed = false;
		while (index < lines.size()) {
			if (lines[index].tryConsume("} //nautilus")) {
				lines[index].expectEnd();
				closed = true;
				index++;
				break;
			}
			index = parseFunction(index);
		}
		if (!closed) {
			throw RuntimeException("IR parse error: missing '} //nautilus' terminator");
		}
		if (index < lines.size()) {
			lines[index].fail("unexpected content after '} //nautilus'");
		}
		if (graph->getFunctionOperations().empty()) {
			throw RuntimeException("IR parse error: input contains no function");
		}
		rebuildPredecessorLists(*graph);
		if (const auto result = IRVerifier::verify(*graph); !result.ok()) {
			throw RuntimeException(fmt::format("Parsed IR failed verification:\n{}", result.toString()));
		}
		return graph;
	}

private:
	std::shared_ptr<IRGraph> graph;
	const IRSymbolResolverFn& symbolResolver;
	std::vector<LineCursor> lines;

	// Per-function parse state, reset by parseFunction.
	std::unordered_map<uint32_t, BasicBlock*> blocksById;
	// Operations (and entry-block arguments) visible for cross-block
	// references, by value id. An id defined in more than one block is
	// mapped to nullptr: it stays resolvable through the local block scope
	// but is rejected as a cross-block reference because it is ambiguous.
	std::unordered_map<uint32_t, Operation*> functionScope;
	// Block-local definitions: the current block's arguments plus the
	// operations parsed so far in it. Values that cross block boundaries
	// flow through block arguments (phi-style), so lookups prefer this map.
	std::unordered_map<uint32_t, Operation*> blockScope;
	// Fresh ids for operations that are printed without one (void calls).
	// Counts down from the top of the id space so it cannot collide with
	// the ids the producing trace assigned.
	uint32_t nextAnonymousId = 0;

	Operation* resolveValue(uint32_t id, LineCursor& cursor) {
		if (auto it = blockScope.find(id); it != blockScope.end()) {
			return it->second;
		}
		if (auto it = functionScope.find(id); it != functionScope.end()) {
			if (it->second == nullptr) {
				cursor.fail(fmt::format("cross-block reference to ${} is ambiguous (defined in multiple blocks)", id));
			}
			return it->second;
		}
		cursor.fail(fmt::format("use of undefined value ${}", id));
	}

	void defineValue(uint32_t id, Operation* operation, LineCursor& cursor) {
		if (!blockScope.emplace(id, operation).second) {
			cursor.fail(fmt::format("duplicate definition of ${} in block", id));
		}
		if (auto [it, inserted] = functionScope.emplace(id, operation); !inserted) {
			it->second = nullptr;
		}
	}

	void* resolveFunctionAddress(const std::string& symbol, const std::string& name, LineCursor& cursor) {
		if (symbolResolver) {
			if (void* address = symbolResolver(symbol, name)) {
				return address;
			}
		}
		if (!symbol.empty()) {
			if (void* address = dlsym(RTLD_DEFAULT, symbol.c_str())) {
				return address;
			}
		}
		cursor.fail(fmt::format("cannot resolve external function '{}' (symbol '{}'); provide a symbol resolver that "
		                        "maps it to an address in this process",
		                        name, symbol));
	}

	FunctionAttributes parseCallAttributes(LineCursor& cursor) {
		FunctionAttributes attributes;
		cursor.skipWs();
		if (!cursor.tryConsume("attrs[")) {
			return attributes;
		}
		while (!cursor.tryConsume("]")) {
			cursor.skipWs();
			if (cursor.tryConsume("modref=")) {
				const auto value = cursor.parseNumber<uint32_t>();
				if (value > static_cast<uint32_t>(ModRefInfo::ModRef)) {
					cursor.fail("modref value out of range");
				}
				attributes.modRefInfo = static_cast<ModRefInfo>(value);
			} else if (cursor.tryConsume("willreturn")) {
				attributes.willReturn = true;
			} else if (cursor.tryConsume("nounwind")) {
				attributes.noUnwind = true;
			} else {
				cursor.fail("unknown call attribute");
			}
			cursor.skipWs();
			cursor.tryConsume(",");
		}
		return attributes;
	}

	/// Parses `($a, $b, ...)` and resolves each reference.
	std::vector<Operation*> parseArgumentList(LineCursor& cursor) {
		std::vector<Operation*> arguments;
		cursor.expect("(");
		cursor.skipWs();
		while (!cursor.tryConsume(")")) {
			arguments.push_back(resolveValue(cursor.parseValueRef(), cursor));
			cursor.skipWs();
			cursor.tryConsume(",");
			cursor.skipWs();
		}
		return arguments;
	}

	struct ParsedInvocation {
		BasicBlock* target = nullptr;
		std::vector<Operation*> arguments;
	};

	/// Parses `Block_<id>($a, $b)` and resolves the target block and arguments.
	ParsedInvocation parseInvocation(LineCursor& cursor) {
		cursor.expect("Block_");
		const auto blockId = cursor.parseNumber<uint32_t>();
		auto it = blocksById.find(blockId);
		if (it == blocksById.end()) {
			cursor.fail(fmt::format("branch to unknown block Block_{}", blockId));
		}
		return ParsedInvocation {it->second, parseArgumentList(cursor)};
	}

	void wireInvocation(BasicBlockInvocation& invocation, const ParsedInvocation& parsed) {
		invocation.setBlock(parsed.target);
		for (auto* argument : parsed.arguments) {
			invocation.addArgument(graph->getArena(), argument);
		}
	}

	size_t parseFunction(size_t index) {
		blocksById.clear();
		functionScope.clear();
		blockScope.clear();
		nextAnonymousId = std::numeric_limits<uint32_t>::max();

		auto& header = lines[index];
		const auto name = std::string(header.parseWord("("));
		std::vector<std::pair<uint32_t, Type>> parameters;
		header.expect("(");
		header.skipWs();
		while (!header.tryConsume(")")) {
			const auto id = header.parseValueRef();
			header.expect(":");
			parameters.emplace_back(id, header.parseType());
			header.skipWs();
			header.tryConsume(",");
			header.skipWs();
		}
		header.skipWs();
		header.expect(":");
		const auto returnType = header.parseType();
		header.skipWs();
		std::vector<AllocaSpec> allocaSpecs;
		if (header.tryConsume("allocas[")) {
			while (!header.tryConsume("]")) {
				header.expect("(");
				const auto size = header.parseNumber<size_t>();
				header.expect(",");
				const auto align = header.parseNumber<size_t>();
				header.expect(")");
				allocaSpecs.push_back(AllocaSpec {size, align});
				header.tryConsume(",");
			}
			header.skipWs();
		}
		std::unordered_map<std::string, std::string> attributes;
		if (header.tryConsume("attrs[")) {
			while (!header.tryConsume("]")) {
				auto key = header.parseQuoted();
				header.expect("=");
				attributes.emplace(std::move(key), header.parseQuoted());
				header.tryConsume(",");
			}
			header.skipWs();
		}
		header.expect("{");
		header.expectEnd();
		index++;

		// First pass: split the function body into block sections and create
		// every block up front, so terminators can reference blocks that
		// appear later in the file (loops branch backwards too).
		auto& arena = graph->getArena();
		std::vector<BlockSection> sections;
		bool functionClosed = false;
		while (index < lines.size()) {
			auto cursor = lines[index];
			if (cursor.tryConsume("}")) {
				cursor.expectEnd();
				functionClosed = true;
				index++;
				break;
			}
			if (cursor.tryConsume("Block_")) {
				BlockSection section;
				const auto blockId = cursor.parseNumber<uint32_t>();
				cursor.expect("(");
				std::vector<BasicBlockArgument*> blockArguments;
				cursor.skipWs();
				while (!cursor.tryConsume(")")) {
					const auto argumentId = cursor.parseValueRef();
					cursor.expect(":");
					const auto argumentType = cursor.parseType();
					auto* argument = arena.create<BasicBlockArgument>(OperationIdentifier {argumentId}, argumentType);
					blockArguments.push_back(argument);
					section.arguments.emplace_back(argumentId, argument);
					cursor.skipWs();
					cursor.tryConsume(",");
					cursor.skipWs();
				}
				cursor.expect(":");
				cursor.expectEnd();
				section.block = arena.create<BasicBlock>(arena, BlockIdentifier {blockId}, std::move(blockArguments));
				if (!blocksById.emplace(blockId, section.block).second) {
					cursor.fail(fmt::format("duplicate block Block_{}", blockId));
				}
				sections.push_back(std::move(section));
			} else {
				if (sections.empty()) {
					cursor.fail("expected a 'Block_<id>(...):' header");
				}
				sections.back().operationLines.push_back(cursor);
			}
			index++;
		}
		if (!functionClosed) {
			header.fail(fmt::format("function '{}' is missing its closing '}}'", name));
		}
		if (sections.empty()) {
			header.fail(fmt::format("function '{}' has no blocks", name));
		}

		// The function's parameters are the entry block's arguments; check
		// that the header and the entry block agree.
		const auto& entryArguments = sections.front().arguments;
		if (parameters.size() != entryArguments.size()) {
			header.fail(fmt::format("function '{}' declares {} parameters but its entry block has {} arguments", name,
			                        parameters.size(), entryArguments.size()));
		}
		for (size_t i = 0; i < parameters.size(); ++i) {
			if (parameters[i].first != entryArguments[i].first ||
			    parameters[i].second != entryArguments[i].second->getStamp()) {
				header.fail(
				    fmt::format("function '{}' parameter {} does not match its entry block argument", name, i + 1));
			}
		}
		// Make every block's arguments visible for cross-block references —
		// passes such as block-argument pruning leave operations referencing
		// arguments of dominating blocks directly. An id used by arguments
		// of more than one block is marked ambiguous (nullptr): it stays
		// resolvable block-locally but is rejected as a cross-block
		// reference. The serializer numbers values uniquely, so this only
		// triggers for hand-written or legacy pretty-printed input.
		for (const auto& section : sections) {
			for (const auto& [argumentId, argument] : section.arguments) {
				if (auto [it, inserted] = functionScope.emplace(argumentId, argument); !inserted) {
					it->second = nullptr;
				}
			}
		}

		// Second pass: parse the operations of each block.
		std::vector<BasicBlock*> blocks;
		blocks.reserve(sections.size());
		for (auto& section : sections) {
			blockScope.clear();
			for (const auto& [argumentId, argument] : section.arguments) {
				if (!blockScope.emplace(argumentId, argument).second) {
					header.fail(fmt::format("duplicate block argument ${}", argumentId));
				}
			}
			for (auto& cursor : section.operationLines) {
				parseOperation(cursor, section.block);
			}
			blocks.push_back(section.block);
		}

		auto* function =
		    arena.create<FunctionOperation>(name, std::move(blocks), std::vector<Type> {}, std::vector<std::string> {},
		                                    returnType, std::move(allocaSpecs), std::move(attributes));
		graph->addFunctionOperation(function);
		return index;
	}

	void parseOperation(LineCursor& cursor, BasicBlock* block) {
		auto& arena = graph->getArena();
		// Terminators and result-less operations.
		if (cursor.tryConsume("br ")) {
			const auto next = parseInvocation(cursor);
			if (cursor.parseTrailingStamp() != Type::v) {
				cursor.fail("'br' must have type void");
			}
			block->addNextBlock(next.target, next.arguments);
			return;
		}
		if (cursor.tryConsume("if ")) {
			auto* condition = resolveValue(cursor.parseValueRef(), cursor);
			cursor.skipWs();
			cursor.expect("?");
			cursor.skipWs();
			const auto trueCase = parseInvocation(cursor);
			cursor.skipWs();
			cursor.expect(":");
			cursor.skipWs();
			const auto falseCase = parseInvocation(cursor);
			cursor.skipWs();
			double probability = 0.5;
			if (cursor.tryConsume("prob(")) {
				probability = cursor.parseDouble();
				cursor.expect(")");
			}
			if (cursor.parseTrailingStamp() != Type::v) {
				cursor.fail("'if' must have type void");
			}
			auto* ifOperation = arena.create<IfOperation>(arena, condition, probability);
			wireInvocation(ifOperation->getTrueBlockInvocation(), trueCase);
			wireInvocation(ifOperation->getFalseBlockInvocation(), falseCase);
			block->addOperation(ifOperation);
			return;
		}
		if (cursor.tryConsume("return")) {
			cursor.skipWs();
			Operation* returnValue = nullptr;
			if (cursor.tryConsume("(")) {
				returnValue = resolveValue(cursor.parseValueRef(), cursor);
				cursor.expect(")");
			}
			const auto stamp = cursor.parseTrailingStamp();
			if (returnValue == nullptr) {
				if (stamp != Type::v) {
					cursor.fail("'return' without a value must have type void");
				}
				block->addOperation<ReturnOperation>();
			} else {
				if (stamp != returnValue->getStamp()) {
					cursor.fail("return type does not match the returned value");
				}
				block->addOperation<ReturnOperation>(returnValue);
			}
			return;
		}
		if (cursor.tryConsume("store(")) {
			auto* value = resolveValue(cursor.parseValueRef(), cursor);
			cursor.expect(",");
			cursor.skipWs();
			auto* address = resolveValue(cursor.parseValueRef(), cursor);
			cursor.expect(")");
			if (cursor.parseTrailingStamp() != Type::v) {
				cursor.fail("'store' must have type void");
			}
			block->addOperation<StoreOperation>(value, address);
			return;
		}
		if (cursor.tryConsume("call @")) {
			parseProxyCall(cursor, block, nextAnonymousId--, /*hasResult*/ false);
			return;
		}
		if (cursor.tryConsume("call_indirect ")) {
			parseIndirectCall(cursor, block, nextAnonymousId--, /*hasResult*/ false);
			return;
		}

		// Value-producing operations: `$<id> = <rhs> :<type>`.
		const auto resultId = cursor.parseValueRef();
		cursor.skipWs();
		cursor.expect("=");
		cursor.skipWs();
		if (cursor.tryConsume("call @")) {
			parseProxyCall(cursor, block, resultId, /*hasResult*/ true);
			return;
		}
		if (cursor.tryConsume("call_indirect ")) {
			parseIndirectCall(cursor, block, resultId, /*hasResult*/ true);
			return;
		}
		Operation* operation = nullptr;
		if (cursor.tryConsume("addressof @")) {
			auto symbol = cursor.parseQuoted();
			cursor.skipWs();
			auto functionName = cursor.parseQuoted();
			void* address = resolveFunctionAddress(symbol, functionName, cursor);
			operation = block->addOperation<FunctionAddressOfOperation>(symbol, functionName, address,
			                                                            OperationIdentifier {resultId});
		} else if (cursor.tryConsume("select ")) {
			auto* condition = resolveValue(cursor.parseValueRef(), cursor);
			cursor.skipWs();
			cursor.expect("?");
			cursor.skipWs();
			auto* trueValue = resolveValue(cursor.parseValueRef(), cursor);
			cursor.skipWs();
			cursor.expect(":");
			cursor.skipWs();
			auto* falseValue = resolveValue(cursor.parseValueRef(), cursor);
			const auto stamp = cursor.parseTrailingStamp();
			operation = block->addOperation<SelectOperation>(OperationIdentifier {resultId}, condition, trueValue,
			                                                 falseValue, stamp);
			defineValue(resultId, operation, cursor);
			return;
		} else if (cursor.tryConsume("load(")) {
			auto* address = resolveValue(cursor.parseValueRef(), cursor);
			cursor.expect(")");
			const auto stamp = cursor.parseTrailingStamp();
			operation = block->addOperation<LoadOperation>(OperationIdentifier {resultId}, address, stamp);
			defineValue(resultId, operation, cursor);
			return;
		} else if (cursor.tryConsume("alloca[")) {
			const auto allocaIndex = cursor.parseNumber<uint32_t>();
			cursor.expect("]");
			operation = block->addOperation<AllocaOperation>(OperationIdentifier {resultId}, allocaIndex);
		} else if (cursor.tryConsume("null")) {
			operation = block->addOperation<ConstPtrOperation>(OperationIdentifier {resultId}, nullptr);
		} else if (cursor.peek() == '!') {
			cursor.expect("!");
			operation = block->addOperation<NotOperation>(OperationIdentifier {resultId},
			                                              resolveValue(cursor.parseValueRef(), cursor));
		} else if (cursor.peek() == '~' || (cursor.peek() == '-' && cursor.peek(1) == '$')) {
			cursor.pos++;
			operation = block->addOperation<NegateOperation>(OperationIdentifier {resultId},
			                                                 resolveValue(cursor.parseValueRef(), cursor));
		} else if (cursor.peek() == '$') {
			operation = parseBinaryOrCast(cursor, block, resultId);
			if (operation == nullptr) {
				return; // cast handled its own stamp and registration
			}
		} else {
			// Constant literal; the concrete type comes from the stamp.
			const auto literal = std::string(cursor.parseWord());
			const auto stamp = cursor.parseTrailingStamp();
			operation = createConstant(literal, stamp, resultId, block, cursor);
			defineValue(resultId, operation, cursor);
			return;
		}
		finishOperation(operation, cursor.parseTrailingStamp(), cursor, resultId);
	}

	/// Validates the declared stamp against the constructed operation and
	/// registers the result value.
	void finishOperation(Operation* operation, Type declaredStamp, LineCursor& cursor, uint32_t resultId) {
		if (operation->getStamp() != declaredStamp) {
			cursor.fail(fmt::format("declared type '{}' does not match the operation's type '{}'",
			                        toString(declaredStamp), toString(operation->getStamp())));
		}
		defineValue(resultId, operation, cursor);
	}

	Operation* createConstant(const std::string& literal, Type stamp, uint32_t resultId, BasicBlock* block,
	                          LineCursor& cursor) {
		const OperationIdentifier identifier {resultId};
		if (stamp == Type::b) {
			if (literal == "true" || literal == "false") {
				return block->addOperation<ConstBooleanOperation>(identifier, literal == "true");
			}
			cursor.fail(fmt::format("invalid boolean literal '{}'", literal));
		}
		if (isInteger(stamp)) {
			errno = 0;
			char* end = nullptr;
			const int64_t value = std::strtoll(literal.c_str(), &end, 10);
			if (errno != 0 || end != literal.c_str() + literal.size()) {
				cursor.fail(fmt::format("invalid integer literal '{}'", literal));
			}
			return block->addOperation<ConstIntOperation>(identifier, value, stamp);
		}
		if (isFloat(stamp)) {
			errno = 0;
			char* end = nullptr;
			const double value = std::strtod(literal.c_str(), &end);
			if (end != literal.c_str() + literal.size()) {
				cursor.fail(fmt::format("invalid float literal '{}'", literal));
			}
			return block->addOperation<ConstFloatOperation>(identifier, value, stamp);
		}
		if (stamp == Type::ptr && literal == "*") {
			cursor.fail("this IR was produced by the lossy pretty printer: pointer constants ('$x = *') carry no "
			            "value. Re-generate the file with the IR serializer.");
		}
		cursor.fail(fmt::format("invalid constant literal '{}' of type '{}'", literal, toString(stamp)));
	}

	Operation* parseBinaryOrCast(LineCursor& cursor, BasicBlock* block, uint32_t resultId) {
		auto* left = resolveValue(cursor.parseValueRef(), cursor);
		cursor.skipWs();
		const auto operatorToken = cursor.parseWord();
		cursor.skipWs();
		const OperationIdentifier identifier {resultId};
		if (operatorToken == "cast_to") {
			const auto targetType = cursor.parseType();
			const auto stamp = cursor.parseTrailingStamp();
			if (targetType != stamp) {
				cursor.fail("cast target type does not match the declared type");
			}
			auto* operation = block->addOperation<CastOperation>(identifier, left, targetType);
			defineValue(resultId, operation, cursor);
			return nullptr;
		}
		auto* right = resolveValue(cursor.parseValueRef(), cursor);
		if (operatorToken == "+") {
			return block->addOperation<AddOperation>(identifier, left, right);
		}
		if (operatorToken == "-") {
			return block->addOperation<SubOperation>(identifier, left, right);
		}
		if (operatorToken == "*") {
			return block->addOperation<MulOperation>(identifier, left, right);
		}
		if (operatorToken == "/") {
			return block->addOperation<DivOperation>(identifier, left, right);
		}
		if (operatorToken == "%") {
			return block->addOperation<ModOperation>(identifier, left, right);
		}
		if (operatorToken == "and") {
			return block->addOperation<AndOperation>(identifier, left, right);
		}
		if (operatorToken == "or") {
			return block->addOperation<OrOperation>(identifier, left, right);
		}
		if (operatorToken == "==") {
			return block->addOperation<CompareOperation>(identifier, left, right, CompareOperation::Comparator::EQ);
		}
		if (operatorToken == "!=") {
			return block->addOperation<CompareOperation>(identifier, left, right, CompareOperation::Comparator::NE);
		}
		if (operatorToken == "<") {
			return block->addOperation<CompareOperation>(identifier, left, right, CompareOperation::Comparator::LT);
		}
		if (operatorToken == "<=") {
			return block->addOperation<CompareOperation>(identifier, left, right, CompareOperation::Comparator::LE);
		}
		if (operatorToken == ">") {
			return block->addOperation<CompareOperation>(identifier, left, right, CompareOperation::Comparator::GT);
		}
		if (operatorToken == ">=") {
			return block->addOperation<CompareOperation>(identifier, left, right, CompareOperation::Comparator::GE);
		}
		if (operatorToken == "<<") {
			return block->addOperation<ShiftOperation>(identifier, left, right, ShiftOperation::ShiftType::LS);
		}
		if (operatorToken == ">>") {
			return block->addOperation<ShiftOperation>(identifier, left, right, ShiftOperation::ShiftType::RS);
		}
		if (operatorToken == "|") {
			return block->addOperation<BinaryCompOperation>(identifier, left, right, BinaryCompOperation::BOR);
		}
		if (operatorToken == "&") {
			return block->addOperation<BinaryCompOperation>(identifier, left, right, BinaryCompOperation::BAND);
		}
		if (operatorToken == "^") {
			return block->addOperation<BinaryCompOperation>(identifier, left, right, BinaryCompOperation::XOR);
		}
		cursor.fail(fmt::format("unknown operator '{}'", operatorToken));
	}

	void parseProxyCall(LineCursor& cursor, BasicBlock* block, uint32_t resultId, bool hasResult) {
		auto symbol = cursor.parseQuoted();
		cursor.skipWs();
		auto functionName = cursor.parseQuoted();
		const auto arguments = parseArgumentList(cursor);
		const auto attributes = parseCallAttributes(cursor);
		const auto stamp = cursor.parseTrailingStamp();
		if (hasResult == (stamp == Type::v)) {
			cursor.fail("a call has a result value exactly when its type is not void");
		}
		void* address = resolveFunctionAddress(symbol, functionName, cursor);
		auto* operation = block->addOperation<ProxyCallOperation>(
		    symbol, functionName, address, OperationIdentifier {resultId},
		    std::span<Operation* const>(arguments.data(), arguments.size()), stamp, attributes);
		if (hasResult) {
			defineValue(resultId, operation, cursor);
		}
	}

	void parseIndirectCall(LineCursor& cursor, BasicBlock* block, uint32_t resultId, bool hasResult) {
		auto* functionPointer = resolveValue(cursor.parseValueRef(), cursor);
		const auto arguments = parseArgumentList(cursor);
		const auto attributes = parseCallAttributes(cursor);
		const auto stamp = cursor.parseTrailingStamp();
		if (hasResult == (stamp == Type::v)) {
			cursor.fail("a call has a result value exactly when its type is not void");
		}
		auto* operation = block->addOperation<IndirectCallOperation>(
		    OperationIdentifier {resultId}, functionPointer,
		    std::span<Operation* const>(arguments.data(), arguments.size()), stamp, attributes);
		if (hasResult) {
			defineValue(resultId, operation, cursor);
		}
	}
};

} // namespace

std::shared_ptr<IRGraph> parseIR(std::string_view text, common::ArenaPool& pool, const CompilationUnitID& id,
                                 const IRSymbolResolverFn& symbolResolver) {
	auto graph = std::make_shared<IRGraph>(pool.acquire(), id);
	return IRTextParser(text, std::move(graph), symbolResolver).parse();
}

std::shared_ptr<IRGraph> parseIR(std::string_view text, const CompilationUnitID& id,
                                 const IRSymbolResolverFn& symbolResolver) {
	auto graph = std::make_shared<IRGraph>(id);
	return IRTextParser(text, std::move(graph), symbolResolver).parse();
}

} // namespace nautilus::compiler::ir
