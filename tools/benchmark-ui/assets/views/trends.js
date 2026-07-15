// Trends view: one line chart per workload of the selected phase, commits on
// the x-axis, one line per variation (tracer, backend, or backend/flag).

import { PHASE_LABELS } from '../parser.js';
import { pickDisplayUnit, formatValue, shortHash, firstLine, formatDate } from '../format.js';

const SLOT_VARS = ['--s1', '--s2', '--s3', '--s4', '--s5', '--s6', '--s7', '--s8'];
const BACKEND_ORDER = ['mlir', 'cpp', 'bc', 'tbc', 'asmjit', 'interpreted', 'twotier', 'singletier'];

function cssVar(name) {
	return getComputedStyle(document.documentElement).getPropertyValue(name).trim();
}

function seriesKeyRank(key) {
	const [head, variant] = key.split(' / ');
	const backendIdx = BACKEND_ORDER.indexOf(head);
	return {
		backend: backendIdx === -1 ? BACKEND_ORDER.length : backendIdx,
		head,
		variant: variant || '',
	};
}

function compareSeriesKeys(a, b) {
	const ra = seriesKeyRank(a);
	const rb = seriesKeyRank(b);
	if (ra.backend !== rb.backend) {
		return ra.backend - rb.backend;
	}
	if (ra.head !== rb.head) {
		return ra.head < rb.head ? -1 : 1;
	}
	return ra.variant < rb.variant ? -1 : ra.variant > rb.variant ? 1 : 0;
}

// Fixed slot assignment per phase: a series keeps its color on every chart of
// the phase (color follows the entity). Past 8 series the hue repeats with a
// dash pattern as secondary encoding rather than generating new hues.
function buildSlotMap(workloads) {
	const keys = new Set();
	for (const seriesMap of workloads.values()) {
		for (const key of seriesMap.keys()) {
			keys.add(key);
		}
	}
	const sorted = [...keys].sort(compareSeriesKeys);
	const slots = new Map();
	sorted.forEach((key, i) => {
		slots.set(key, { colorVar: SLOT_VARS[i % 8], dashed: i >= 8 });
	});
	return slots;
}

function phaseVariants(model, workloads) {
	const variants = new Set();
	for (const seriesMap of workloads.values()) {
		for (const benchName of seriesMap.values()) {
			const parsed = model.names.get(benchName);
			if (parsed && parsed.backend) {
				variants.add(parsed.variant);
			}
		}
	}
	return [...variants].sort((a, b) => (a === 'default' ? -1 : b === 'default' ? 1 : a < b ? -1 : 1));
}

