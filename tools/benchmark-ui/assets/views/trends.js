// Trends view: one line chart per workload of the selected phase, commits on
// the x-axis, one line per variation (tracer, backend, or backend/flag).

import { pickDisplayUnit, formatValue, shortHash, firstLine, formatDate } from '../format.js';

const SLOT_VARS = ['--s1', '--s2', '--s3', '--s4', '--s5', '--s6', '--s7', '--s8'];
const BACKEND_ORDER = ['mlir', 'cpp', 'tbc', 'asmjit', 'interpreted', 'twotier', 'singletier'];

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

// Fixed slot assignment: a series keeps its color on every chart of the phase
// (color follows the entity), or on every chart of the dataset when the model
// fixes the series order. Past 8 series the hue repeats with a dash pattern as
// secondary encoding rather than generating new hues.
function buildSlotMap(model, workloads) {
	const keys = new Set();
	for (const seriesMap of workloads.values()) {
		for (const key of seriesMap.keys()) {
			keys.add(key);
		}
	}
	const sorted = model.fixedSeriesOrder ? model.seriesOrder : [...keys].sort(compareSeriesKeys);
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
	title.textContent = `${model.phaseLabels[phase] || phase} — trends`;
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
	subtitle.textContent = model.fixedSeriesOrder
		? `${model.runs.length} runs on main · one chart per measurement, one line per nautilus configuration · ` +
			'click a point for the full run.'
		: `${model.runs.length} runs on main · one chart per workload, one line per variation · ` +
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
				navigate(hashFor(model, phase, { ...params, v: v || 'default' }));
			};
			toolbar.appendChild(chip);
		}
	}
	// Series chips: show or hide a series on every chart; hidden ones keep
	// their slot, so the visible ones never change color.
	const hidden = new Set(params.hide ? params.hide.split(',') : []);
	if (model.seriesFilter) {
		const label = document.createElement('span');
		label.className = 'label';
		label.textContent = 'Configurations:';
		toolbar.appendChild(label);
		const present = new Set();
		for (const seriesMap of workloads.values()) {
			for (const key of seriesMap.keys()) {
				present.add(key);
			}
		}
		for (const key of model.seriesOrder.filter((k) => present.has(k))) {
			const chip = document.createElement('button');
			chip.type = 'button';
			chip.className = 'chip series' + (hidden.has(key) ? '' : ' on');
			const swatch = document.createElement('span');
			swatch.className = 'swatch';
			chip.appendChild(swatch);
			chip.appendChild(document.createTextNode(key));
			chip.dataset.series = key;
			chip.onclick = () => {
				if (hidden.has(key)) {
					hidden.delete(key);
				} else {
					hidden.add(key);
				}
				navigate(hashFor(model, phase, { ...params, hide: [...hidden].join(',') }));
			};
			toolbar.appendChild(chip);
		}
	}
	// `log` is explicit in the URL once toggled; otherwise the dataset decides.
	const logScale = params.log !== undefined && params.log !== '' ? params.log === '1' : !!model.defaultLog;
	const logLabel = document.createElement('label');
	logLabel.className = 'chip' + (logScale ? ' on' : '');
	logLabel.style.userSelect = 'none';
	const logBox = document.createElement('input');
	logBox.type = 'checkbox';
	logBox.checked = logScale;
	logBox.style.display = 'none';
	logBox.onchange = () =>
		navigate(hashFor(model, phase, { ...params, log: logBox.checked === !!model.defaultLog ? '' : logBox.checked ? '1' : '0' }));
	logLabel.appendChild(logBox);
	logLabel.appendChild(document.createTextNode('log scale'));
	toolbar.appendChild(logLabel);
	container.appendChild(toolbar);

	const grid = document.createElement('div');
	grid.className = 'chart-grid';
	container.appendChild(grid);

	const slots = buildSlotMap(model, workloads);
	for (const chip of toolbar.querySelectorAll('.chip.series')) {
		chip.querySelector('.swatch').style.background = cssVar(slots.get(chip.dataset.series).colorVar);
	}
	const labels = model.runs.map((r) => shortHash(r.commit.id));
	const charts = [];
	const ink2 = cssVar('--ink-2');
	const mutedColor = cssVar('--muted');
	const gridColor = cssVar('--grid');
	const baselineColor = cssVar('--baseline');
	const surfaceColor = cssVar('--surface');

	const workloadNames = [...workloads.keys()].sort((a, b) =>
		model.workloadRank(a) !== model.workloadRank(b) ? model.workloadRank(a) - model.workloadRank(b) : a < b ? -1 : a > b ? 1 : 0,
	);
	for (const workload of workloadNames) {
		const seriesMap = workloads.get(workload);
		let entries = [...seriesMap.entries()]
			.filter(([key]) => !hidden.has(key))
			.sort((a, b) =>
				model.fixedSeriesOrder
					? model.seriesOrder.indexOf(a[0]) - model.seriesOrder.indexOf(b[0])
					: compareSeriesKeys(a[0], b[0]),
			);
		if (entries.length === 0) {
			continue;
		}
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
		let minNs = Infinity;
		for (const [, benchName] of entries) {
			for (const v of model.series(benchName)) {
				if (v !== null && v > maxNs) {
					maxNs = v;
				}
				if (v !== null && v > 0 && v < minNs) {
					minNs = v;
				}
			}
		}
		const unit = pickDisplayUnit(maxNs);
		// Left to the dataset, log applies only where values span 10x or more: on
		// a narrower range it adds nothing but a dense band of grid lines.
		const chartLog = logScale && (params.log === '1' || maxNs >= 10 * minNs);

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
								const detail = bench.range ? ` (raw ${bench.range})` : '';
								return `${item.dataset.label}: ${formatValue(item.parsed.y)} ${unit.label}${detail}`;
							},
							footer: (items) => {
								const bench = model.runs[items[0].dataIndex].byName.get(items[0].dataset.benchName);
								return bench && bench.extra ? [firstLine(bench.extra), 'click for run details'] : 'click for run details';
							},
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
						type: chartLog ? 'logarithmic' : 'linear',
						beginAtZero: !chartLog,
						grid: { color: gridColor },
						border: { display: false },
						title: { display: true, text: unit.label, color: mutedColor },
						ticks: { color: mutedColor, autoSkip: true, autoSkipPadding: 10 },
					},
				},
				onClick: (event, elements) => {
					if (elements.length > 0) {
						const run = model.runs[elements[0].index];
						navigate(`#/${model.prefix}run/${run.commit.id}`);
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

export function hashFor(model, phase, params) {
	const q = new URLSearchParams();
	if (params.w) {
		q.set('w', params.w);
	}
	if (params.v && params.v !== 'default') {
		q.set('v', params.v);
	}
	if (params.log) {
		q.set('log', params.log);
	}
	if (params.hide) {
		q.set('hide', params.hide);
	}
	const qs = q.toString();
	return `#/${model.prefix}trends/${phase}${qs ? '?' + qs : ''}`;
}
