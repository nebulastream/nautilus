// Parses the flat benchmark names emitted by nautilus-benchmarks (catch2) into
// structured dimensions. The grammar is defined by the name-building code in
// nautilus/test/benchmark/{TracingBenchmark,ExecutionBenchmark,TieredCompilationBenchmark}.cpp:
//
//   trace_{workload}             tracer = exception-based trace context
//   completing_trace_{workload}  tracer = lazy trace context
//   ssa_{workload}, ir_{workload}
//   comp_{backend}_{workload}
//   exec_{backend}_{workload}[_variantTag[_variantTag...]]
//   single_compile_{backend}_{workload}
//   tiered_{twotier|singletier}_{workload}
//   e2e_{anything}
//
// Workload names are camelCase and never contain underscores, so after the
// phase/backend prefix the first token is the workload and any remaining
// tokens form the variant tag. This keeps the parser free of a variant-tag
// whitelist and robust against new A/B flags showing up in the data.

export const PHASE_ORDER = ['trace', 'ssa', 'ir', 'comp', 'exec', 'single_compile', 'tiered', 'e2e', 'other'];

export const PHASE_LABELS = {
	trace: 'Tracing',
	ssa: 'SSA Creation',
	ir: 'IR Creation',
	comp: 'Compilation',
	exec: 'Execution',
	single_compile: 'Single Compile',
	tiered: 'Tiered',
	e2e: 'End-to-End',
	other: 'Other',
};

const BACKENDS = new Set(['mlir', 'cpp', 'bc', 'tbc', 'asmjit', 'interpreted']);

function mk(name, phase, backend, workload, variant) {
	let seriesKey;
	if (backend && variant !== 'default') {
		seriesKey = `${backend} / ${variant}`;
	} else if (backend) {
		seriesKey = backend;
	} else if (variant !== 'default') {
		seriesKey = variant;
	} else {
		seriesKey = 'value';
	}
	return { name, phase, backend, workload, variant, seriesKey };
}

export function parse(name) {
	let m;
	if ((m = name.match(/^completing_trace_(.+)$/))) {
		return mk(name, 'trace', null, m[1], 'lazy tracing');
	}
	if ((m = name.match(/^trace_(.+)$/))) {
		return mk(name, 'trace', null, m[1], 'exception-based tracing');
	}
	if ((m = name.match(/^(ssa|ir)_(.+)$/))) {
		return mk(name, m[1], null, m[2], 'default');
	}
	// e2e names embed underscores in the payload (e2e_tiered_bc_to_mlir); keep whole.
	if ((m = name.match(/^e2e_(.+)$/))) {
		return mk(name, 'e2e', null, m[1], 'default');
	}
	if ((m = name.match(/^tiered_(twotier|singletier)_(.+)$/))) {
		return mk(name, 'tiered', m[1], m[2], 'default');
	}
	if ((m = name.match(/^single_compile_([a-z]+)_(.+)$/)) && BACKENDS.has(m[1])) {
		return mk(name, 'single_compile', m[1], m[2], 'default');
	}
	if ((m = name.match(/^(comp|exec)_([a-z]+)_(.+)$/)) && BACKENDS.has(m[2])) {
		const tokens = m[3].split('_');
		const workload = tokens[0];
		const variant = tokens.length > 1 ? tokens.slice(1).join('_') : 'default';
		return mk(name, m[1], m[2], workload, variant);
	}
	return mk(name, 'other', null, name, 'default');
}