export function renderTrends(container, model, phase, params, navigate) {
	const workloads = model.catalog.get(phase);
	container.innerHTML = '';
	const title = document.createElement('h2');
	title.textContent = `${PHASE_LABELS[phase] || phase} — trends`;
	container.appendChild(title);

	if (!workloads || workloads.size === 0 || model.runs.length === 0) {
		const p = document.createElement('p');
		p.className = 'subtitle';
		p.textContent = 'No benchmark data for this phase.';
		container.appendChild(p);
		return () => {};
	}

	const subtitle = document.createElement('p');
	subtitle.className = 'subtitle';
	subtitle.textContent =
		`${model.runs.length} runs on main · one chart per workload, one line per variation · ` +
		'click a point for the full run.';
	container.appendChild(subtitle);

	// Toolbar: variant chips (only for phases with A/B flag variants) + log toggle.
	const variants = phaseVariants(model, workloads);
	const selected = new Set(params.v ? params.v.split(',') : ['default']);
	const toolbar = document.createElement('div');
	toolbar.className = 'toolbar';
	if (variants.length > 1) {
		const label = document.createElement('span');
		label.className = 'label';
		label.textContent = 'Variants:';
		toolbar.appendChild(label);
		for (const variant of variants) {
			const chip = document.createElement('button');
			chip.type = 'button';
			chip.className = 'chip' + (selected.has(variant) ? ' on' : '');
			chip.textContent = variant === 'default' ? 'baseline' : variant;
			chip.onclick = () => {
				if (selected.has(variant)) {
					selected.delete(variant);
				} else {
					selected.add(variant);
				}
				const v = [...selected].join(',');
				navigate(hashFor(phase, { ...params, v: v || 'default' }));
			};
			toolbar.appendChild(chip);
		}
	}
	const logLabel = document.createElement('label');
	logLabel.className = 'chip' + (params.log ? ' on' : '');
	logLabel.style.userSelect = 'none';
	const logBox = document.createElement('input');
	logBox.type = 'checkbox';
	logBox.checked = !!params.log;
	logBox.style.display = 'none';
	logBox.onchange = () => navigate(hashFor(phase, { ...params, log: logBox.checked ? '1' : '' }));
	logLabel.appendChild(logBox);
	logLabel.appendChild(document.createTextNode('log scale'));
	toolbar.appendChild(logLabel);
	container.appendChild(toolbar);

	const grid = document.createElement('div');
	grid.className = 'chart-grid';
	container.appendChild(grid);

	const slots = buildSlotMap(workloads);
	const labels = model.runs.map((r) => shortHash(r.commit.id));
	const charts = [];
	const ink2 = cssVar('--ink-2');
	const mutedColor = cssVar('--muted');
	const gridColor = cssVar('--grid');
	const baselineColor = cssVar('--baseline');
	const surfaceColor = cssVar('--surface');

	const workloadNames = [...workloads.keys()].sort();
	for (const workload of workloadNames) {
		const seriesMap = workloads.get(workload);
		let entries = [...seriesMap.entries()].sort((a, b) => compareSeriesKeys(a[0], b[0]));
		if (variants.length > 1) {
			const filtered = entries.filter(([, benchName]) => {
				const parsed = model.names.get(benchName);
				return !parsed.backend || selected.has(parsed.variant);
			});
			if (filtered.length > 0) {
				entries = filtered;
			}
		}

		let maxNs = 0;
		for (const [, benchName] of entries) {
			for (const v of model.series(benchName)) {
				if (v !== null && v > maxNs) {
					maxNs = v;
				}
			}
		}
		const unit = pickDisplayUnit(maxNs);

		const card = document.createElement('div');
		card.className = 'card';
		if (params.w === workload) {
			card.classList.add('highlight');
		}
		const h = document.createElement('h3');
		h.textContent = workload;
		card.appendChild(h);
		const note = document.createElement('div');
		note.className = 'unit-note';
		note.textContent = `${unit.label} · lower is better`;
		card.appendChild(note);
		const plot = document.createElement('div');
		plot.className = 'plot';
		const canvas = document.createElement('canvas');
		plot.appendChild(canvas);
		card.appendChild(plot);
		grid.appendChild(card);

		const datasets = entries.map(([key, benchName]) => {
			const slot = slots.get(key);
			const color = cssVar(slot.colorVar);
			return {
				label: key,
				benchName,
				data: model.series(benchName).map((v) => (v === null ? null : v / unit.div)),
				borderColor: color,
				backgroundColor: color,
				borderWidth: 2,
				pointRadius: 2.5,
				pointHoverRadius: 5,
				pointHitRadius: 10,
				spanGaps: true,
				borderDash: slot.dashed ? [6, 4] : undefined,
				segment: {
					// Dash the bridge over runs where this bench is missing.
					borderDash: (ctx) => (ctx.p0.skip || ctx.p1.skip ? [4, 4] : slot.dashed ? [6, 4] : undefined),
				},
			};
		});

		const chart = new Chart(canvas, {
			type: 'line',
			data: { labels, datasets },
			options: {
				responsive: true,
				maintainAspectRatio: false,
				interaction: { mode: 'nearest', intersect: false },
				plugins: {
					legend: {
						display: entries.length > 1,
						labels: { color: ink2, usePointStyle: true, pointStyle: 'line', boxHeight: 6 },
					},
					tooltip: {
						backgroundColor: surfaceColor,
						titleColor: cssVar('--ink'),
						bodyColor: ink2,
						footerColor: mutedColor,
						borderColor: cssVar('--border'),
						borderWidth: 1,
						callbacks: {
							title: (items) => {
								const run = model.runs[items[0].dataIndex];
								return `${shortHash(run.commit.id)} · ${firstLine(run.commit.message)}`;
							},
							afterTitle: (items) => {
								const run = model.runs[items[0].dataIndex];
								return `${run.commit.author?.name || ''} · ${formatDate(run.commit.timestamp)}`;
							},
							label: (item) => {
								const bench = model.runs[item.dataIndex].byName.get(item.dataset.benchName);
								if (!bench) {
									return `${item.dataset.label}: —`;
								}
								return `${item.dataset.label}: ${formatValue(item.parsed.y)} ${unit.label} (raw ${bench.range})`;
							},
							footer: () => 'click for run details',
						},
					},
				},
				scales: {
					x: {
						grid: { display: false },
						border: { color: baselineColor },
						ticks: { color: mutedColor, maxRotation: 60, autoSkip: true, font: { size: 10 } },
					},
					y: {
						type: params.log ? 'logarithmic' : 'linear',
						beginAtZero: !params.log,
						grid: { color: gridColor },
						border: { display: false },
						title: { display: true, text: unit.label, color: mutedColor },
						ticks: { color: mutedColor },
					},
				},
				onClick: (event, elements) => {
					if (elements.length > 0) {
						const run = model.runs[elements[0].index];
						navigate(`#/run/${run.commit.id}`);
					}
				},
			},
		});
		charts.push(chart);

		if (params.w === workload) {
			requestAnimationFrame(() => card.scrollIntoView({ block: 'center' }));
		}
	}

	return () => charts.forEach((c) => c.destroy());
}

export function hashFor(phase, params) {
	const q = new URLSearchParams();
	if (params.w) {
		q.set('w', params.w);
	}
	if (params.v && params.v !== 'default') {
		q.set('v', params.v);
	}
	if (params.log) {
		q.set('log', '1');
	}
	const qs = q.toString();
	return `#/trends/${phase}${qs ? '?' + qs : ''}`;
}
