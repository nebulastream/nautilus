#!/usr/bin/env node
// One-time helper: the benchmark workflow's max-items-in-chart used to be 20,
// so dev/bench/data.js only keeps the last 20 runs — but every older run is
// still recorded in the `pages` branch history. This script walks that history,
// unions all runs per entry (deduplicated by commit id), and writes a merged
// data.js capped at MAX_RUNS, which a maintainer can commit back to `pages`:
//
//   git fetch origin pages
//   node tools/benchmark-ui/backfill-history.mjs > /tmp/data.js
//   git worktree add /tmp/pages pages
//   cp /tmp/data.js /tmp/pages/dev/bench/data.js
//   (cd /tmp/pages && git commit -am "backfill benchmark history" && git push origin pages)
//   git worktree remove /tmp/pages

import { execFileSync } from 'node:child_process';

const MAX_RUNS = 200;
const REF = process.argv[2] || 'origin/pages';
const FILE = 'dev/bench/data.js';

function git(...args) {
	return execFileSync('git', args, { encoding: 'utf8', maxBuffer: 1024 * 1024 * 512 });
}

function parseDataJs(source) {
	const start = source.indexOf('{');
	const end = source.lastIndexOf('}');
	if (start === -1 || end === -1) {
		return null;
	}
	try {
		return JSON.parse(source.slice(start, end + 1));
	} catch {
		return null;
	}
}

const shas = git('log', '--format=%H', REF, '--', FILE).trim().split('\n').filter(Boolean);
console.error(`${shas.length} historical versions of ${FILE} on ${REF}`);

const merged = { lastUpdate: 0, repoUrl: '', entries: {} };
const seen = new Map(); // entryName -> Set<commit id>
let skipped = 0;

// Newest first so lastUpdate/repoUrl come from the most recent version.
for (const sha of shas) {
	let data;
	try {
		data = parseDataJs(git('show', `${sha}:${FILE}`));
	} catch {
		data = null;
	}
	if (!data || !data.entries) {
		skipped++;
		continue;
	}
	merged.lastUpdate = Math.max(merged.lastUpdate, data.lastUpdate || 0);
	merged.repoUrl = merged.repoUrl || data.repoUrl || '';
	for (const [entryName, runs] of Object.entries(data.entries)) {
		if (!merged.entries[entryName]) {
			merged.entries[entryName] = [];
			seen.set(entryName, new Set());
		}
		for (const run of runs) {
			const id = run.commit && run.commit.id;
			if (!id || seen.get(entryName).has(id)) {
				continue;
			}
			seen.get(entryName).add(id);
			merged.entries[entryName].push(run);
		}
	}
}

for (const entryName of Object.keys(merged.entries)) {
	const runs = merged.entries[entryName];
	runs.sort((a, b) => new Date(a.commit.timestamp) - new Date(b.commit.timestamp));
	if (runs.length > MAX_RUNS) {
		merged.entries[entryName] = runs.slice(runs.length - MAX_RUNS);
	}
	console.error(`entry "${entryName}": ${merged.entries[entryName].length} unique runs (of ${runs.length} found)`);
}
if (skipped > 0) {
	console.error(`skipped ${skipped} unparsable historical versions`);
}

process.stdout.write(`window.BENCHMARK_DATA = ${JSON.stringify(merged, null, 2)}`);
