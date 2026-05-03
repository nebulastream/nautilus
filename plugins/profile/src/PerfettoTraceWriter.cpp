// Hand-rolled Perfetto trace writer. Encodes the subset of
// perfetto.protos.Trace needed to render regions + sampled stacks in
// https://ui.perfetto.dev — no Perfetto SDK or protoc dependency.
//
// Wire format (https://protobuf.dev/programming-guides/encoding/):
//   * Tag = (field_number << 3) | wire_type, encoded as a varint.
//   * Wire types used here:
//       0  varint (uint64_t / int64_t / bool / enum)
//       2  length-delimited (string / bytes / embedded message / packed)
//   * Embedded messages: write tag, write varint(length), append bytes.
// We buffer each nested message into a temporary std::string so we can
// prefix it with its length. That doubles the memory footprint relative
// to a streaming encoder but keeps the code dead-simple.
//
// Field numbers below come from the upstream perfetto.protos schema:
//   trace.proto, trace_packet.proto, track_event/track_descriptor.proto,
//   track_event/process_descriptor.proto, track_event/thread_descriptor.proto,
//   track_event/track_event.proto, track_event/counter_descriptor.proto,
//   profiling/profile_packet.proto, profiling/profile_common.proto,
//   interned_data/interned_data.proto.

#include "PerfettoTraceWriter.hpp"
#include <cstdint>
#include <fstream>
#include <functional>
#include <map>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace nautilus::profile::detail {

// Defined in profile_runtime.cpp. Converts session ticks to microseconds
// using the calibrated ticks_per_us.
uint64_t ticksToMicros(uint64_t ticks);

