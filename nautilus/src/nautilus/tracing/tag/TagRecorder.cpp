

#include "TagRecorder.hpp"
#include "nautilus/exceptions/NotImplementedException.hpp"
#include "nautilus/tracing/exceptions/TagCreationException.hpp"
#include <execinfo.h>

namespace nautilus::tracing {

#pragma GCC diagnostic ignored "-Wframe-address"

static void* getReturnAddress(uint32_t offset);

TagRecorder::TagRecorder(TagAddress startAddress) : startAddress(startAddress) {
	useBuiltinTagCreation = getReturnAddress(1) != nullptr;
}

// check if gnu backtrace is available.
#if defined(BACKWARD_HAS_BACKTRACE) & !defined(HOST_IS_MUSL)
TagVector TagRecorder::createBaseTag() {
        void* tagBuffer[MAX_TAG_SIZE];
	int size = backtrace(tagBuffer, MAX_TAG_SIZE);
	std::vector<TagAddress> addresses;
	for (int i = 0; i < size; i++) {
		addresses.emplace_back((TagAddress) tagBuffer[i]);
	}
	return {addresses};
}
#else

static void* getReturnAddress(uint32_t offset);

TagVector TagRecorder::createBaseTag() {
	// throw NotImplementedException("No plugin registered that can handle this
	// operation between");
	[[maybe_unused]] void* root = __builtin_thread_pointer();
	std::vector<TagAddress> addresses;
	for (size_t i = 0; i < MAX_TAG_SIZE; i++) {
		auto address = getReturnAddress(i);
		[[maybe_unused]] void* addr = __builtin_extract_return_addr(address);
		addresses.emplace_back((TagAddress) address);
	}
	return addresses;
}

#endif

TagAddress TagRecorder::getBaseAddress(TagVector& tag1, TagVector& tag2) {
	auto& tag1Addresses = tag1.getAddresses();
	auto& tag2Addresses = tag2.getAddresses();
	auto size = std::min(tag1Addresses.size(), tag2Addresses.size());
	size_t index = 0;
	while (index < size && tag1Addresses[index] == tag2Addresses[index]) {
		index++;
	}
	while (index < size && tag1Addresses[index] != tag2Addresses[index]) {
		index++;
	}
	if (index >= size) {
		return -1;
	}
	return tag1Addresses[index];
}

Tag* TagRecorder::createReferenceTagBacktrace() {
	auto* currentTagNode = &rootTagThreeNode;
	void* tagBuffer[MAX_TAG_SIZE];
	int size = backtrace(tagBuffer, MAX_TAG_SIZE);
	for (int i = 0; i < size; i++) {
		auto tagAddress = (TagAddress) tagBuffer[i];
		if (tagAddress == startAddress) {
			return currentTagNode;
		}
		currentTagNode = currentTagNode->append(tagAddress);
	}
	throw TagCreationException("Stack is too deep. This could indicate the use "
	                           "of recursive control-flow,"
	                           " which is not supported in Nautilus code.");
}

Tag* TagRecorder::createReferenceTagBuildin() {
	auto* currentTagNode = &rootTagThreeNode;
#pragma GCC diagnostic ignored "-Wframe-address"
	[[maybe_unused]] auto tag1 = __builtin_return_address(0);
	[[maybe_unused]] auto tag2 = __builtin_return_address(1);
	[[maybe_unused]] auto tag3 = __builtin_return_address(1);

	for (size_t i = 0; i <= MAX_TAG_SIZE; i++) {
		auto tagAddress = (TagAddress) getReturnAddress(i);
		if (tagAddress == startAddress) {
			return currentTagNode;
		}
		currentTagNode = currentTagNode->append(tagAddress);
	}
	throw TagCreationException("Stack is too deep. This could indicate the use "
	                           "of recursive control-flow,"
	                           " which is not supported in Nautilus code.");
}

Tag* TagRecorder::createReferenceTag() {
	if (useBuiltinTagCreation) {
		return createReferenceTagBuildin();
	}
	return createReferenceTagBacktrace();
}

static void* getReturnAddress(uint32_t offset) {
	switch (offset) {
	case 0:
		return __builtin_return_address(0 + 1);
	case 1:
		return __builtin_return_address(1 + 1);
	case 2:
		return __builtin_return_address(2 + 1);
	case 3:
		return __builtin_return_address(3 + 1);
	case 4:
		return __builtin_return_address(4 + 1);
	case 5:
		return __builtin_return_address(5 + 1);
	case 6:
		return __builtin_return_address(6 + 1);
	case 7:
		return __builtin_return_address(7 + 1);
	case 8:
		return __builtin_return_address(8 + 1);
	case 9:
		return __builtin_return_address(9 + 1);
	case 10:
		return __builtin_return_address(10 + 1);
	case 11:
		return __builtin_return_address(11 + 1);
	case 12:
		return __builtin_return_address(12 + 1);
	case 13:
		return __builtin_return_address(13 + 1);
	case 14:
		return __builtin_return_address(14 + 1);
	case 15:
		return __builtin_return_address(15 + 1);
	case 16:
		return __builtin_return_address(16 + 1);
	case 17:
		return __builtin_return_address(17 + 1);
	case 18:
		return __builtin_return_address(18 + 1);
	case 19:
		return __builtin_return_address(19 + 1);
	case 20:
		return __builtin_return_address(20 + 1);
	case 21:
		return __builtin_return_address(21 + 1);
	case 22:
		return __builtin_return_address(22 + 1);
	case 23:
		return __builtin_return_address(23 + 1);
	case 24:
		return __builtin_return_address(24 + 1);
	case 25:
		return __builtin_return_address(25 + 1);
	case 26:
		return __builtin_return_address(26 + 1);
	case 27:
		return __builtin_return_address(27 + 1);
	case 28:
		return __builtin_return_address(28 + 1);
	case 29:
		return __builtin_return_address(29 + 1);
	case 30:
		return __builtin_return_address(30 + 1);
	case 31:
		return __builtin_return_address(31 + 1);
	case 32:
		return __builtin_return_address(32 + 1);
	case 33:
		return __builtin_return_address(33 + 1);
	case 34:
		return __builtin_return_address(34 + 1);
	case 35:
		return __builtin_return_address(35 + 1);
	case 36:
		return __builtin_return_address(36 + 1);
	case 37:
		return __builtin_return_address(37 + 1);
	case 38:
		return __builtin_return_address(38 + 1);
	case 39:
		return __builtin_return_address(39 + 1);
	case 40:
		return __builtin_return_address(40 + 1);
	case 41:
		return __builtin_return_address(41 + 1);
	case 42:
		return __builtin_return_address(42 + 1);
	case 43:
		return __builtin_return_address(43 + 1);
	case 44:
		return __builtin_return_address(44 + 1);
	case 45:
		return __builtin_return_address(45 + 1);
	case 46:
		return __builtin_return_address(46 + 1);
	case 47:
		return __builtin_return_address(47 + 1);
	case 48:
		return __builtin_return_address(48 + 1);
	case 49:
		return __builtin_return_address(49 + 1);
	case 50:
		return __builtin_return_address(50 + 1);
	case 51:
		return __builtin_return_address(51 + 1);
	case 52:
		return __builtin_return_address(52 + 1);
	case 53:
		return __builtin_return_address(53 + 1);
	case 54:
		return __builtin_return_address(54 + 1);
	case 55:
		return __builtin_return_address(55 + 1);
	case 56:
		return __builtin_return_address(56 + 1);
	case 57:
		return __builtin_return_address(57 + 1);
	case 58:
		return __builtin_return_address(58 + 1);
	case 59:
		return __builtin_return_address(59 + 1);
	case 60:
		return __builtin_return_address(60 + 1);
	case 61:
		return __builtin_return_address(61 + 1);
	case 62:
		return __builtin_return_address(62 + 1);
	case 63:
		return __builtin_return_address(63 + 1);
	case 64:
		return __builtin_return_address(64 + 1);
	case 65:
		return __builtin_return_address(65 + 1);
	case 66:
		return __builtin_return_address(66 + 1);
	case 67:
		return __builtin_return_address(67 + 1);
	case 68:
		return __builtin_return_address(68 + 1);
	case 69:
		return __builtin_return_address(69 + 1);
	case 70:
		return __builtin_return_address(70 + 1);
	case 71:
		return __builtin_return_address(71 + 1);
	case 72:
		return __builtin_return_address(72 + 1);
	case 73:
		return __builtin_return_address(73 + 1);
	case 74:
		return __builtin_return_address(74 + 1);
	case 75:
		return __builtin_return_address(75 + 1);
	case 76:
		return __builtin_return_address(76 + 1);
	case 77:
		return __builtin_return_address(77 + 1);
	case 78:
		return __builtin_return_address(78 + 1);
	case 79:
		return __builtin_return_address(79 + 1);
	case 80:
		return __builtin_return_address(80 + 1);
	case 81:
		return __builtin_return_address(81 + 1);
	case 82:
		return __builtin_return_address(82 + 1);
	case 83:
		return __builtin_return_address(83 + 1);
	case 84:
		return __builtin_return_address(84 + 1);
	case 85:
		return __builtin_return_address(85 + 1);
	case 86:
		return __builtin_return_address(86 + 1);
	case 87:
		return __builtin_return_address(87 + 1);
	case 88:
		return __builtin_return_address(88 + 1);
	case 89:
		return __builtin_return_address(89 + 1);
	case 90:
		return __builtin_return_address(90 + 1);
	case 91:
		return __builtin_return_address(91 + 1);
	case 92:
		return __builtin_return_address(92 + 1);
	case 93:
		return __builtin_return_address(93 + 1);
	case 94:
		return __builtin_return_address(94 + 1);
	case 95:
		return __builtin_return_address(95 + 1);
	case 96:
		return __builtin_return_address(96 + 1);
	case 97:
		return __builtin_return_address(97 + 1);
	case 98:
		return __builtin_return_address(98 + 1);
	case 99:
		return __builtin_return_address(99 + 1);
	case 100:
		return __builtin_return_address(100 + 1);
	case 101:
		return __builtin_return_address(101 + 1);
	case 102:
		return __builtin_return_address(102 + 1);
	case 103:
		return __builtin_return_address(103 + 1);
	case 104:
		return __builtin_return_address(104 + 1);
	case 105:
		return __builtin_return_address(105 + 1);
	case 106:
		return __builtin_return_address(106 + 1);
	case 107:
		return __builtin_return_address(107 + 1);
	case 108:
		return __builtin_return_address(108 + 1);
	case 109:
		return __builtin_return_address(109 + 1);
	case 110:
		return __builtin_return_address(110 + 1);
	case 111:
		return __builtin_return_address(111 + 1);
	case 112:
		return __builtin_return_address(112 + 1);
	case 113:
		return __builtin_return_address(113 + 1);
	case 114:
		return __builtin_return_address(114 + 1);
	case 115:
		return __builtin_return_address(115 + 1);
	case 116:
		return __builtin_return_address(116 + 1);
	case 117:
		return __builtin_return_address(117 + 1);
	case 118:
		return __builtin_return_address(118 + 1);
	case 119:
		return __builtin_return_address(119 + 1);
	case 120:
		return __builtin_return_address(120 + 1);
	case 121:
		return __builtin_return_address(121 + 1);
	case 122:
		return __builtin_return_address(122 + 1);
	case 123:
		return __builtin_return_address(123 + 1);
	case 124:
		return __builtin_return_address(124 + 1);
	case 125:
		return __builtin_return_address(125 + 1);
	case 126:
		return __builtin_return_address(126 + 1);
	case 127:
		return __builtin_return_address(127 + 1);
	case 128:
		return __builtin_return_address(128 + 1);
	case 129:
		return __builtin_return_address(129 + 1);
	case 130:
		return __builtin_return_address(130 + 1);
	case 131:
		return __builtin_return_address(131 + 1);
	case 132:
		return __builtin_return_address(132 + 1);
	case 133:
		return __builtin_return_address(133 + 1);
	case 134:
		return __builtin_return_address(134 + 1);
	case 135:
		return __builtin_return_address(135 + 1);
	case 136:
		return __builtin_return_address(136 + 1);
	case 137:
		return __builtin_return_address(137 + 1);
	case 138:
		return __builtin_return_address(138 + 1);
	case 139:
		return __builtin_return_address(139 + 1);
	case 140:
		return __builtin_return_address(140 + 1);
	case 141:
		return __builtin_return_address(141 + 1);
	case 142:
		return __builtin_return_address(142 + 1);
	case 143:
		return __builtin_return_address(143 + 1);
	case 144:
		return __builtin_return_address(144 + 1);
	case 145:
		return __builtin_return_address(145 + 1);
	case 146:
		return __builtin_return_address(146 + 1);
	case 147:
		return __builtin_return_address(147 + 1);
	case 148:
		return __builtin_return_address(148 + 1);
	case 149:
		return __builtin_return_address(149 + 1);
	case 150:
		return __builtin_return_address(150 + 1);
	case 151:
		return __builtin_return_address(151 + 1);
	case 152:
		return __builtin_return_address(152 + 1);
	case 153:
		return __builtin_return_address(153 + 1);
	case 154:
		return __builtin_return_address(154 + 1);
	case 155:
		return __builtin_return_address(155 + 1);
	case 156:
		return __builtin_return_address(156 + 1);
	case 157:
		return __builtin_return_address(157 + 1);
	case 158:
		return __builtin_return_address(158 + 1);
	case 159:
		return __builtin_return_address(159 + 1);
	case 160:
		return __builtin_return_address(160 + 1);
	case 161:
		return __builtin_return_address(161 + 1);
	case 162:
		return __builtin_return_address(162 + 1);
	case 163:
		return __builtin_return_address(163 + 1);
	case 164:
		return __builtin_return_address(164 + 1);
	case 165:
		return __builtin_return_address(165 + 1);
	case 166:
		return __builtin_return_address(166 + 1);
	case 167:
		return __builtin_return_address(167 + 1);
	case 168:
		return __builtin_return_address(168 + 1);
	case 169:
		return __builtin_return_address(169 + 1);
	case 170:
		return __builtin_return_address(170 + 1);
	case 171:
		return __builtin_return_address(171 + 1);
	case 172:
		return __builtin_return_address(172 + 1);
	case 173:
		return __builtin_return_address(173 + 1);
	case 174:
		return __builtin_return_address(174 + 1);
	case 175:
		return __builtin_return_address(175 + 1);
	case 176:
		return __builtin_return_address(176 + 1);
	case 177:
		return __builtin_return_address(177 + 1);
	case 178:
		return __builtin_return_address(178 + 1);
	case 179:
		return __builtin_return_address(179 + 1);
	case 180:
		return __builtin_return_address(180 + 1);
	case 181:
		return __builtin_return_address(181 + 1);
	case 182:
		return __builtin_return_address(182 + 1);
	case 183:
		return __builtin_return_address(183 + 1);
	case 184:
		return __builtin_return_address(184 + 1);
	case 185:
		return __builtin_return_address(185 + 1);
	case 186:
		return __builtin_return_address(186 + 1);
	case 187:
		return __builtin_return_address(187 + 1);
	case 188:
		return __builtin_return_address(188 + 1);
	case 189:
		return __builtin_return_address(189 + 1);
	case 190:
		return __builtin_return_address(190 + 1);
	case 191:
		return __builtin_return_address(191 + 1);
	case 192:
		return __builtin_return_address(192 + 1);
	case 193:
		return __builtin_return_address(193 + 1);
	case 194:
		return __builtin_return_address(194 + 1);
	case 195:
		return __builtin_return_address(195 + 1);
	case 196:
		return __builtin_return_address(196 + 1);
	case 197:
		return __builtin_return_address(197 + 1);
	case 198:
		return __builtin_return_address(198 + 1);
	case 199:
		return __builtin_return_address(199 + 1);
	case 200:
		return __builtin_return_address(200 + 1);
	case 201:
		return __builtin_return_address(201 + 1);
	case 202:
		return __builtin_return_address(202 + 1);
	case 203:
		return __builtin_return_address(203 + 1);
	case 204:
		return __builtin_return_address(204 + 1);
	case 205:
		return __builtin_return_address(205 + 1);
	case 206:
		return __builtin_return_address(206 + 1);
	case 207:
		return __builtin_return_address(207 + 1);
	case 208:
		return __builtin_return_address(208 + 1);
	case 209:
		return __builtin_return_address(209 + 1);
	case 210:
		return __builtin_return_address(210 + 1);
	case 211:
		return __builtin_return_address(211 + 1);
	case 212:
		return __builtin_return_address(212 + 1);
	case 213:
		return __builtin_return_address(213 + 1);
	case 214:
		return __builtin_return_address(214 + 1);
	case 215:
		return __builtin_return_address(215 + 1);
	case 216:
		return __builtin_return_address(216 + 1);
	case 217:
		return __builtin_return_address(217 + 1);
	case 218:
		return __builtin_return_address(218 + 1);
	case 219:
		return __builtin_return_address(219 + 1);
	case 220:
		return __builtin_return_address(220 + 1);
	case 221:
		return __builtin_return_address(221 + 1);
	case 222:
		return __builtin_return_address(222 + 1);
	case 223:
		return __builtin_return_address(223 + 1);
	case 224:
		return __builtin_return_address(224 + 1);
	case 225:
		return __builtin_return_address(225 + 1);
	case 226:
		return __builtin_return_address(226 + 1);
	case 227:
		return __builtin_return_address(227 + 1);
	case 228:
		return __builtin_return_address(228 + 1);
	case 229:
		return __builtin_return_address(229 + 1);
	case 230:
		return __builtin_return_address(230 + 1);
	case 231:
		return __builtin_return_address(231 + 1);
	case 232:
		return __builtin_return_address(232 + 1);
	case 233:
		return __builtin_return_address(233 + 1);
	case 234:
		return __builtin_return_address(234 + 1);
	case 235:
		return __builtin_return_address(235 + 1);
	case 236:
		return __builtin_return_address(236 + 1);
	case 237:
		return __builtin_return_address(237 + 1);
	case 238:
		return __builtin_return_address(238 + 1);
	case 239:
		return __builtin_return_address(239 + 1);
	case 240:
		return __builtin_return_address(240 + 1);
	case 241:
		return __builtin_return_address(241 + 1);
	case 242:
		return __builtin_return_address(242 + 1);
	case 243:
		return __builtin_return_address(243 + 1);
	case 244:
		return __builtin_return_address(244 + 1);
	case 245:
		return __builtin_return_address(245 + 1);
	case 246:
		return __builtin_return_address(246 + 1);
	case 247:
		return __builtin_return_address(247 + 1);
	case 248:
		return __builtin_return_address(248 + 1);
	case 249:
		return __builtin_return_address(249 + 1);
	case 250:
		return __builtin_return_address(250 + 1);
	case 251:
		return __builtin_return_address(251 + 1);
	case 252:
		return __builtin_return_address(252 + 1);
	case 253:
		return __builtin_return_address(253 + 1);
	case 254:
		return __builtin_return_address(254 + 1);
	case 255:
		return __builtin_return_address(255 + 1);
	case 256:
		return __builtin_return_address(256 + 1);
	default:
		return nullptr;
	}
}


} // namespace nautilus::tracing
