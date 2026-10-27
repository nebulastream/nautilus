#include "nautilus/nir/Parser.hpp"
#include "nautilus/common/FloatText.hpp"
#include "nautilus/extensions/ExtensionRegistry.hpp"
#include "nautilus/nir/Operands.hpp"
#include "nautilus/nir/Verifier.hpp"
#include <algorithm>
#include <bit>
#include <charconv>
#include <cmath>
#include <map>

namespace nautilus::nir {

std::string ParseError::str() const {
	return std::string(codeName(code)) + " at " + std::to_string(line) + ":" + std::to_string(column) + ": " + message;
}

namespace {

// ---- lexer ---------------------------------------------------------------------------------------------------

enum class Tok { End, Ident, Number, Const, String, Bytes, Value, Block, Table, FName, Punct };

struct Token {
	Tok kind = Tok::End;
	std::string text; // identifier, number, literal of a constant, decoded string, punctuation
	std::string type; // Const: the type name
	uint8_t provenance = 0;
	bool bytes = false;  // Const: the literal is raw bytes (an extension-type constant)
	uint32_t number = 0; // Value, Block, Table: N
	char letter = 0;     // Table: c, p, i, b, d, l, r
	uint32_t line = 1, column = 1;
};

[[noreturn]] void fail(DiagCode code, const Token& t, std::string message) {
	throw ParseError {code, t.line, t.column, std::move(message)};
}

bool isNameStart(char c) {
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_' || c == '$';
}
bool isNameChar(char c) {
	return isNameStart(c) || (c >= '0' && c <= '9') || c == '.';
}
bool isDigit(char c) {
	return c >= '0' && c <= '9';
}
bool isHexDigit(char c) {
	return isDigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

class Lexer {
public:
	explicit Lexer(std::string_view s) : s_(s) {
	}

	std::vector<Token> run() {
		std::vector<Token> out;
		while (true) {
			skip();
			Token t;
			t.line = line_;
			t.column = col();
			if (pos_ >= s_.size()) {
				t.kind = Tok::End;
				out.push_back(t);
				return out;
			}
			lexOne(t);
			out.push_back(std::move(t));
		}
	}

private:
	uint32_t col() const {
		return static_cast<uint32_t>(pos_ - lineStart_) + 1;
	}
	char cur(size_t k = 0) const {
		return pos_ + k < s_.size() ? s_[pos_ + k] : '\0';
	}
	void advance() {
		if (s_[pos_] == '\n') {
			line_++;
			lineStart_ = pos_ + 1;
		}
		pos_++;
	}
	void skip() {
		while (pos_ < s_.size()) {
			char c = cur();
			if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
				advance();
			} else if (c == ';') {
				while (pos_ < s_.size() && cur() != '\n') {
					advance();
				}
			} else {
				break;
			}
		}
	}
	[[noreturn]] void error(Token& t, const std::string& m) {
		fail(DiagCode::P001, t, m);
	}

	uint32_t decimal(Token& t) {
		if (!isDigit(cur())) {
			error(t, "a decimal number is expected");
		}
		uint64_t v = 0;
		while (isDigit(cur())) {
			v = v * 10 + (cur() - '0');
			if (v > 0xFFFFFFFFull) {
				error(t, "number out of range");
			}
			advance();
		}
		return static_cast<uint32_t>(v);
	}

	std::string stringBody(Token& t) {
		std::string out;
		advance(); // opening quote
		while (true) {
			if (pos_ >= s_.size()) {
				error(t, "unterminated string");
			}
			char c = cur();
			if (c == '"') {
				advance();
				return out;
			}
			if (c == '\\') {
				advance();
				char e = cur();
				if (e == '\\' || e == '"') {
					out += e;
					advance();
				} else if (e == 'x' && isHexDigit(cur(1)) && isHexDigit(cur(2))) {
					out += static_cast<char>(std::stoi(std::string(s_.substr(pos_ + 1, 2)), nullptr, 16));
					advance();
					advance();
					advance();
				} else {
					error(t, "invalid escape in string");
				}
			} else {
				out += c;
				advance();
			}
		}
	}

	/// A number: decimal or hex integer, or a decimal or hex floating literal.
	std::string numberText() {
		size_t start = pos_;
		if (cur() == '-') {
			advance();
		}
		bool hex = cur() == '0' && (cur(1) == 'x' || cur(1) == 'X');
		while (isNameChar(cur()) ||
		       ((cur() == '+' || cur() == '-') &&
		        (hex ? (s_[pos_ - 1] == 'p' || s_[pos_ - 1] == 'P') : (s_[pos_ - 1] == 'e' || s_[pos_ - 1] == 'E')))) {
			advance();
		}
		return std::string(s_.substr(start, pos_ - start));
	}

	/// `!` and an extname, optionally followed by a balanced `<...>` group, as one token (13 §9.1).
	std::string extensionTypeText(Token& t) {
		size_t start = pos_;
		advance();
		while (isNameChar(cur())) {
			advance();
		}
		if (cur() == '<') {
			int depth = 0;
			do {
				if (cur() == '<') {
					++depth;
				} else if (cur() == '>') {
					--depth;
				} else if (cur() == '\0' || cur() == ' ' || cur() == '\n' || cur() == '\t') {
					error(t, "an unterminated extension type");
				}
				advance();
			} while (depth > 0);
		}
		return std::string(s_.substr(start, pos_ - start));
	}

	void lexOne(Token& t) {
		char c = cur();
		auto constantTail = [&](std::string literal) {
			// a constant is `literal:type[prov]` with no whitespace
			if (cur() != ':' || !(isNameStart(cur(1)) || cur(1) == '!')) {
				return false;
			}
			advance();
			size_t start = pos_;
			if (cur() == '!') {
				extensionTypeText(t);
			} else {
				while (isNameChar(cur())) {
					advance();
				}
			}
			t.kind = Tok::Const;
			t.text = std::move(literal);
			t.type = std::string(s_.substr(start, pos_ - start));
			if (cur() == '[') {
				size_t q = pos_ + 1;
				size_t e = s_.find(']', q);
				auto word = e == std::string_view::npos ? std::string_view() : s_.substr(q, e - q);
				if (word == "audited") {
					t.provenance = 1;
				} else if (word == "raw") {
					t.provenance = 2;
				} else {
					error(t, "provenance must be [audited] or [raw]");
				}
				while (pos_ <= e) {
					advance();
				}
			}
			return true;
		};
		if (c == '"') {
			t.kind = Tok::String;
			t.text = stringBody(t);
			return;
		}
		if (c == 'x' && cur(1) == '"') {
			advance();
			auto body = stringBody(t);
			if (body.size() % 2 != 0) {
				error(t, "bytes need an even number of hex digits");
			}
			std::string raw;
			for (size_t i = 0; i < body.size(); i += 2) {
				if (!isHexDigit(body[i]) || !isHexDigit(body[i + 1])) {
					error(t, "bytes contain a character that is not a hex digit");
				}
				raw += static_cast<char>(std::stoi(body.substr(i, 2), nullptr, 16));
			}
			t.kind = Tok::Bytes;
			t.text = raw;
			if (cur() == ':' && (isNameStart(cur(1)) || cur(1) == '!')) {
				// `x"..." : T` with no whitespace is the constant of an extension type
				constantTail(raw);
				t.bytes = true;
			}
			return;
		}
		if (c == '!') {
			t.kind = Tok::Ident;
			t.text = extensionTypeText(t);
			return;
		}
		if (c == '%') {
			advance();
			t.kind = Tok::Value;
			t.number = decimal(t);
			return;
		}
		if (c == '^') {
			advance();
			if (cur() != 'b') {
				error(t, "a block reference is ^bN");
			}
			advance();
			t.kind = Tok::Block;
			t.number = decimal(t);
			return;
		}
		if (c == '#') {
			advance();
			char l = cur();
			if (std::string("cpibdlr").find(l) == std::string::npos || l == '\0') {
				error(t, "unknown reference kind");
			}
			advance();
			t.kind = Tok::Table;
			t.letter = l;
			t.number = decimal(t);
			return;
		}
		if (c == '@') {
			advance();
			t.kind = Tok::FName;
			if (cur() == '"') {
				t.text = stringBody(t);
			} else if (isNameStart(cur())) {
				size_t start = pos_;
				while (isNameChar(cur())) {
					advance();
				}
				t.text = std::string(s_.substr(start, pos_ - start));
			} else {
				error(t, "a function name is expected after @");
			}
			return;
		}
		if (isDigit(c) || (c == '-' && (isDigit(cur(1)) || cur(1) == 'i' || cur(1) == 'n'))) {
			if (c == '-' && !isDigit(cur(1))) {
				// -inf, -nan(0x...)
				advance();
				std::string word = "-";
				size_t start = pos_;
				while (isNameChar(cur())) {
					advance();
				}
				word += std::string(s_.substr(start, pos_ - start));
				if (cur() == '(') {
					size_t e = s_.find(')', pos_);
					if (e == std::string_view::npos) {
						error(t, "unterminated nan literal");
					}
					while (pos_ <= e) {
						word += cur();
						advance();
					}
				}
				if (!constantTail(word)) {
					error(t, "a literal needs a type: " + word + ":type");
				}
				return;
			}
			auto text = numberText();
			if (!constantTail(text)) {
				t.kind = Tok::Number;
				t.text = text;
			}
			return;
		}
		if (isNameStart(c)) {
			size_t start = pos_;
			while (isNameChar(cur())) {
				advance();
			}
			std::string word(s_.substr(start, pos_ - start));
			if ((word == "true" || word == "false" || word == "inf") && constantTail(word)) {
				return;
			}
			if (word == "nan" && cur() == '(') {
				size_t e = s_.find(')', pos_);
				if (e == std::string_view::npos) {
					error(t, "unterminated nan literal");
				}
				while (pos_ <= e) {
					word += cur();
					advance();
				}
				if (!constantTail(word)) {
					error(t, "a literal needs a type");
				}
				return;
			}
			t.kind = Tok::Ident;
			t.text = std::move(word);
			return;
		}
		t.kind = Tok::Punct;
		if (c == ':' && cur(1) == ':') {
			t.text = "::";
			advance();
			advance();
		} else if (c == '-' && cur(1) == '>') {
			t.text = "->";
			advance();
			advance();
		} else if (std::string("()[]{},=:+").find(c) != std::string::npos) {
			t.text = std::string(1, c);
			advance();
		} else {
			error(t, std::string("unexpected character '") + c + "'");
		}
	}

	std::string_view s_;
	size_t pos_ = 0;
	size_t lineStart_ = 0;
	uint32_t line_ = 1;
};

// ---- parser --------------------------------------------------------------------------------------------------

struct Operand {
	Ref ref;
	TypeId type;
};

class Parser {
public:
	Parser(std::string_view text) : toks_(Lexer(text).run()) {
	}