namespace {

// ---- Protobuf wire-format primitives ---------------------------------------

inline void writeRawVarint(std::string& out, uint64_t v) {
	while (v >= 0x80) {
		out.push_back(static_cast<char>(v | 0x80));
		v >>= 7;
	}
	out.push_back(static_cast<char>(v));
}

inline void writeTag(std::string& out, uint32_t fieldNum, uint32_t wireType) {
	writeRawVarint(out, (static_cast<uint64_t>(fieldNum) << 3) | wireType);
}

inline void pbVarint(std::string& out, uint32_t fieldNum, uint64_t v) {
	writeTag(out, fieldNum, 0);
	writeRawVarint(out, v);
}

inline void pbBytes(std::string& out, uint32_t fieldNum, const void* data, size_t len) {
	writeTag(out, fieldNum, 2);
	writeRawVarint(out, len);
	out.append(reinterpret_cast<const char*>(data), len);
}

inline void pbString(std::string& out, uint32_t fieldNum, std::string_view s) {
	pbBytes(out, fieldNum, s.data(), s.size());
}

template <typename Body>
void pbMessage(std::string& out, uint32_t fieldNum, Body&& body) {
	std::string buf;
	body(buf);
	writeTag(out, fieldNum, 2);
	writeRawVarint(out, buf.size());
	out.append(buf);
}

inline void pbPackedVarint(std::string& out, uint32_t fieldNum, const std::vector<uint64_t>& values) {
	if (values.empty()) {
		return;
	}
	std::string buf;
	for (auto v : values) {
		writeRawVarint(buf, v);
	}
	pbBytes(out, fieldNum, buf.data(), buf.size());
}

// ---- Perfetto field IDs (cross-checked against upstream proto schema) -----

// Trace
constexpr uint32_t kTrace_Packet = 1;

// TracePacket
constexpr uint32_t kTracePacket_Timestamp = 8;
constexpr uint32_t kTracePacket_TimestampClockId = 58;
constexpr uint32_t kTracePacket_TrustedPacketSequenceId = 10;
constexpr uint32_t kTracePacket_SequenceFlags = 13;
constexpr uint32_t kTracePacket_TrackEvent = 11;
constexpr uint32_t kTracePacket_InternedData = 12;
constexpr uint32_t kTracePacket_TrackDescriptor = 60;
constexpr uint32_t kTracePacket_PerfSample = 66;

// SequenceFlags (bitmask)
constexpr uint64_t kSeqIncrementalStateCleared = 1;
constexpr uint64_t kSeqNeedsIncrementalState = 2;

// BuiltinClock
constexpr uint64_t kBuiltinClockMonotonic = 3;

// TrackDescriptor
constexpr uint32_t kTrackDescriptor_Uuid = 1;
constexpr uint32_t kTrackDescriptor_Name = 2;
constexpr uint32_t kTrackDescriptor_Process = 3;
constexpr uint32_t kTrackDescriptor_Thread = 4;
constexpr uint32_t kTrackDescriptor_ParentUuid = 5;
constexpr uint32_t kTrackDescriptor_Counter = 8;

// ProcessDescriptor
constexpr uint32_t kProcessDescriptor_Pid = 1;
constexpr uint32_t kProcessDescriptor_Name = 6;

// ThreadDescriptor
constexpr uint32_t kThreadDescriptor_Pid = 1;
constexpr uint32_t kThreadDescriptor_Tid = 2;
constexpr uint32_t kThreadDescriptor_Name = 5;

// TrackEvent
constexpr uint32_t kTrackEvent_Categories = 22;
constexpr uint32_t kTrackEvent_Type = 9;
constexpr uint32_t kTrackEvent_TrackUuid = 11;
constexpr uint32_t kTrackEvent_Name = 23;
constexpr uint32_t kTrackEvent_CounterValue = 30;

// TrackEvent.Type
constexpr uint64_t kTrackEvent_TypeSliceBegin = 1;
constexpr uint64_t kTrackEvent_TypeSliceEnd = 2;
constexpr uint64_t kTrackEvent_TypeCounter = 4;

// CounterDescriptor (CategoryDescriptor.counter)
// Empty body is enough to mark a track as a counter track.

// InternedData
constexpr uint32_t kInternedData_FunctionNames = 5;
constexpr uint32_t kInternedData_Frames = 6;
constexpr uint32_t kInternedData_Callstacks = 7;
constexpr uint32_t kInternedData_Mappings = 19;

// InternedString
constexpr uint32_t kInternedString_Iid = 1;
constexpr uint32_t kInternedString_Str = 2;

// Frame
constexpr uint32_t kFrame_Iid = 1;
constexpr uint32_t kFrame_FunctionNameId = 2;
constexpr uint32_t kFrame_MappingId = 3;

// Callstack
constexpr uint32_t kCallstack_Iid = 1;
constexpr uint32_t kCallstack_FrameIds = 2;

// Mapping
constexpr uint32_t kMapping_Iid = 1;

// PerfSample
constexpr uint32_t kPerfSample_Pid = 2;
constexpr uint32_t kPerfSample_Tid = 3;
constexpr uint32_t kPerfSample_CallstackIid = 4;

// ---- Per-trace constants ---------------------------------------------------

constexpr uint64_t kSequenceId = 1; // single producer, fixed sequence
constexpr int64_t kPid = 1;         // logical pid for the process descriptor
constexpr uint64_t kProcessTrackUuid = 1;
constexpr uint64_t kMappingIid = 1;

// ---- Interning tables -----------------------------------------------------

struct CallstackKey {
	std::vector<uint64_t> frame_iids;
	bool operator==(const CallstackKey& o) const noexcept {
		return frame_iids == o.frame_iids;
	}
};
struct CallstackKeyHash {
	size_t operator()(const CallstackKey& k) const noexcept {
		size_t h = 0xcbf29ce484222325ULL;
		for (uint64_t v : k.frame_iids) {
			h ^= v;
			h *= 0x100000001b3ULL;
		}
		return h;
	}
};

} // namespace

