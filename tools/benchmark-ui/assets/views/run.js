// Run detail view: everything one benchmark run (commit) measured, grouped by
// phase, with deltas against a baseline run (the chronologically previous one
// by default; the compare view swaps in an arbitrary baseline).

import { PHASE_ORDER, PHASE_LABELS } from '../parser.js';
import { formatNs, formatPercent, shortHash, firstLine, formatDate } from '../format.js';

// A run counts as regressed/improved beyond this percentage.
export const DELTA_THRESHOLD_PCT = 5;

function el(tag, className, text) {
	const node = document.createElement(tag);
	if (className) {
		node.className = className;
	}
	if (text !== undefined) {
		node.textContent = text;
	}
	return node;
}

function deltaCell(curNs, baseNs) {
	const td = el('td', 'num');
	if (curNs === null || baseNs === null || baseNs === undefined || baseNs === 0) {
		td.appendChild(el('span', 'delta flat', '—'));
		return td;
	}
	const pct = ((curNs - baseNs) / baseNs) * 100;
	const cls = pct > DELTA_THRESHOLD_PCT ? 'bad' : pct < -DELTA_THRESHOLD_PCT ? 'good' : 'flat';
	const span = el('span', `delta ${cls}`, formatPercent(pct));
	span.title = `${formatNs(baseNs)} → ${formatNs(curNs)}`;
	td.appendChild(span);
	return td;
}

// Renders per-phase delta tables of `run` against `baseRun` (may be null).
export function renderDeltaTables(container, model, run, baseRun, baseLabel) {
	const byPhase = new Map();
	for (const name of run.byName.keys()) {
		const parsed = model.names.get(name);
		if (!byPhase.has(parsed.phase)) {
			byPhase.set(parsed.phase, []);
		}
		byPhase.get(parsed.phase).push(parsed);
	}

	for (const phase of PHASE_ORDER) {
		const parsedList = byPhase.get(phase);
		if (!parsedList) {
			continue;
		}
		parsedList.sort((a, b) =>
			a.workload === b.workload
				? a.seriesKey < b.seriesKey
					? -1
					: 1
				: a.workload < b.workload
					? -1
					: 1,
		);

		container.appendChild(el('h2', null, PHASE_LABELS[phase] || phase));
		const wrap = el('div', 'table-wrap');
		const table = el('table', 'bench');
		const thead = el('thead');
		const headRow = el('tr');
		for (const [text, cls] of [
			['benchmark', null],
			['value', 'num'],
			['± (raw)', 'num'],
			[`Δ vs ${baseLabel}`, 'num'],
		]) {
			headRow.appendChild(el('th', cls, text));
		}
		thead.appendChild(headRow);
		table.appendChild(thead);
		const tbody = el('tbody');

		for (const parsed of parsedList) {
			const bench = run.byName.get(parsed.name);
			const baseBench = baseRun ? baseRun.byName.get(parsed.name) : undefined;
			const tr = el('tr');

			const nameTd = el('td');
			const link = el('a', 'mono', parsed.name);
			link.href = `#/trends/${parsed.phase}?w=${encodeURIComponent(parsed.workload)}`;
			nameTd.appendChild(link);
			if (baseRun && !baseBench) {
				nameTd.appendChild(el('span', 'badge', 'new'));
			}
			tr.appendChild(nameTd);

			tr.appendChild(el('td', 'num', formatNs(bench.valueNs)));
			tr.appendChild(el('td', 'num range', bench.range || ''));
			tr.appendChild(deltaCell(bench.valueNs, baseBench ? baseBench.valueNs : null));
			tbody.appendChild(tr);
		}
		table.appendChild(tbody);
		wrap.appendChild(table);
		container.appendChild(wrap);
	}

	if (baseRun) {
		const removed = [...baseRun.byName.keys()].filter((name) => !run.byName.has(name));
		if (removed.length > 0) {
			const note = el('p', 'section-note');
			note.style.margin = '0 0 20px';
			note.textContent = `Not measured in this run (present in ${baseLabel}): ${removed.join(', ')}`;
			container.appendChild(note);
		}
	}
}