	Module run() {
		expectIdent("nir");
		auto v = next();
		// the version is `1.<minor>`; the minor version is a short run of digits (a longer one is not a version)
		bool versionOk = v.kind == Tok::Number && v.text.rfind("1.", 0) == 0 && v.text.size() >= 3 &&
		                 v.text.size() <= 11 && v.text.find_first_not_of("0123456789", 2) == std::string::npos;
		if (!versionOk) {
			fail(DiagCode::P002, v, "the version must be 1.x");
		}
		if (std::stoul(v.text.substr(2)) > 0) {
			fail(DiagCode::P002, v, "unsupported minor version " + v.text);
		}
		if (acceptIdent("module")) {
			m_.module.nameId = m_.internName(expectString());
		}
		while (peek().kind == Tok::Ident && peek().text != "func") {
			declaration();
		}
		while (peek().kind == Tok::Ident && peek().text == "func") {
			function();
		}
		if (peek().kind != Tok::End) {
			fail(DiagCode::P002, peek(), "a declaration or function is expected");
		}
		finish();
		return std::move(m_);
	}

private:
	// ---- token helpers ---------------------------------------------------------------------------------------

	const Token& peek(size_t k = 0) const {
		return toks_[std::min(pos_ + k, toks_.size() - 1)];
	}
	Token next() {
		auto t = peek();
		if (pos_ < toks_.size() - 1) {
			pos_++;
		}
		return t;
	}
	bool isPunct(const char* p, size_t k = 0) const {
		return peek(k).kind == Tok::Punct && peek(k).text == p;
	}
	bool acceptPunct(const char* p) {
		if (isPunct(p)) {
			next();
			return true;
		}
		return false;
	}
	void expectPunct(const char* p) {
		if (!acceptPunct(p)) {
			fail(DiagCode::P002, peek(), std::string("'") + p + "' is expected");
		}
	}
	bool isIdent(const char* w, size_t k = 0) const {
		return peek(k).kind == Tok::Ident && peek(k).text == w;
	}
	bool acceptIdent(const char* w) {
		if (isIdent(w)) {
			next();
			return true;
		}
		return false;
	}
	void expectIdent(const char* w) {
		if (!acceptIdent(w)) {
			fail(DiagCode::P002, peek(), std::string("'") + w + "' is expected");
		}
	}
	std::string expectString() {
		auto t = next();
		if (t.kind != Tok::String) {
			fail(DiagCode::P002, t, "a string is expected");
		}
		return t.text;
	}

	uint64_t parseUInt(const Token& t) {
		if (t.kind != Tok::Number) {
			fail(DiagCode::P002, t, "a number is expected");
		}
		uint64_t v = 0;
		std::from_chars_result r {};
		if (t.text.rfind("0x", 0) == 0 || t.text.rfind("0X", 0) == 0) {
			r = std::from_chars(t.text.data() + 2, t.text.data() + t.text.size(), v, 16);
		} else {
			r = std::from_chars(t.text.data(), t.text.data() + t.text.size(), v, 10);
		}
		if (r.ec != std::errc() || r.ptr != t.text.data() + t.text.size()) {
			fail(DiagCode::P002, t, "an unsigned integer is expected");
		}
		return v;
	}
	uint64_t expectUInt() {
		return parseUInt(next());
	}