bool writePerfettoTrace(const std::string& path, const std::vector<Event>& events) {
	std::string trace; // entire serialised Trace message

	// emitPacket wraps `packetBody` in a TracePacket with the shared
	// trusted_packet_sequence_id stamped on it.
	auto emitPacket = [&](auto packetBody) {
		pbMessage(trace, kTrace_Packet, [&](std::string& pkt) {
			packetBody(pkt);
			pbVarint(pkt, kTracePacket_TrustedPacketSequenceId, kSequenceId);
		});
	};

	// 1. Process descriptor — the root track.
	emitPacket([&](std::string& pkt) {
		pbMessage(pkt, kTracePacket_TrackDescriptor, [&](std::string& td) {
			pbVarint(td, kTrackDescriptor_Uuid, kProcessTrackUuid);
			pbMessage(td, kTrackDescriptor_Process, [&](std::string& proc) {
				pbVarint(proc, kProcessDescriptor_Pid, kPid);
				pbString(proc, kProcessDescriptor_Name, "nautilus");
			});
		});
	});

	// 2. Determine the set of (tid, module) virtual tracks. One slice
	//    track per pair (matches the Chrome JSON writer's behaviour) plus
	//    one counter track per pair so traceCounter() values render in
	//    the dedicated counter UI.
	std::map<std::pair<uint64_t, std::string>, uint64_t> sliceTrackUuid;
	std::map<std::pair<uint64_t, std::string>, uint64_t> counterTrackUuid;
	uint64_t nextUuid = 100;
	auto getSliceTrack = [&](uint64_t tid, const std::string& module) -> uint64_t {
		auto key = std::make_pair(tid, module);
		auto it = sliceTrackUuid.find(key);
		if (it != sliceTrackUuid.end()) {
			return it->second;
		}
		uint64_t uuid = nextUuid++;
		sliceTrackUuid[key] = uuid;
		return uuid;
	};
	auto getCounterTrack = [&](uint64_t tid, const std::string& module) -> uint64_t {
		auto key = std::make_pair(tid, module);
		auto it = counterTrackUuid.find(key);
		if (it != counterTrackUuid.end()) {
			return it->second;
		}
		uint64_t uuid = nextUuid++;
		counterTrackUuid[key] = uuid;
		return uuid;
	};

	for (const auto& ev : events) {
		switch (ev.kind) {
		case Event::Kind::Begin:
		case Event::Kind::End:
			getSliceTrack(ev.tid, ev.module);
			break;
		case Event::Kind::CounterI64:
			getCounterTrack(ev.tid, ev.module);
			break;
		case Event::Kind::Sample:
			break;
		}
	}

	// 3. Emit a TrackDescriptor per slice track and per counter track.
	for (const auto& [key, uuid] : sliceTrackUuid) {
		const auto& [tid, module] = key;
		emitPacket([&](std::string& pkt) {
			pbMessage(pkt, kTracePacket_TrackDescriptor, [&](std::string& td) {
				pbVarint(td, kTrackDescriptor_Uuid, uuid);
				pbVarint(td, kTrackDescriptor_ParentUuid, kProcessTrackUuid);
				const std::string& trackName = module.empty() ? "<default>" : module;
				pbMessage(td, kTrackDescriptor_Thread, [&](std::string& th) {
					pbVarint(th, kThreadDescriptor_Pid, kPid);
					pbVarint(th, kThreadDescriptor_Tid, static_cast<int64_t>(tid));
					pbString(th, kThreadDescriptor_Name, trackName);
				});
			});
		});
	}
	for (const auto& [key, uuid] : counterTrackUuid) {
		const auto& [tid, module] = key;
		emitPacket([&](std::string& pkt) {
			pbMessage(pkt, kTracePacket_TrackDescriptor, [&](std::string& td) {
				pbVarint(td, kTrackDescriptor_Uuid, uuid);
				pbVarint(td, kTrackDescriptor_ParentUuid, sliceTrackUuid.at(key));
				const std::string trackName = (module.empty() ? std::string("counters") : module + ".counters");
				pbString(td, kTrackDescriptor_Name, trackName);
				// Empty CounterDescriptor body marks this track as a counter
				// track so values render as a graph rather than a slice.
				pbMessage(td, kTrackDescriptor_Counter, [&](std::string&) {});
			});
		});
	}

	// 4. Build interning tables for sampled stacks.
	std::unordered_map<std::string, uint64_t> nameToIid;
	std::unordered_map<uint64_t, uint64_t> nameIidToFrameIid;
	std::unordered_map<CallstackKey, uint64_t, CallstackKeyHash> callstackToIid;
	uint64_t nextIid = 1;

	auto internName = [&](const std::string& name) -> uint64_t {
		auto it = nameToIid.find(name);
		if (it != nameToIid.end()) {
			return it->second;
		}
		uint64_t iid = nextIid++;
		nameToIid.emplace(name, iid);
		return iid;
	};
	auto internFrame = [&](uint64_t name_iid) -> uint64_t {
		auto it = nameIidToFrameIid.find(name_iid);
		if (it != nameIidToFrameIid.end()) {
			return it->second;
		}
		uint64_t iid = nextIid++;
		nameIidToFrameIid.emplace(name_iid, iid);
		return iid;
	};
	auto internCallstack = [&](CallstackKey key) -> uint64_t {
		auto it = callstackToIid.find(key);
		if (it != callstackToIid.end()) {
			return it->second;
		}
		uint64_t iid = nextIid++;
		callstackToIid.emplace(std::move(key), iid);
		return iid;
	};

	// One pass to assign callstack iids per Sample event.
	std::vector<uint64_t> sampleCallstackIid;
	sampleCallstackIid.reserve(events.size());
	for (const auto& ev : events) {
		if (ev.kind != Event::Kind::Sample) {
			sampleCallstackIid.push_back(0);
			continue;
		}
		// Sampler stores frames innermost-first which is also Perfetto's
		// expected order for Callstack.frame_ids (leaf to root).
		CallstackKey csk;
		csk.frame_iids.reserve(ev.stack.size());
		for (const auto& fname : ev.stack) {
			csk.frame_iids.push_back(internFrame(internName(fname)));
		}
		sampleCallstackIid.push_back(internCallstack(std::move(csk)));
	}

	// 5. Emit one InternedData packet for the whole trace. Marks the
	//    sequence's incremental state as cleared, so subsequent packets
	//    (which set NEEDS_INCREMENTAL_STATE) can resolve iids.
	emitPacket([&](std::string& pkt) {
		pbVarint(pkt, kTracePacket_SequenceFlags, kSeqIncrementalStateCleared);
		pbMessage(pkt, kTracePacket_InternedData, [&](std::string& id) {
			pbMessage(id, kInternedData_Mappings, [&](std::string& m) { pbVarint(m, kMapping_Iid, kMappingIid); });
			for (const auto& [name, iid] : nameToIid) {
				pbMessage(id, kInternedData_FunctionNames, [&](std::string& s) {
					pbVarint(s, kInternedString_Iid, iid);
					pbBytes(s, kInternedString_Str, name.data(), name.size());
				});
			}
			for (const auto& [name_iid, frame_iid] : nameIidToFrameIid) {
				pbMessage(id, kInternedData_Frames, [&](std::string& f) {
					pbVarint(f, kFrame_Iid, frame_iid);
					pbVarint(f, kFrame_FunctionNameId, name_iid);
					pbVarint(f, kFrame_MappingId, kMappingIid);
				});
			}
			for (const auto& [csk, cs_iid] : callstackToIid) {
				pbMessage(id, kInternedData_Callstacks, [&](std::string& c) {
					pbVarint(c, kCallstack_Iid, cs_iid);
					pbPackedVarint(c, kCallstack_FrameIds, csk.frame_iids);
				});
			}
		});
	});

	// 6. Emit one TracePacket per Event. Convert ticks to nanoseconds
	//    using the session's calibration; clock id BUILTIN_CLOCK_MONOTONIC.
	auto ticksToNs = [](uint64_t ticks) -> uint64_t {
		return ticksToMicros(ticks) * 1000ULL;
	};

	for (size_t i = 0; i < events.size(); ++i) {
		const auto& ev = events[i];
		emitPacket([&](std::string& pkt) {
			pbVarint(pkt, kTracePacket_Timestamp, ticksToNs(ev.timestamp_ticks));
			pbVarint(pkt, kTracePacket_TimestampClockId, kBuiltinClockMonotonic);
			pbVarint(pkt, kTracePacket_SequenceFlags, kSeqNeedsIncrementalState);

			switch (ev.kind) {
			case Event::Kind::Sample:
				pbMessage(pkt, kTracePacket_PerfSample, [&](std::string& ps) {
					pbVarint(ps, kPerfSample_Pid, kPid);
					pbVarint(ps, kPerfSample_Tid, ev.tid);
					pbVarint(ps, kPerfSample_CallstackIid, sampleCallstackIid[i]);
				});
				return;
			case Event::Kind::Begin:
			case Event::Kind::End: {
				const uint64_t track = sliceTrackUuid.at({ev.tid, ev.module});
				pbMessage(pkt, kTracePacket_TrackEvent, [&](std::string& te) {
					pbVarint(te, kTrackEvent_TrackUuid, track);
					pbVarint(te, kTrackEvent_Type,
					         ev.kind == Event::Kind::Begin ? kTrackEvent_TypeSliceBegin : kTrackEvent_TypeSliceEnd);
					if (ev.kind == Event::Kind::Begin) {
						pbString(te, kTrackEvent_Name, ev.name);
						pbString(te, kTrackEvent_Categories, "nautilus");
					}
				});
				return;
			}
			case Event::Kind::CounterI64: {
				const uint64_t track = counterTrackUuid.at({ev.tid, ev.module});
				pbMessage(pkt, kTracePacket_TrackEvent, [&](std::string& te) {
					pbVarint(te, kTrackEvent_TrackUuid, track);
					pbVarint(te, kTrackEvent_Type, kTrackEvent_TypeCounter);
					pbVarint(te, kTrackEvent_CounterValue, static_cast<uint64_t>(ev.value));
					pbString(te, kTrackEvent_Categories, "nautilus");
				});
				return;
			}
			}
		});
	}

	std::ofstream out(path, std::ios::binary);
	if (!out) {
		return false;
	}
	out.write(trace.data(), static_cast<std::streamsize>(trace.size()));
	return out.good();
}

} // namespace nautilus::profile::detail
