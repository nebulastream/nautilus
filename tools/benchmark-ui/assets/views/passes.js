// Compilation passes view: for one run, one card per benchmark query with a
// stacked bar per nautilus configuration, splitting its compilation time into
// passes from tracing to code generation. The passes come from the second line
// of each query's compilation `extra` (see nes-parser.js).

import { formatValue, shortHash, firstLine, formatDate } from '../format.js';
import { splitExtra, passStage, PASS_STAGES } from '../nes-parser.js';

const STAGE_VARS = ['--s1', '--s2', '--s3', '--s4', '--s5', '--s6', '--s7', '--s8'];
const PHASE = 'query_compilation';

function cssVar(name) {
	return getComputedStyle(document.documentElement).getPropertyValue(name).trim();
}

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

// workload -> configuration -> { detail, totalMs, stages: stage -> { ms, passes } }
function breakdowns(model, run) {
	const result = new Map();
	for (const [workload, seriesMap] of model.catalog.get(PHASE) || []) {
		for (const [configuration, benchName] of seriesMap) {
			const bench = run.byName.get(benchName);
			const { detail, passes } = splitExtra(bench?.extra);
			if (!passes) {
				continue;
			}
			const stages = new Map();
			for (const pass of passes) {
				const key = passStage(pass.name);
				if (!stages.has(key)) {
					stages.set(key, { ms: 0, passes: [] });
				}
				stages.get(key).ms += pass.ms;
				stages.get(key).passes.push(pass);
			}
			if (!result.has(workload)) {
				result.set(workload, new Map());
			}
			const totalMs = passes.reduce((sum, p) => sum + p.ms, 0);
			result.get(workload).set(configuration, { detail, totalMs, stages });
		}
	}
	return result;
}

export function hashForPasses(model, params) {
	const q = new URLSearchParams();
	for (const key of ['run', 'hide', 'ms']) {
		if (params[key]) {
			q.set(key, params[key]);
		}
	}
	const qs = q.toString();
	return `#/${model.prefix}passes${qs ? '?' + qs : ''}`;
}