	/// A table reference `#<letter>N` that must be the next declaration.
	uint32_t declId(char letter, size_t expected) {
		auto t = next();
		if (t.kind != Tok::Table || t.letter != letter) {
			fail(DiagCode::P002, t, std::string("a #") + letter + "N reference is expected");
		}
		if (t.number != expected) {
			fail(DiagCode::P003, t,
			     "declaration " + std::to_string(t.number) + " is out of order, expected " + std::to_string(expected));
		}
		return t.number;
	}
	uint32_t tableRef(char letter) {
		auto t = next();
		if (t.kind != Tok::Table || t.letter != letter) {
			fail(DiagCode::P002, t, std::string("a #") + letter + "N reference is expected");
		}
		note(letter, t);
		return t.number;
	}
	void note(char letter, const Token& t) {
		auto& e = maxRef_[letter];
		if (!e.set || t.number > e.token.number) {
			e = {true, t};
		}
	}
	uint32_t blockRef() {
		auto t = next();
		if (t.kind != Tok::Block) {
			fail(DiagCode::P002, t, "a block reference is expected");
		}
		if (!maxBlock_.set || t.number > maxBlock_.token.number) {
			maxBlock_ = {true, t};
		}
		return t.number;
	}

	// ---- types, signatures, attributes -----------------------------------------------------------------------

	static bool typeByName(const std::string& n, TypeId& t) {
		static const std::map<std::string, TypeId> names = {
		    {"void", TypeId::Void}, {"bool", TypeId::Bool}, {"i8", TypeId::I8},
		    {"i16", TypeId::I16},   {"i32", TypeId::I32},   {"i64", TypeId::I64},
		    {"f32", TypeId::F32},   {"f64", TypeId::F64},   {"ptr", TypeId::Ptr}};
		if (!n.empty() && n[0] == '!') {
			auto id = ext::ExtensionRegistry::instance().findType(std::string_view(n).substr(1));
			if (!id) {
				return false;
			}
			t = static_cast<TypeId>(*id);
			return true;
		}
		auto it = names.find(n);
		if (it == names.end()) {
			return false;
		}
		t = it->second;
		return true;
	}

	TypeId parseType() {
		auto t = next();
		TypeId id;
		if (t.kind != Tok::Ident || !typeByName(t.text, id)) {
			fail(DiagCode::P002, t, "a type is expected");
		}
		return id;
	}

	TypeWord parsePType() {
		auto t = parseType();
		ExtKind k = ExtKind::None;
		if (acceptIdent("zext")) {
			k = ExtKind::Zext;
		} else if (acceptIdent("sext")) {
			k = ExtKind::Sext;
		}
		return makeTypeWord(t, k);
	}

	uint32_t parseSignature() {
		expectPunct("(");
		std::vector<TypeWord> params;
		if (!isPunct(")")) {
			do {
				params.push_back(parsePType());
			} while (acceptPunct(","));
		}
		expectPunct(")");
		expectPunct("->");
		auto result = parsePType();
		return m_.internSignature(result, params);
	}

	uint32_t parseAttrs() {
		if (!acceptIdent("attrs")) {
			return 0;
		}
		static const char* const names[] = {"noArgRead", "noArgWrite", "noOtherRead", "noOtherWrite",
		                                    "noUnwind",  "willReturn", "declared"};
		uint32_t bits = 0;
		expectPunct("(");
		do {
			auto t = next();
			bool found = false;
			for (uint32_t i = 0; i < 7 && t.kind == Tok::Ident; ++i) {
				if (t.text == names[i]) {
					bits |= 1u << i;
					found = true;
				}
			}
			if (!found) {
				fail(DiagCode::P002, t, "an attribute name is expected");
			}
		} while (acceptPunct(","));
		expectPunct(")");
		return bits;
	}

	// ---- declarations ----------------------------------------------------------------------------------------

	void declaration() {
		auto kw = next();
		const auto& w = kw.text;
		if (w == "extension") {
			extensionDeclaration(kw);
		} else if (w == "import") {
			declId('i', m_.imports.size());
			expectPunct("=");
			auto mod = m_.internName(expectString());
			expectPunct("::");
			auto fn = m_.internName(expectString());
			expectPunct(":");
			auto sig = parseSignature();
			m_.imports.push_back(ImportRecord {mod, fn, sig, parseAttrs()});
		} else if (w == "pointer") {
			declId('p', m_.pointers.size());
			expectPunct("=");
			auto k = next();
			PointerRecord r {};
			static const char* const kinds[] = {"callee", "thunk", "destructor", "address", "payload"};
			bool found = false;
			for (uint8_t i = 0; i < 5; ++i) {
				if (k.kind == Tok::Ident && k.text == kinds[i]) {
					r.kind = i;
					found = true;
				}
			}
			if (!found) {
				fail(DiagCode::P002, k, "a pointer kind is expected");
			}
			auto mode = next();
			if (mode.kind == Tok::Ident && mode.text == "local") {
				r.mode = 0;
				r.address = expectUInt();
				processLocal_ = true;
			} else if (mode.kind == Tok::Ident && mode.text == "symbol") {
				r.mode = 1;
				r.nameId = m_.internName(expectString());
			} else if (mode.kind == Tok::Ident && mode.text == "image") {
				r.mode = 2;
				r.nameId = m_.internName(expectString());
				expectPunct("+");
				r.address = expectUInt();
			} else {
				fail(DiagCode::P002, mode, "local, symbol or image is expected");
			}
			if (acceptIdent("audited")) {
				r.provenance = 1;
			} else if (acceptIdent("raw")) {
				r.provenance = 2;
			}
			m_.pointers.push_back(r);
		} else if (w == "binding") {
			declId('b', m_.bindings.size());
			expectPunct("=");
			auto name = m_.internName(expectString());
			expectPunct(":");
			auto type = parseType();
			expectIdent("size");
			auto size = static_cast<uint32_t>(expectUInt());
			expectIdent("align");
			auto align = static_cast<uint32_t>(expectUInt());
			m_.addBinding(name, type, size, align);
		} else if (w == "data") {
			declId('d', m_.data.size());
			expectIdent("align");
			auto align = static_cast<uint32_t>(expectUInt());
			expectPunct("=");
			auto bytes = next();
			if (bytes.kind != Tok::Bytes) {
				fail(DiagCode::P002, bytes, "bytes are expected");
			}
			if (align == 0 || (align & (align - 1)) != 0 || align > 4096) {
				fail(DiagCode::P002, kw, "the alignment of a data object is a power of two up to 4096");
			}
			size_t offset = (m_.dataBytes.size() + align - 1) / align * align;
			m_.dataBytes.resize(offset, std::byte {0});
			for (char c : bytes.text) {
				m_.dataBytes.push_back(static_cast<std::byte>(c));
			}
			m_.data.push_back(
			    DataRecord {static_cast<uint32_t>(bytes.text.size()), align, static_cast<uint32_t>(offset), 0});
		} else if (w == "callee") {
			declId('c', m_.callees.size());
			expectPunct("=");
			CalleeRecord r {};
			auto k = next();
			std::string internalName;
			Token internalTok;
			if (k.kind == Tok::Ident && k.text == "internal") {
				r.kind = static_cast<uint8_t>(CalleeKind::Internal);
				internalTok = next();
				if (internalTok.kind != Tok::FName) {
					fail(DiagCode::P002, internalTok, "a function name is expected");
				}
				r.target = NONE;
			} else if (k.kind == Tok::Ident && k.text == "external") {
				r.kind = static_cast<uint8_t>(CalleeKind::External);
				r.target = tableRef('p');
			} else if (k.kind == Tok::Ident && k.text == "imported") {
				r.kind = static_cast<uint8_t>(CalleeKind::Imported);
				r.target = tableRef('i');
			} else if (k.kind == Tok::Ident && k.text == "intrinsic") {
				r.kind = static_cast<uint8_t>(CalleeKind::Intrinsic);
				auto name = next();
				if (name.kind != Tok::Ident || !ext::ExtensionRegistry::instance().findIntrinsic(name.text)) {
					fail(DiagCode::P005, name, "an unknown intrinsic '" + name.text + "'");
				}
				r.target = intrinsicExtension(name.text);
			} else {
				fail(DiagCode::P002, k, "internal, external, imported or intrinsic is expected");
			}
			expectPunct(":");
			r.signature = parseSignature();
			r.attrs = parseAttrs();
			if (r.kind == static_cast<uint8_t>(CalleeKind::Internal)) {
				pendingInternal_.push_back({static_cast<uint32_t>(m_.callees.size()), internalTok});
			}
			m_.callees.push_back(r);
		} else {
			fail(DiagCode::P002, kw, "'" + w + "' is not a declaration (debug locations and regions come later)");
		}
	}

