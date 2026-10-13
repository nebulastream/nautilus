// Parses the metric names .github/nebulastream/nes-bench.py reports for the
// NebulaStream benchmark into the dimensions the views use. A configuration is
// a backend plus nautilus options (benchmark.toml) and never contains spaces:
//
//   {configuration} query {label}: compilation|execution
//   {configuration} suite compilation: total|median per pipeline
//   {configuration} suite compilation phase: {statistic}
//   {configuration} suite execution: geometric mean per query|{group}/ geometric mean
//   {configuration} suite generated code: {statistic}     (a count, not charted)
//
// The configuration is the series of every chart, so each one keeps its color
// across all of them.

export const NES_PHASE_ORDER = ['query_compilation', 'query_execution', 'suite_compilation', 'suite_execution'];

export const NES_PHASE_LABELS = {
	query_compilation: 'Compilation per query',
	query_execution: 'Execution per query',
	suite_compilation: 'Suite compilation',
	suite_execution: 'Suite execution',
	other: 'Other',
};

function mk(name, phase, configuration, workload) {
	return { name, phase, backend: configuration, workload, variant: 'default', seriesKey: configuration };
}

export function parseNes(name) {
	let m;
	if ((m = name.match(/^(\S+) query (.+): (compilation|execution)$/))) {
		return mk(name, `query_${m[3]}`, m[1], m[2]);
	}
	if ((m = name.match(/^(\S+) suite compilation phase: (.+)$/))) {
		return mk(name, 'suite_compilation', m[1], `phase ${m[2]}`);
	}
	if ((m = name.match(/^(\S+) suite (compilation|execution): (.+)$/))) {
		return mk(name, `suite_${m[2]}`, m[1], m[3]);
	}
	return mk(name, 'other', name.split(' ')[0], name);
}

// Headline measurements first, then the per-phase and per-directory breakdowns,
// each group alphabetical.
export function nesWorkloadRank(workload) {
	if (workload === 'total' || workload === 'geometric mean per query') {
		return 0;
	}
	if (workload === 'median per pipeline') {
		return 1;
	}
	return 2;
}

// A query's compilation metric carries the median time of each compilation
// pass as the second line of its `extra`: `passes tracing=4.1 ssaCreation=1.5
// ...` in ms, adding up to the compilation time. The first line is the
// human-readable detail (query, pipelines, rounds).
export function splitExtra(extra) {
	const [detail = '', ...rest] = (extra || '').split('\n');
	const line = rest.find((l) => l.startsWith('passes '));
	if (!line) {
		return { detail, passes: null };
	}
	const passes = [];
	for (const token of line.slice('passes '.length).split(' ')) {
		const at = token.lastIndexOf('=');
		const ms = Number(token.slice(at + 1));
		if (at > 0 && isFinite(ms)) {
			passes.push({ name: token.slice(0, at), ms });
		}
	}
	return { detail, passes };
}

// The breakdown colors passes by compilation stage, in pipeline order, so the
// eight stages fit the categorical palette; the tooltip names every pass.
export const PASS_STAGES = [
	{ key: 'tracing', label: 'Tracing' },
	{ key: 'ssa', label: 'SSA creation' },
	{ key: 'ir', label: 'IR generation' },
	{ key: 'irPasses', label: 'IR passes' },
	{ key: 'lowering', label: 'Lowering' },
	{ key: 'optimization', label: 'MLIR / LLVM passes' },
	{ key: 'codegen', label: 'Code generation' },
	{ key: 'other', label: 'Other' },
];

export function passStage(name) {
	if (name === 'tracing') {
		return 'tracing';
	}
	if (name === 'ssaCreation') {
		return 'ssa';
	}
	if (name === 'irGeneration') {
		return 'ir';
	}
	if (name.startsWith('irPasses.')) {
		return 'irPasses';
	}
	if (/lowering/i.test(name)) {
		return 'lowering';
	}
	if (name === 'mlir.pipeline' || name.startsWith('llvm.')) {
		return 'optimization';
	}
	// `backend` is backend time no pass of its own covers: all of it for the
	// bytecode backends, whose backend is code generation.
	if (name.endsWith('.compile') || name === 'backend') {
		return 'codegen';
	}
	return 'other';
}
