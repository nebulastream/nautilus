// Entry point: builds a model per dataset, renders the shell (topbar, sidebar)
// and routes between views via the location hash so every state is linkable.
// Each dataset's views live under its route prefix:
//   #/trends/<phase>?w=<workload>&v=<variants>&log=1     tracing benchmark
//   #/run/<sha>       #/runs        #/compare/<baseSha>..<targetSha>
//   #/nes/trends/<phase>?w=<measurement>&hide=<configurations>&log=1
//   #/nes/run/<sha>   #/nes/runs    #/nes/compare/<baseSha>..<targetSha>

import { buildModel, TRACING_SCHEME, NES_SCHEME } from './model.js';
import { renderTrends } from './views/trends.js';
import { renderRun, renderRunList } from './views/run.js';
import { renderCompare } from './views/compare.js';

const content = document.getElementById('content');
const sidebar = document.getElementById('sidebar');

// The NebulaStream benchmark (.github/nebulastream) is published by
// github-action-benchmark next to this page's directory, in dev/nebulastream.
// Its data.js assigns the same global as ours, so it is fetched and parsed
// rather than loaded as a script. fetch-data.sh writes the local copy.
const NES_DATA_URLS = ['../nebulastream/data.js', './nebulastream-data.js'];

async function loadNesData() {
	for (const url of NES_DATA_URLS) {
		try {
			const response = await fetch(url);
			if (!response.ok) {
				continue;
			}
			const text = await response.text();
			return JSON.parse(text.slice(text.indexOf('{'), text.lastIndexOf('}') + 1));
		} catch {
			// Try the next location; a page without NebulaStream data still works.
		}
	}
	return null;
}

function navigate(hash) {
	if (location.hash === hash) {
		route();
	} else {
		location.hash = hash;
	}
}

function showEmptyState(message) {
	content.innerHTML = '';
	const box = document.createElement('div');
	box.className = 'empty-state';
	const h = document.createElement('h2');
	h.textContent = 'No benchmark data';
	box.appendChild(h);
	const p = document.createElement('p');
	p.textContent = message;
	box.appendChild(p);
	const hint = document.createElement('p');
	hint.innerHTML =
		'For local development run <code>./fetch-data.sh</code> next to ' +
		'<code>index.html</code> to grab the published <code>data.js</code>, then reload.';
	box.appendChild(hint);
	content.appendChild(box);
}

// ---- theme toggle (auto → explicit light/dark, persisted) ----
function applyTheme(theme) {
	if (theme === 'light' || theme === 'dark') {
		document.documentElement.dataset.theme = theme;
	} else {
		delete document.documentElement.dataset.theme;
	}
}

function initTheme(onChange) {
	const saved = localStorage.getItem('bench-theme') || 'auto';
	applyTheme(saved);
	const button = document.getElementById('theme-toggle');
	const icons = { auto: '◐ auto', light: '○ light', dark: '● dark' };
	let current = saved;
	button.textContent = icons[current] || icons.auto;
	button.onclick = () => {
		current = current === 'auto' ? 'light' : current === 'light' ? 'dark' : 'auto';
		localStorage.setItem('bench-theme', current);
		applyTheme(current);
		button.textContent = icons[current];
		onChange();
	};
	window.matchMedia('(prefers-color-scheme: dark)').addEventListener('change', onChange);
}

// ---- routing ----
const models = [];