	// ---- functions -------------------------------------------------------------------------------------------

	struct PendingInst {
		uint32_t id;
		Inst inst;
		uint32_t block;
	};

	void function() {
		next(); // func
		bool exported = acceptIdent("export");
		bool pressa = acceptIdent("pressa");
		auto nameTok = next();
		if (nameTok.kind != Tok::FName) {
			fail(DiagCode::P002, nameTok, "a function name is expected");
		}
		expectPunct(":");
		FunctionRecord fr {};
		fr.nameId = m_.internName(nameTok.text);
		fr.signature = parseSignature();
		fr.attrs = parseAttrs();
		fr.level = pressa ? 0 : 1;
		fr.visibility = exported ? 1 : 0;
		fr.firstInst = static_cast<uint32_t>(m_.insts.size());
		fr.firstBlock = static_cast<uint32_t>(m_.blocks.size());
		fr.firstAlloca = static_cast<uint32_t>(m_.allocas.size());
		m_.functions.push_back(fr);
		fnIndex_ = static_cast<uint32_t>(m_.functions.size() - 1);
		level_ = pressa ? Level::PreSSA : Level::SSA;
		valueTypes_.clear();
		pending_.clear();
		blockLists_.clear();
		maxBlock_ = {};
		maxValue_ = {};
		instCount_ = 0;
		expectPunct("{");
		while (!isPunct("}") && peek().kind == Tok::Ident && (peek().text == "alloca" || peek().text == "attr")) {
			auto kw = next();
			if (kw.text == "alloca") {
				auto size = static_cast<uint32_t>(expectUInt());
				expectIdent("align");
				auto align = static_cast<uint32_t>(expectUInt());
				m_.allocas.push_back(AllocaRecord {size, align});
				m_.functions[fnIndex_].allocaCount++;
			} else {
				auto key = m_.internName(expectString());
				expectPunct("=");
				auto value = m_.internName(expectString());
				m_.functionAttrs.push_back(FunctionAttrRecord {fnIndex_, key, value, 0});
			}
		}
		if (peek().kind != Tok::Block) {
			fail(DiagCode::P002, peek(), "a block is expected");
		}
		while (peek().kind == Tok::Block) {
			block();
		}
		expectPunct("}");
		endFunction();
	}

	void endFunction() {
		auto& fn = m_.functions[fnIndex_];
		uint32_t blocks = static_cast<uint32_t>(fn.blockCount);
		if (maxBlock_.set && maxBlock_.token.number >= blocks) {
			fail(DiagCode::P003, maxBlock_.token,
			     "block ^b" + std::to_string(maxBlock_.token.number) + " is not defined");
		}
		if (level_ == Level::PreSSA) {
			std::vector<const PendingInst*> byId(pending_.size(), nullptr);
			for (auto& p : pending_) {
				if (p.id >= pending_.size() || byId[p.id] != nullptr) {
					fail(DiagCode::P004, peek(),
					     "instruction ids must be 0 ... count-1, each exactly once (id " + std::to_string(p.id) + ")");
				}
				byId[p.id] = &p;
			}
			for (auto* p : byId) {
				m_.insts.push_back(p->inst);
			}
			for (uint32_t b = 0; b < blocks; ++b) {
				auto& blk = m_.blocks[fn.firstBlock + b];
				blk.first = m_.addExtra(blockLists_[b]);
				blk.count = static_cast<uint32_t>(blockLists_[b].size());
			}
		}
		fn.instCount = static_cast<uint32_t>(m_.insts.size()) - fn.firstInst;
		if (maxValue_.set && maxValue_.token.number >= fn.instCount) {
			fail(DiagCode::P003, maxValue_.token, "%" + std::to_string(maxValue_.token.number) + " is not defined");
		}
	}

	void block() {
		auto label = next();
		auto& fn = m_.functions[fnIndex_];
		if (label.number != fn.blockCount) {
			fail(DiagCode::P003, label,
			     "blocks must be ^b0, ^b1, ... in order; expected ^b" + std::to_string(fn.blockCount));
		}
		BlockRecord blk {};
		uint32_t blockIndex = fn.blockCount++;
		if (level_ == Level::SSA) {
			blk.first = instCount_;
		} else {
			blockLists_.emplace_back();
		}
		m_.blocks.push_back(blk);
		auto idx = m_.blocks.size() - 1;
		uint32_t argCount = 0;
		if (acceptPunct("(")) {
			if (level_ == Level::PreSSA && blockIndex != 0) {
				fail(DiagCode::P002, peek(), "only block ^b0 of a pre-SSA function has arguments (the parameters)");
			}
			if (!isPunct(")")) {
				do {
					auto id = next();
					if (id.kind != Tok::Value) {
						fail(DiagCode::P002, id, "a block argument is %N: type");
					}
					expectPunct(":");
					auto type = parseType();
					Inst inst {static_cast<uint16_t>(Op::BlockArg), static_cast<uint8_t>(type), 0, argCount, 0, 0};
					define(id, inst, blockIndex, true);
					argCount++;
				} while (acceptPunct(","));
			}
			expectPunct(")");
		}
		if (level_ == Level::SSA) {
			m_.blocks[idx].argCount = static_cast<uint16_t>(argCount);
		}
		if (acceptIdent("pad")) {
			m_.blocks[idx].flags |= BLOCK_LANDING_PAD;
		}
		expectPunct(":");
		while (peek().kind != Tok::Block && !isPunct("}")) {
			instruction(blockIndex);
		}
		if (level_ == Level::SSA) {
			m_.blocks[idx].count = instCount_ - m_.blocks[idx].first;
		}
	}

