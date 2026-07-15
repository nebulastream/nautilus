// Turns window.BENCHMARK_DATA (written by benchmark-action/github-action-benchmark
// on the `pages` branch) into the structures the views need. Benches may appear
// or disappear between runs, so the catalog is the union over all runs and
// series contain nulls where a run lacks a bench.

import { parse } from './parser.js';
import { toNs } from './format.js';

export function buildModel(data) {
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
				names.set(name, parse(name));
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

	return {
		runs,
		names,
		catalog,
		series,
		findRunIndex,
		lastUpdate: data ? data.lastUpdate : null,
		repoUrl: data ? data.repoUrl : '',
	};
}