function parseHash() {
	const hash = location.hash.replace(/^#\/?/, '');
	const [path, query] = hash.split('?');
	const params = Object.fromEntries(new URLSearchParams(query || ''));
	let segments = path.split('/').filter(Boolean);
	// The longest matching prefix picks the dataset; the tracing one has none.
	const model = [...models].sort((a, b) => b.prefix.length - a.prefix.length).find((m) => path.startsWith(m.prefix));
	if (!model) {
		return null;
	}
	segments = segments.slice(model.prefix.split('/').filter(Boolean).length);
	if (segments[0] === 'run' && segments[1]) {
		return { model, view: 'run', sha: segments[1] };
	}
	if (segments[0] === 'runs') {
		return { model, view: 'runs' };
	}
	if (segments[0] === 'compare') {
		const [base, target] = (segments[1] || '').split('..');
		return { model, view: 'compare', base, target };
	}
	if (segments[0] === 'trends' && segments[1]) {
		return { model, view: 'trends', phase: segments[1], params };
	}
	return null;
}

let cleanup = () => {};

function renderSidebarSection(model, active) {
	const isActive = active.model === model;
	const datasetTitle = document.createElement('div');
	datasetTitle.className = 'nav-dataset';
	datasetTitle.textContent = model.title;
	sidebar.appendChild(datasetTitle);
	const trendsTitle = document.createElement('div');
	trendsTitle.className = 'nav-title';
	trendsTitle.textContent = 'Trends';
	sidebar.appendChild(trendsTitle);
	for (const phase of model.phaseOrder) {
		if (!model.catalog.has(phase)) {
			continue;
		}
		const a = document.createElement('a');
		a.href = `#/${model.prefix}trends/${phase}`;
		a.textContent = model.phaseLabels[phase] || phase;
		if (isActive && active.view === 'trends' && active.phase === phase) {
			a.className = 'active';
		}
		sidebar.appendChild(a);
	}
	const runsTitle = document.createElement('div');
	runsTitle.className = 'nav-title';
	runsTitle.textContent = 'Runs';
	sidebar.appendChild(runsTitle);
	const runsLink = document.createElement('a');
	runsLink.href = `#/${model.prefix}runs`;
	runsLink.textContent = 'All runs';
	if (isActive && (active.view === 'runs' || active.view === 'run')) {
		runsLink.className = 'active';
	}
	sidebar.appendChild(runsLink);
	const compareLink = document.createElement('a');
	compareLink.href = `#/${model.prefix}compare/`;
	compareLink.textContent = 'Compare';
	if (isActive && active.view === 'compare') {
		compareLink.className = 'active';
	}
	sidebar.appendChild(compareLink);
}

function renderSidebar(active) {
	sidebar.innerHTML = '';
	for (const model of models) {
		renderSidebarSection(model, active);
	}
}

function updateMeta(model) {
	document.getElementById('meta-info').textContent =
		`${model.title}: ${model.runs.length} runs · last update ` +
		`${new Date(model.lastUpdate).toISOString().slice(0, 16).replace('T', ' ')} UTC`;
	document.getElementById('repo-link').href = model.repoUrl || '#';
}

function route() {
	cleanup();
	cleanup = () => {};
	const parsed = parseHash();
	if (!parsed) {
		const model = models[0];
		const defaultPhase = model.phaseOrder.find((p) => model.catalog.has(p)) || model.phaseOrder[0];
		location.replace(`#/${model.prefix}trends/${defaultPhase}`);
		return;
	}
	const model = parsed.model;
	renderSidebar(parsed);
	updateMeta(model);
	if (parsed.view === 'trends') {
		cleanup = renderTrends(content, model, parsed.phase, parsed.params, navigate) || (() => {});
	} else if (parsed.view === 'run') {
		cleanup = renderRun(content, model, parsed.sha) || (() => {});
	} else if (parsed.view === 'runs') {
		cleanup = renderRunList(content, model) || (() => {});
	} else if (parsed.view === 'compare') {
		cleanup = renderCompare(content, model, parsed.base, parsed.target, navigate) || (() => {});
	}
	window.scrollTo(0, 0);
}

async function main() {
	if (window.BENCHMARK_DATA) {
		const tracing = buildModel(window.BENCHMARK_DATA, TRACING_SCHEME);
		if (tracing.runs.length > 0) {
			models.push(tracing);
		}
	}
	const nesData = await loadNesData();
	if (nesData) {
		const nes = buildModel(nesData, NES_SCHEME);
		if (nes.runs.length > 0) {
			models.push(nes);
		}
	}
	if (models.length === 0) {
		showEmptyState('No published benchmark runs were found next to this page.');
		return;
	}
	initTheme(route);
	window.addEventListener('hashchange', route);
	route();
}

main();