	/// Records an instruction with the id it prints (or none for void SSA instructions).
	void define(const Token& idTok, const Inst& inst, uint32_t block, bool hasId) {
		uint32_t id = instCount_;
		if (hasId) {
			if (level_ == Level::SSA && idTok.number != id) {
				fail(DiagCode::P004, idTok,
				     "%" + std::to_string(idTok.number) + " is the instruction at position " + std::to_string(id));
			}
			id = idTok.number;
		} else if (level_ == Level::PreSSA) {
			fail(DiagCode::P002, idTok, "every pre-SSA instruction carries its id");
		}
		if (inst.type != static_cast<uint8_t>(TypeId::Void)) {
			valueTypes_[id] = static_cast<TypeId>(inst.type);
		}
		if (level_ == Level::SSA) {
			m_.insts.push_back(inst);
		} else {
			pending_.push_back({id, inst, block});
			blockLists_[block].push_back(id);
		}
		instCount_++;
	}

	// ---- operands --------------------------------------------------------------------------------------------

	Operand operand() {
		auto t = next();
		if (t.kind == Tok::Value) {
			auto it = valueTypes_.find(t.number);
			if (!maxValue_.set || t.number > maxValue_.token.number) {
				maxValue_ = {true, t};
			}
			return {t.number, it == valueTypes_.end() ? TypeId::Void : it->second};
		}
		if (t.kind == Tok::Const) {
			return constant(t);
		}
		fail(DiagCode::P002, t, "an operand (%N or a constant) is expected");
	}

	/// An operand whose type decides the result type: it must be defined already.
	Operand typedOperand() {
		auto t = peek();
		auto o = operand();
		if (t.kind == Tok::Value && o.type == TypeId::Void) {
			fail(DiagCode::P003, t, "%" + std::to_string(t.number) + " is not defined before this use");
		}
		return o;
	}

	bool startsOperand() const {
		if (peek().kind == Tok::Const) {
			return true;
		}
		// a value followed by `=` begins the next instruction
		return peek().kind == Tok::Value && !isPunct("=", 1);
	}

	Operand constant(const Token& t) {
		TypeId type;
		if (!typeByName(t.type, type) || type == TypeId::Void) {
			fail(DiagCode::P002, t, "unknown constant type '" + t.type + "'");
		}
		auto prov = static_cast<Provenance>(t.provenance);
		if (prov == Provenance::RawProcessValue) {
			processLocal_ = true;
		}
		const auto& lit = t.text;
		auto bad = [&](const std::string& why) {
			fail(DiagCode::P006, t, why);
		};
		uint64_t bits = 0;
		switch (type) {
		case TypeId::Ptr:
			fail(DiagCode::P006, t, "ptr has no constants");
		case TypeId::Bool:
			if (lit == "true") {
				bits = 1;
			} else if (lit != "false") {
				bad("a bool constant is true or false");
			}
			break;
		case TypeId::I8:
		case TypeId::I16:
		case TypeId::I32:
		case TypeId::I64: {
			unsigned n = sizeOf(type) * 8;
			bool neg = !lit.empty() && lit[0] == '-';
			std::string_view digits = std::string_view(lit).substr(neg ? 1 : 0);
			int base = 10;
			if (digits.rfind("0x", 0) == 0 || digits.rfind("0X", 0) == 0) {
				base = 16;
				digits.remove_prefix(2);
			}
			unsigned long long mag = 0;
			auto r = std::from_chars(digits.data(), digits.data() + digits.size(), mag, base);
			if (digits.empty() || r.ec != std::errc() || r.ptr != digits.data() + digits.size()) {
				bad("not an integer literal: " + lit);
			}
			unsigned long long maxPos = n == 64 ? ~0ull : (1ull << n) - 1;
			unsigned long long maxNeg = 1ull << (n - 1);
			if (neg ? mag > maxNeg : mag > maxPos) {
				bad("the literal " + lit + " is out of range for " + t.type);
			}
			bits = neg ? (~mag + 1) : mag;
			if (n < 64) {
				bits &= (1ull << n) - 1;
			}
			break;
		}
		case TypeId::F32:
		case TypeId::F64:
			bits = floatBits(t, type == TypeId::F32);
			break;
		default: {
			auto* info = ext::ExtensionRegistry::instance().type(static_cast<uint8_t>(type));
			if (info == nullptr || !t.bytes || t.text.size() != info->size) {
				bad("a constant of " + t.type + " is bytes of exactly " + (info ? std::to_string(info->size) : "?") +
				    " bytes");
			}
			auto payload = std::as_bytes(std::span<const char>(t.text.data(), t.text.size()));
			return {m_.consts.internBytes(type, prov, payload, info->align), type};
		}
		}
		return {m_.consts.intern(type, prov, bits), type};
	}

	uint64_t floatBits(const Token& t, bool single) {
		const auto& lit = t.text;
		bool neg = !lit.empty() && lit[0] == '-';
		std::string body = neg ? lit.substr(1) : lit;
		int mantissa = single ? 23 : 52;
		int exponent = single ? 8 : 11;
		uint64_t signBit = neg ? 1ull << (mantissa + exponent) : 0;
		uint64_t expAll = ((1ull << exponent) - 1) << mantissa;
		if (body == "inf") {
			return signBit | expAll;
		}
		if (body.rfind("nan(", 0) == 0) {
			std::string inner = body.substr(4, body.size() - 5);
			uint64_t v = 0;
			if (inner.rfind("0x", 0) != 0 ||
			    std::from_chars(inner.data() + 2, inner.data() + inner.size(), v, 16).ec != std::errc() || v == 0 ||
			    v >= (1ull << mantissa)) {
				fail(DiagCode::P006, t, "nan(0x...) needs a non-zero significand that fits the type");
			}
			return signBit | expAll | v;
		}
		bool hex = body.rfind("0x", 0) == 0 || body.rfind("0X", 0) == 0;
		std::string_view digits = std::string_view(body).substr(hex ? 2 : 0);
		if (single) {
			float f = 0;
			if (!nautilus::parseFloatText(digits, f, hex)) {
				fail(DiagCode::P006, t, "not a float literal: " + lit);
			}
			return std::bit_cast<uint32_t>(f) | signBit;
		}
		double d = 0;
		if (!nautilus::parseFloatText(digits, d, hex)) {
			fail(DiagCode::P006, t, "not a float literal: " + lit);
		}
		return std::bit_cast<uint64_t>(d) | signBit;
	}