function runHeader(model, run, idx) {
	const header = el('div', 'run-header');
	header.appendChild(el('div', 'msg', firstLine(run.commit.message)));
	const meta = el('div', 'meta');
	const link = el('a', 'mono', run.commit.id);
	link.href = run.commit.url;
	link.target = '_blank';
	link.rel = 'noopener';
	meta.appendChild(link);
	meta.appendChild(
		document.createTextNode(
			` · ${run.commit.author?.name || 'unknown'} · ${formatDate(run.commit.timestamp)} · run ${idx + 1} of ${model.runs.length}`,
		),
	);
	header.appendChild(meta);

	const nav = el('div', 'run-nav');
	if (idx > 0) {
		const prev = el('a', null, `← ${shortHash(model.runs[idx - 1].commit.id)} (older)`);
		prev.href = `#/run/${model.runs[idx - 1].commit.id}`;
		nav.appendChild(prev);
	}
	if (idx < model.runs.length - 1) {
		const next = el('a', null, `${shortHash(model.runs[idx + 1].commit.id)} (newer) →`);
		next.href = `#/run/${model.runs[idx + 1].commit.id}`;
		nav.appendChild(next);
	}
	const compare = el('a', null, 'compare against another run');
	compare.href = idx > 0 ? `#/compare/${model.runs[idx - 1].commit.id}..${run.commit.id}` : '#/compare/';
	nav.appendChild(compare);
	header.appendChild(nav);
	return header;
}

export function renderRun(container, model, sha) {
	container.innerHTML = '';
	const idx = model.findRunIndex(sha);
	if (idx === -1) {
		container.appendChild(el('h2', null, 'Run not found'));
		const p = el('p', 'subtitle');
		p.append('No benchmark run recorded for commit ');
		p.appendChild(el('span', 'mono', sha));
		p.append('. See the ');
		const a = el('a', null, 'run list');
		a.href = '#/runs';
		p.appendChild(a);
		p.append('.');
		container.appendChild(p);
		return () => {};
	}
	const run = model.runs[idx];
	const baseRun = idx > 0 ? model.runs[idx - 1] : null;
	container.appendChild(runHeader(model, run, idx));
	const note = el('p', 'section-note');
	note.textContent = baseRun
		? `Deltas vs previous run ${shortHash(baseRun.commit.id)}; green/red beyond ±${DELTA_THRESHOLD_PCT}%. ` +
			'The raw ± column is catch2 std-dev as published (unitless).'
		: 'First recorded run — no baseline for deltas.';
	container.appendChild(note);
	renderDeltaTables(container, model, run, baseRun, baseRun ? shortHash(baseRun.commit.id) : 'previous');
	return () => {};
}

export function renderRunList(container, model) {
	container.innerHTML = '';
	container.appendChild(el('h2', null, 'Benchmark runs'));
	container.appendChild(
		el('p', 'subtitle', `${model.runs.length} runs recorded on main, newest first. Counts use the ±${DELTA_THRESHOLD_PCT}% threshold vs the previous run.`),
	);
	const wrap = el('div', 'table-wrap');
	const table = el('table', 'bench');
	const thead = el('thead');
	const headRow = el('tr');
	for (const [text, cls] of [
		['date', null],
		['commit', null],
		['message', null],
		['author', null],
		['benches', 'num'],
		['regressions', 'num'],
		['improvements', 'num'],
	]) {
		headRow.appendChild(el('th', cls, text));
	}
	thead.appendChild(headRow);
	table.appendChild(thead);
	const tbody = el('tbody');

	for (let i = model.runs.length - 1; i >= 0; i--) {
		const run = model.runs[i];
		const prev = i > 0 ? model.runs[i - 1] : null;
		let regressions = 0;
		let improvements = 0;
		if (prev) {
			for (const [name, bench] of run.byName) {
				const base = prev.byName.get(name);
				if (!base || bench.valueNs === null || base.valueNs === null || base.valueNs === 0) {
					continue;
				}
				const pct = ((bench.valueNs - base.valueNs) / base.valueNs) * 100;
				if (pct > DELTA_THRESHOLD_PCT) {
					regressions++;
				} else if (pct < -DELTA_THRESHOLD_PCT) {
					improvements++;
				}
			}
		}
		const tr = el('tr');
		tr.appendChild(el('td', null, formatDate(run.commit.timestamp)));
		const shaTd = el('td');
		const link = el('a', 'mono', shortHash(run.commit.id));
		link.href = `#/run/${run.commit.id}`;
		shaTd.appendChild(link);
		tr.appendChild(shaTd);
		tr.appendChild(el('td', null, firstLine(run.commit.message)));
		tr.appendChild(el('td', null, run.commit.author?.name || ''));
		tr.appendChild(el('td', 'num', String(run.byName.size)));
		const regTd = el('td', 'num');
		regTd.appendChild(el('span', `delta ${regressions > 0 ? 'bad' : 'flat'}`, prev ? String(regressions) : '—'));
		tr.appendChild(regTd);
		const impTd = el('td', 'num');
		impTd.appendChild(el('span', `delta ${improvements > 0 ? 'good' : 'flat'}`, prev ? String(improvements) : '—'));
		tr.appendChild(impTd);
		tbody.appendChild(tr);
	}
	table.appendChild(tbody);
	wrap.appendChild(table);
	container.appendChild(wrap);
	return () => {};
}