export function renderPasses(container, model, params, navigate) {
	container.innerHTML = '';
	container.appendChild(el('h2', null, 'Compilation passes per query'));

	// Only runs whose metrics carry the breakdown, newest first.
	const runIndices = model.runs
		.map((run, i) => i)
		.filter((i) => breakdowns(model, model.runs[i]).size > 0)
		.reverse();
	if (runIndices.length === 0) {
		container.appendChild(el('p', 'subtitle', 'No run carries a per-pass breakdown yet.'));
		return () => {};
	}
	let runIdx = params.run ? model.findRunIndex(params.run) : -1;
	if (!runIndices.includes(runIdx)) {
		runIdx = runIndices[0];
	}
	const run = model.runs[runIdx];
	const data = breakdowns(model, run);
	// Shares by default: C++ compiles 10-100x longer than TBC, which on a
	// common ms axis leaves the fast configurations' passes unreadable.
	const share = params.ms !== '1';
	const hidden = new Set(params.hide ? params.hide.split(',') : []);

	container.appendChild(
		el(
			'p',
			'subtitle',
			'The median time of each compilation pass over a query’s rounds, one bar per nautilus configuration. ' +
				'Hover a segment for the passes in it; click a bar for the query’s compilation trend.',
		),
	);

	const toolbar = el('div', 'toolbar');
	toolbar.appendChild(el('span', 'label', 'Run:'));
	const select = document.createElement('select');
	for (const i of runIndices) {
		const r = model.runs[i];
		const option = el(
			'option',
			null,
			`${shortHash(r.commit.id)} · ${formatDate(r.commit.timestamp)} · ${firstLine(r.commit.message).slice(0, 60)}`,
		);
		option.value = r.commit.id;
		option.selected = i === runIdx;
		select.appendChild(option);
	}
	select.onchange = () => navigate(hashForPasses(model, { ...params, run: select.value }));
	toolbar.appendChild(select);

	const msChip = el('button', 'chip' + (share ? '' : ' on'), 'absolute time (ms)');
	msChip.type = 'button';
	msChip.onclick = () => navigate(hashForPasses(model, { ...params, ms: share ? '1' : '' }));
	toolbar.appendChild(msChip);
	container.appendChild(toolbar);

	// Configuration chips, as on the trends view: hidden ones drop out of every card.
	const present = new Set();
	for (const perConfiguration of data.values()) {
		for (const configuration of perConfiguration.keys()) {
			present.add(configuration);
		}
	}
	const configurationBar = el('div', 'toolbar');
	configurationBar.appendChild(el('span', 'label', 'Configurations:'));
	for (const configuration of model.seriesOrder.filter((k) => present.has(k))) {
		const chip = el('button', 'chip' + (hidden.has(configuration) ? '' : ' on'), configuration);
		chip.type = 'button';
		chip.onclick = () => {
			const next = new Set(hidden);
			if (next.has(configuration)) {
				next.delete(configuration);
			} else {
				next.add(configuration);
			}
			navigate(hashForPasses(model, { ...params, hide: [...next].join(',') }));
		};
		configurationBar.appendChild(chip);
	}
	container.appendChild(configurationBar);

	// One legend for every card: the stages in pipeline order.
	const usedStages = new Set();
	for (const perConfiguration of data.values()) {
		for (const breakdown of perConfiguration.values()) {
			for (const key of breakdown.stages.keys()) {
				usedStages.add(key);
			}
		}
	}
	const stages = PASS_STAGES.map((stage, i) => ({ ...stage, color: cssVar(STAGE_VARS[i]) })).filter((s) =>
		usedStages.has(s.key),
	);
	const legend = el('div', 'toolbar legend');
	legend.appendChild(el('span', 'label', 'Passes:'));
	for (const stage of stages) {
		const item = el('span', 'legend-item');
		const swatch = el('span', 'swatch');
		swatch.style.background = stage.color;
		item.appendChild(swatch);
		item.appendChild(document.createTextNode(stage.label));
		legend.appendChild(item);
	}
	container.appendChild(legend);

	const grid = el('div', 'chart-grid');
	container.appendChild(grid);
	const charts = [];
	const ink = cssVar('--ink');
	const ink2 = cssVar('--ink-2');
	const mutedColor = cssVar('--muted');
	const gridColor = cssVar('--grid');
	const surfaceColor = cssVar('--surface');

	for (const [workload, perConfiguration] of data) {
		const configurations = model.seriesOrder.filter((k) => perConfiguration.has(k) && !hidden.has(k));
		if (configurations.length === 0) {
			continue;
		}
		const rows = configurations.map((k) => perConfiguration.get(k));
		const card = el('div', 'card');
		card.appendChild(el('h3', null, workload));
		const query = rows[0].detail.split(',')[0];
		card.appendChild(el('div', 'unit-note', `${query} · ${share ? '% of each configuration’s compilation' : 'ms · lower is better'}`));
		const plot = el('div', 'plot');
		plot.style.height = `${Math.max(140, 30 * configurations.length + 50)}px`;
		const canvas = document.createElement('canvas');
		plot.appendChild(canvas);
		card.appendChild(plot);
		grid.appendChild(card);

		const value = (row, ms) => (share ? (100 * ms) / (row.totalMs || 1) : ms);
		const datasets = stages
			.filter((stage) => rows.some((row) => row.stages.has(stage.key)))
			.map((stage) => ({
				label: stage.label,
				stageKey: stage.key,
				data: rows.map((row) => value(row, row.stages.get(stage.key)?.ms ?? 0)),
				backgroundColor: stage.color,
				// A thin surface-colored edge keeps adjacent segments apart.
				borderColor: surfaceColor,
				borderWidth: { right: 1 },
				borderSkipped: false,
				barPercentage: 0.8,
				categoryPercentage: 0.9,
			}));

		const chart = new Chart(canvas, {
			type: 'bar',
			data: { labels: configurations, datasets },
			options: {
				indexAxis: 'y',
				responsive: true,
				maintainAspectRatio: false,
				interaction: { mode: 'nearest', intersect: true },
				plugins: {
					legend: { display: false },
					tooltip: {
						backgroundColor: surfaceColor,
						titleColor: ink,
						bodyColor: ink2,
						footerColor: mutedColor,
						borderColor: cssVar('--border'),
						borderWidth: 1,
						callbacks: {
							title: (items) => `${items[0].label} · ${workload}`,
							label: (item) => {
								const row = rows[item.dataIndex];
								const stage = row.stages.get(item.dataset.stageKey);
								const ms = stage?.ms ?? 0;
								return `${item.dataset.label}: ${formatValue(ms)} ms (${formatValue((100 * ms) / (row.totalMs || 1))}%)`;
							},
							afterLabel: (item) => {
								const stage = rows[item.dataIndex].stages.get(item.dataset.stageKey);
								// A stage of one pass named like the stage needs no list.
								if (!stage || (stage.passes.length === 1 && ['tracing', 'ssaCreation', 'irGeneration', 'other'].includes(stage.passes[0].name))) {
									return [];
								}
								return stage.passes.map((p) => `  ${p.name}: ${formatValue(p.ms)} ms`);
							},
							footer: (items) => {
								const row = rows[items[0].dataIndex];
								return [`compilation ${formatValue(row.totalMs)} ms`, row.detail];
							},
						},
					},
				},
				scales: {
					x: {
						stacked: true,
						beginAtZero: true,
						max: share ? 100 : undefined,
						grid: { color: gridColor },
						border: { display: false },
						ticks: { color: mutedColor, autoSkip: true, autoSkipPadding: 10 },
						title: { display: true, text: share ? '% of compilation' : 'ms', color: mutedColor },
					},
					y: {
						stacked: true,
						grid: { display: false },
						border: { color: cssVar('--baseline') },
						ticks: { color: ink2 },
					},
				},
				onClick: () =>
					navigate(`#/${model.prefix}trends/${PHASE}?w=${encodeURIComponent(workload)}`),
			},
		});
		charts.push(chart);
	}

	return () => charts.forEach((c) => c.destroy());
}