	std::vector<uint32_t> argList(std::vector<TypeId>* types = nullptr) {
		std::vector<uint32_t> out;
		expectPunct("(");
		if (!isPunct(")")) {
			do {
				auto o = operand();
				out.push_back(o.ref);
				if (types) {
					types->push_back(o.type);
				}
			} while (acceptPunct(","));
		}
		expectPunct(")");
		return out;
	}

	/// `unwind caller`, `unwind ^bN`, `cleanup [...]` or nothing.
	uint32_t unwind() {
		if (acceptIdent("unwind")) {
			if (acceptIdent("caller")) {
				return NO_CLEANUP;
			}
			return blockRef();
		}
		if (acceptIdent("cleanup")) {
			std::vector<uint32_t> list {0};
			expectPunct("[");
			if (!isPunct("]")) {
				do {
					expectPunct("(");
					list.push_back(tableRef('c'));
					expectPunct(",");
					list.push_back(operand().ref);
					expectPunct(")");
					list[0]++;
				} while (acceptPunct(","));
			}
			expectPunct("]");
			return m_.addExtra(list);
		}
		return NO_UNWIND;
	}

	uint8_t alignCode() {
		if (!acceptIdent("align")) {
			return 0;
		}
		auto t = peek();
		auto n = expectUInt();
		if (n == 0 || (n & (n - 1)) != 0 || n > 4096) {
			fail(DiagCode::P002, t, "an alignment is a power of two from 1 to 4096");
		}
		return static_cast<uint8_t>(std::countr_zero(n) + 1);
	}

	// ---- instructions ----------------------------------------------------------------------------------------

	void instruction(uint32_t block) {
		Token idTok;
		bool hasId = false;
		bool bracket = false;
		if (peek().kind == Tok::Value && isPunct("=", 1)) {
			idTok = next();
			next();
			hasId = true;
		} else if (isPunct("[")) {
			next();
			idTok = next();
			if (idTok.kind != Tok::Number) {
				fail(DiagCode::P002, idTok, "an instruction id is expected");
			}
			idTok.number = static_cast<uint32_t>(parseUInt(idTok));
			expectPunct("]");
			hasId = true;
			bracket = true;
		}
		auto mn = next();
		if (mn.kind != Tok::Ident) {
			fail(DiagCode::P002, mn, "an instruction is expected");
		}
		TypeId type = TypeId::Void;
		Inst inst = op(mn, type);
		inst.type = static_cast<uint8_t>(type);
		if (type != TypeId::Void && !(hasId && !bracket)) {
			fail(DiagCode::P002, mn, "an instruction with a result is written %N = ...");
		}
		if (type == TypeId::Void && hasId && !bracket) {
			fail(DiagCode::P002, mn, "an instruction without a result has no %N =");
		}
		define(idTok, inst, block, hasId);
	}

