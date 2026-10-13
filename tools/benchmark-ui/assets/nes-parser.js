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
