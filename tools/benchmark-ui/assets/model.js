// Turns a BENCHMARK_DATA object (written by benchmark-action/github-action-benchmark
// on the `pages` branch) into the structures the views need. Benches may appear
// or disappear between runs, so the catalog is the union over all runs and
// series contain nulls where a run lacks a bench.
//
// A scheme describes one dataset: how its bench names parse, which phases the
// views show and in what order, and the route prefix its views live under. The
// views take everything dataset-specific from the model, so the tracing
// benchmark and the NebulaStream benchmark share them.

import { parse, PHASE_ORDER, PHASE_LABELS } from './parser.js';
import { parseNes, nesWorkloadRank, NES_PHASE_ORDER, NES_PHASE_LABELS } from './nes-parser.js';
import { toNs } from './format.js';

export const TRACING_SCHEME = {
	title: 'Nautilus',
	parse,
	phaseOrder: PHASE_ORDER,
	phaseLabels: PHASE_LABELS,
	prefix: '',
	// Series are tracers/backends; their colors are assigned per phase.
	fixedSeriesOrder: false,
	seriesFilter: false,
	defaultLog: false,
	workloadRank: () => 0,
};

export const NES_SCHEME = {
	title: 'NebulaStream',
	parse: parseNes,
	phaseOrder: NES_PHASE_ORDER,
	phaseLabels: NES_PHASE_LABELS,
	prefix: 'nes/',
	// Series are nautilus configurations: each keeps one color on every chart,
	// in the order the benchmark lists them, and can be hidden.
	fixedSeriesOrder: true,
	seriesFilter: true,
	// Configurations differ by 10-100x (C++ compiles in seconds, TBC in
	// milliseconds), so a linear axis flattens all but the slowest. Charts
	// whose values span less than 10x stay linear unless log is chosen.
	defaultLog: true,
	workloadRank: nesWorkloadRank,
};

export function buildModel(data, scheme = TRACING_SCHEME) {
	const entries = (data && data.entries) || {};
	const keys = Object.keys(entries);
	if (keys.length > 1) {
		console.warn('BENCHMARK_DATA has multiple entries, using the first:', keys);
	}
	const rawRuns = keys.length > 0 ? entries[keys[0]] : [];

	const runs = [...rawRuns]
		.sort((a, b) => new Date(a.commit.timestamp) - new Date(b.commit.timestamp))
		.map((run) => ({
			commit: run.commit,
			date: run.date,
			byName: new Map((run.benches || []).map((b) => [b.name, { ...b, valueNs: toNs(b.value, b.unit) }])),
		}));

	const names = new Map();
	for (const run of runs) {
		for (const name of run.byName.keys()) {
			if (!names.has(name)) {
				names.set(name, scheme.parse(name));
			}
		}
	}

	// phase -> workload -> seriesKey -> benchmark name
	const catalog = new Map();
	for (const p of names.values()) {
		if (!catalog.has(p.phase)) {
			catalog.set(p.phase, new Map());
		}
		const workloads = catalog.get(p.phase);
		if (!workloads.has(p.workload)) {
			workloads.set(p.workload, new Map());
		}
		workloads.get(p.workload).set(p.seriesKey, p.name);
	}

	const seriesCache = new Map();
	function series(benchName) {
		let s = seriesCache.get(benchName);
		if (!s) {
			s = runs.map((r) => r.byName.get(benchName)?.valueNs ?? null);
			seriesCache.set(benchName, s);
		}
		return s;
	}

	function findRunIndex(shaPrefix) {
		return runs.findIndex((r) => r.commit.id.startsWith(shaPrefix));
	}

	// Series keys in order of first appearance, oldest run first: the order the
	// benchmark reports them in, stable as long as the benchmark keeps it.
	const seriesOrder = [];
	for (const p of names.values()) {
		if (!seriesOrder.includes(p.seriesKey)) {
			seriesOrder.push(p.seriesKey);
		}
	}

	return {
		...scheme,
		seriesOrder,
		runs,
		names,
		catalog,
		series,
		findRunIndex,
		lastUpdate: data ? data.lastUpdate : null,
		repoUrl: data ? data.repoUrl : '',
	};
}