	Inst op(const Token& mn, TypeId& type) {
		const auto& n = mn.text;
		auto I = [](Op o, uint8_t flags, uint32_t a = 0, uint32_t b = 0, uint32_t c = 0) {
			return Inst {static_cast<uint16_t>(o), 0, flags, a, b, c};
		};
		auto binary = [&](Op o, bool boolResult) {
			auto a = typedOperand();
			expectPunct(",");
			auto b = operand();
			type = boolResult ? TypeId::Bool : a.type;
			return I(o, 0, a.ref, b.ref);
		};
		auto unary = [&](Op o) {
			auto a = typedOperand();
			type = a.type;
			return I(o, 0, a.ref);
		};
		if (n == "add")
			return binary(Op::Add, false);
		if (n == "sub")
			return binary(Op::Sub, false);
		if (n == "mul")
			return binary(Op::Mul, false);
		if (n == "and")
			return binary(Op::And, true);
		if (n == "or")
			return binary(Op::Or, true);
		if (n == "band")
			return binary(Op::BAnd, false);
		if (n == "bor")
			return binary(Op::BOr, false);
		if (n == "bxor")
			return binary(Op::BXor, false);
		if (n == "shl")
			return binary(Op::Shl, false);
		if (n == "fneg")
			return unary(Op::FNeg);
		if (n == "bnot")
			return unary(Op::BNot);
		if (n == "not") {
			auto a = operand();
			type = TypeId::Bool;
			return I(Op::Not, 0, a.ref);
		}
		if (n == "shr.s" || n == "shr.u") {
			auto a = typedOperand();
			expectPunct(",");
			auto b = operand();
			type = a.type;
			return I(Op::Shr, n == "shr.u" ? 1 : 0, a.ref, b.ref);
		}
		if (n.rfind("div", 0) == 0 || n.rfind("rem", 0) == 0) {
			Op o = n[0] == 'd' ? Op::Div : Op::Rem;
			uint8_t flags = 0;
			if (n.find(".u") != std::string::npos) {
				flags |= 1;
			}
			if (n.find(".checked") != std::string::npos) {
				flags |= 2;
			}
			auto a = typedOperand();
			expectPunct(",");
			auto b = operand();
			type = a.type;
			uint32_t c = 0;
			if (flags & 2) {
				c = unwind();
			}
			return I(o, flags, a.ref, b.ref, c);
		}
		if (n.rfind("cmp.", 0) == 0) {
			static const char* const preds[] = {"eq", "ne", "lt", "le", "gt", "ge"};
			std::string rest = n.substr(4);
			uint8_t flags = 0;
			auto dot = rest.find('.');
			std::string pred = rest.substr(0, dot);
			bool found = false;
			for (uint8_t i = 0; i < 6; ++i) {
				if (pred == preds[i]) {
					flags = i;
					found = true;
				}
			}
			if (!found) {
				fail(DiagCode::P002, mn, "unknown comparison predicate");
			}
			if (dot != std::string::npos) {
				if (rest.substr(dot) == ".u") {
					flags |= 8;
				} else if (rest.substr(dot) != ".s") {
					fail(DiagCode::P002, mn, "a comparison suffix is .s or .u");
				}
			}
			auto a = typedOperand();
			expectPunct(",");
			auto b = operand();
			type = TypeId::Bool;
			return I(Op::Cmp, flags, a.ref, b.ref);
		}
		if (n == "select") {
			auto a = operand();
			expectPunct(",");
			auto b = typedOperand();
			expectPunct(",");
			auto c = operand();
			type = b.type;
			return I(Op::Select, 0, a.ref, b.ref, c.ref);
		}
		if (n.rfind("cast.", 0) == 0) {
			static const char* const kinds[] = {"sext",   "zext",  "trunc",   "sitofp",  "uitofp",   "fptosi",
			                                    "fptoui", "fpext", "fptrunc", "bitcast", "ptrtoint", "inttoptr"};
			uint8_t flags = 0;
			bool found = false;
			for (uint8_t i = 0; i < 12; ++i) {
				if (n.substr(5) == kinds[i]) {
					flags = i;
					found = true;
				}
			}
			if (!found) {
				fail(DiagCode::P002, mn, "unknown cast kind");
			}
			auto a = operand();
			expectPunct(":");
			type = parseType();
			return I(Op::Cast, flags, a.ref);
		}
		if (n == "load") {
			auto a = operand();
			expectPunct(":");
			type = parseType();
			return I(Op::Load, alignCode(), a.ref);
		}
		if (n == "store") {
			auto a = operand();
			expectPunct(",");
			auto b = operand();
			return I(Op::Store, alignCode(), a.ref, b.ref);
		}
		if (n == "alloca") {
			type = TypeId::Ptr;
			return I(Op::Alloca, 0, static_cast<uint32_t>(expectUInt()));
		}
		if (n == "call") {
			auto c = peek();
			auto callee = tableRef('c');
			auto args = argList();
			auto u = unwind();
			uint32_t thunk = NONE;
			if (acceptIdent("thunk")) {
				thunk = tableRef('p');
			}
			if (callee >= m_.callees.size()) {
				fail(DiagCode::P003, c, "#c" + std::to_string(callee) + " is not declared");
			}
			type = typeOf(m_.signatures[m_.callees[callee].signature].result);
			std::vector<uint32_t> list {static_cast<uint32_t>(args.size())};
			list.insert(list.end(), args.begin(), args.end());
			list.push_back(u);
			list.push_back(thunk);
			return I(Op::Call, 0, callee, 0, m_.addExtra(list));
		}
		if (n == "call.indirect") {
			auto a = operand();
			auto args = argList();
			expectPunct(":");
			auto sig = parseSignature();
			auto attrs = parseAttrs();
			auto u = unwind();
			uint32_t thunk = NONE;
			if (acceptIdent("thunk")) {
				thunk = tableRef('p');
			}
			type = typeOf(m_.signatures[sig].result);
			std::vector<uint32_t> list {static_cast<uint32_t>(args.size())};
			list.insert(list.end(), args.begin(), args.end());
			list.push_back(u);
			list.push_back(thunk);
			list.push_back(attrs);
			return I(Op::CallIndirect, 0, a.ref, sig, m_.addExtra(list));
		}
		if (n == "funcaddr") {
			type = TypeId::Ptr;
			return I(Op::FuncAddr, 0, tableRef('c'));
		}
		if (n == "bind.addr") {
			type = TypeId::Ptr;
			return I(Op::BindAddr, 0, tableRef('b'));
		}
		if (n == "data.addr") {
			type = TypeId::Ptr;
			return I(Op::DataAddr, 0, tableRef('d'));
		}
		if (n == "ptr.const") {
			type = TypeId::Ptr;
			if (acceptIdent("null")) {
				return I(Op::PtrConst, 0, NONE);
			}
			return I(Op::PtrConst, 0, tableRef('p'));
		}
		if (n == "br") {
			auto target = blockRef();
			std::vector<uint32_t> list {0};
			if (isPunct("(")) {
				auto args = argList();
				list[0] = static_cast<uint32_t>(args.size());
				list.insert(list.end(), args.begin(), args.end());
			}
			return I(Op::Br, 0, target, 0, m_.addExtra(list));
		}
		if (n == "condbr") {
			auto cond = operand();
			expectPunct(",");
			auto t = blockRef();
			std::vector<uint32_t> targs, fargs;
			if (isPunct("(")) {
				targs = argList();
			}
			expectPunct(",");
			auto f = blockRef();
			if (isPunct("(")) {
				fargs = argList();
			}
			uint32_t prob = NONE;
			if (acceptIdent("prob")) {
				auto pt = next();
				if (pt.kind != Tok::Number) {
					fail(DiagCode::P002, pt, "a probability is expected");
				}
				if (pt.text.find('.') != std::string::npos) {
					double p = 0;
					if (!nautilus::parseFloatText(pt.text, p) || p < 0 || p > 1) {
						fail(DiagCode::P006, pt, "a probability fraction is in [0, 1]");
					}
					prob = static_cast<uint32_t>(std::nearbyint(p * 4294967294.0));
				} else {
					prob = static_cast<uint32_t>(parseUInt(pt));
				}
			}
			std::vector<uint32_t> list {prob, static_cast<uint32_t>(targs.size())};
			list.insert(list.end(), targs.begin(), targs.end());
			list.push_back(f);
			list.push_back(static_cast<uint32_t>(fargs.size()));
			list.insert(list.end(), fargs.begin(), fargs.end());
			return I(Op::CondBr, 0, cond.ref, t, m_.addExtra(list));
		}
		if (n == "ret") {
			if (startsOperand()) {
				return I(Op::Ret, 0, operand().ref);
			}
			return I(Op::Ret, 0, NONE);
		}
		if (n == "resume") {
			return I(Op::Resume, 0);
		}
		if (n == "assign") {
			auto v = next();
			if (v.kind != Tok::Value) {
				fail(DiagCode::P002, v, "a variable (%N) is expected");
			}
			if (!maxValue_.set || v.number > maxValue_.token.number) {
				maxValue_ = {true, v};
			}
			expectPunct(",");
			auto b = operand();
			return I(Op::Assign, 0, v.number, b.ref);
		}
		if (n == "var") {
			auto a = typedOperand();
			type = a.type;
			return I(Op::Var, 0, a.ref);
		}
		if (n.find('.') != std::string::npos && ext::ExtensionRegistry::instance().findOp(n)) {
			return extensionOperation(mn, type);
		}
		fail(DiagCode::P002, mn, "unknown instruction '" + n + "'");
	}

	/// The `extensions` index of the intrinsic `name`, adding its record with the registered version if the text had no
	/// `extension` line for it.
	uint32_t intrinsicExtension(const std::string& name) {
		for (uint32_t i = 0; i < m_.extensions.size(); ++i) {
			std::string_view full = m_.name(m_.extensions[i].nameId), bare;
			uint32_t version;
			if (m_.extensions[i].kind == static_cast<uint8_t>(ExtensionKind::Intrinsic) &&
			    ext::splitNameVersion(full, bare, version) && bare == name) {
				return i;
			}
		}
		auto& registry = ext::ExtensionRegistry::instance();
		auto* info = registry.intrinsic(*registry.findIntrinsic(name));
		m_.extensions.push_back(ExtensionRecord {m_.internName(name + "@" + std::to_string(info->version)),
		                                         static_cast<uint8_t>(ExtensionKind::Intrinsic),
		                                         {}});
		return static_cast<uint32_t>(m_.extensions.size() - 1);
	}

	/// `extension "name@version"`: the registry must know the name and accept the version (13 §8.4, §9.4).
	void extensionDeclaration(const Token& kw) {
		auto strTok = peek();
		auto text = expectString();
		std::string_view bare;
		uint32_t version = 0;
		if (!ext::splitNameVersion(text, bare, version)) {
			fail(DiagCode::P005, strTok, "an extension is written \"name@version\"");
		}
		auto& registry = ext::ExtensionRegistry::instance();
		std::string name(bare);
		uint32_t current = 0, oldest = 0;
		ExtensionKind kind;
		if (auto op = registry.findOp(name)) {
			auto* info = registry.op(*op);
			current = info->version;
			oldest = info->minCompatibleVersion;
			kind = ExtensionKind::Operation;
		} else if (auto type = registry.findType(name)) {
			auto* info = registry.type(*type);
			current = info->version;
			oldest = info->minCompatibleVersion;
			kind = ExtensionKind::Type;
		} else if (auto intrinsic = registry.findIntrinsic(name)) {
			auto* info = registry.intrinsic(*intrinsic);
			current = info->version;
			oldest = info->minCompatibleVersion;
			kind = ExtensionKind::Intrinsic;
		} else {
			fail(DiagCode::P005, strTok, "the registry does not know the extension '" + name + "'");
		}
		if (version < oldest || version > current) {
			fail(DiagCode::P005, strTok,
			     "the registry accepts versions " + std::to_string(oldest) + " to " + std::to_string(current) +
			         " of '" + name + "'");
		}
		(void) kw;
		m_.extensions.push_back(ExtensionRecord {m_.internName(text), static_cast<uint8_t>(kind), {}});
	}

	/// An extension instruction, read generically from its operand shape (13 §5.11, §9.2): `name [flags(F)] f0, f1, ...
	/// [: T]`.
	Inst extensionOperation(const Token& mn, TypeId& type) {
		auto& registry = ext::ExtensionRegistry::instance();
		auto opcode = registry.findOp(mn.text);
		if (!opcode) {
			fail(DiagCode::P005, mn, "unknown instruction '" + mn.text + "'");
		}
		auto* info = registry.op(*opcode);
		Inst inst {*opcode, 0, 0, 0, 0, 0};
		if (acceptIdent("flags")) {
			expectPunct("(");
			auto f = peek();
			auto value = expectUInt();
			if (value > 255) {
				fail(DiagCode::P002, f, "flags are one byte");
			}
			inst.flags = static_cast<uint8_t>(value);
			expectPunct(")");
		}
		bool first = true;
		auto separator = [&] {
			if (!first) {
				expectPunct(",");
			}
			first = false;
		};
		auto field = [&](ext::FieldKind kind) -> uint32_t {
			switch (kind) {
			case ext::FieldKind::Unused:
				return 0;
			case ext::FieldKind::Value:
			case ext::FieldKind::Const:
				separator();
				return operand().ref;
			case ext::FieldKind::Imm: {
				separator();
				return static_cast<uint32_t>(expectUInt());
			}
			case ext::FieldKind::List: {
				separator();
				expectPunct("[");
				std::vector<uint32_t> list {0};
				if (!isPunct("]")) {
					do {
						auto k = list.size() - 1;
						ext::WordKind wk =
						    k < info->shape.prefix.size() ? info->shape.prefix[k]
						    : info->shape.group.empty()
						        ? ext::WordKind::Imm
						        : info->shape.group[(k - info->shape.prefix.size()) % info->shape.group.size()];
						switch (wk) {
						case ext::WordKind::Value:
						case ext::WordKind::Const:
							list.push_back(operand().ref);
							break;
						case ext::WordKind::Pointer:
							list.push_back(tableRef('p'));
							break;
						case ext::WordKind::Type:
							list.push_back(static_cast<uint32_t>(parseType()));
							break;
						case ext::WordKind::Imm:
							list.push_back(static_cast<uint32_t>(expectUInt()));
							break;
						}
						list[0]++;
					} while (acceptPunct(","));
				}
				expectPunct("]");
				return m_.addExtra(list);
			}
			}
			return 0;
		};
		inst.a = field(info->shape.a);
		inst.b = field(info->shape.b);
		inst.c = field(info->shape.c);
		type = TypeId::Void;
		if (acceptPunct(":")) {
			type = parseType();
		}
		return inst;
	}

	// ---- end of module ---------------------------------------------------------------------------------------

	void finish() {
		std::map<std::string, uint32_t> functions;
		for (uint32_t f = 0; f < m_.functions.size(); ++f) {
			functions.emplace(std::string(m_.name(m_.functions[f].nameId)), f);
		}
		for (auto& [callee, tok] : pendingInternal_) {
			auto it = functions.find(tok.text);
			if (it == functions.end()) {
				fail(DiagCode::P003, tok, "function @" + tok.text + " is not defined");
			}
			m_.callees[callee].target = it->second;
		}
		struct Limit {
			char letter;
			size_t size;
			const char* name;
		} limits[] = {{'c', m_.callees.size(), "callee"},
		              {'p', m_.pointers.size(), "pointer"},
		              {'i', m_.imports.size(), "import"},
		              {'b', m_.bindings.size(), "binding"},
		              {'d', m_.data.size(), "data object"}};
		for (auto& l : limits) {
			auto it = maxRef_.find(l.letter);
			if (it != maxRef_.end() && it->second.token.number >= l.size) {
				fail(DiagCode::P003, it->second.token,
				     std::string("the ") + l.name + " #" + l.letter + std::to_string(it->second.token.number) +
				         " is not declared");
			}
		}
		if (processLocal_) {
			m_.module.processToken = currentProcessToken();
		}
		m_.rebuildIndexes();
	}

	struct Mark {
		bool set = false;
		Token token = {};
	};

	std::vector<Token> toks_;
	size_t pos_ = 0;
	Module m_;
	bool processLocal_ = false;
	std::map<char, Mark> maxRef_;
	std::vector<std::pair<uint32_t, Token>> pendingInternal_;

	// per function
	uint32_t fnIndex_ = 0;
	Level level_ = Level::SSA;
	uint32_t instCount_ = 0;
	std::map<uint32_t, TypeId> valueTypes_;
	std::vector<PendingInst> pending_;
	std::vector<std::vector<uint32_t>> blockLists_;
	Mark maxBlock_, maxValue_;
};

} // namespace

ParseResult parse(std::string_view text, const ParseOptions& options) {
	ParseResult result;
	// a text parse is a first reader: the registry is complete from here on (04 §3.7)
	ext::ExtensionRegistry::instance().freeze();
	try {
		result.module = Parser(text).run();
	} catch (ParseError& e) {
		result.error = std::move(e);
		return result;
	}
	if (options.verify) {
		result.verifierDiagnostics = verify(*result.module);
	}
	return result;
}

} // namespace nautilus::nir
