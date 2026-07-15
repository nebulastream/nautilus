// Entry point: builds the model from window.BENCHMARK_DATA, renders the shell
// (topbar, sidebar) and routes between views via the location hash so every
// state is linkable:
//   #/trends/<phase>?w=<workload>&v=<variants>&log=1
//   #/run/<sha>       #/runs        #/compare/<baseSha>..<targetSha>

import { buildModel } from './model.js';
import { PHASE_ORDER, PHASE_LABELS } from './parser.js';
import { renderTrends } from './views/trends.js';
import { renderRun, renderRunList } from './views/run.js';
import { renderCompare } from './views/compare.js';

const content = document.getElementById('content');
const sidebar = document.getElementById('sidebar');

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
function parseHash() {
	const hash = location.hash.replace(/^#\/?/, '');
	const [path, query] = hash.split('?');
	const params = Object.fromEntries(new URLSearchParams(query || ''));
	const segments = path.split('/').filter(Boolean);
	if (segments[0] === 'run' && segments[1]) {
		return { view: 'run', sha: segments[1] };
	}
	if (segments[0] === 'runs') {
		return { view: 'runs' };
	}
	if (segments[0] === 'compare') {
		const [base, target] = (segments[1] || '').split('..');
		return { view: 'compare', base, target };
	}
	if (segments[0] === 'trends' && segments[1]) {
		return { view: 'trends', phase: segments[1], params };
	}
	return null;
}

let model = null;
let cleanup = () => {};

function renderSidebar(active) {
	sidebar.innerHTML = '';
	const trendsTitle = document.createElement('div');
	trendsTitle.className = 'nav-title';
	trendsTitle.textContent = 'Trends';
	sidebar.appendChild(trendsTitle);
	for (const phase of PHASE_ORDER) {
		if (!model.catalog.has(phase)) {
			continue;
		}
		const a = document.createElement('a');
		a.href = `#/trends/${phase}`;
		a.textContent = PHASE_LABELS[phase] || phase;
		if (active.view === 'trends' && active.phase === phase) {
			a.className = 'active';
		}
		sidebar.appendChild(a);
	}
	const runsTitle = document.createElement('div');
	runsTitle.className = 'nav-title';
	runsTitle.textContent = 'Runs';
	sidebar.appendChild(runsTitle);
	const runsLink = document.createElement('a');
	runsLink.href = '#/runs';
	runsLink.textContent = 'All runs';
	if (active.view === 'runs' || active.view === 'run') {
		runsLink.className = 'active';
	}
	sidebar.appendChild(runsLink);
	const compareLink = document.createElement('a');
	compareLink.href = '#/compare/';
	compareLink.textContent = 'Compare';
	if (active.view === 'compare') {
		compareLink.className = 'active';
	}
	sidebar.appendChild(compareLink);
}

function route() {
	cleanup();
	cleanup = () => {};
	const parsed = parseHash();
	if (!parsed) {
		const defaultPhase = PHASE_ORDER.find((p) => model.catalog.has(p)) || 'trace';
		location.replace(`#/trends/${defaultPhase}`);
		return;
	}
	renderSidebar(parsed);
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

function main() {
	if (!window.BENCHMARK_DATA) {
		showEmptyState('data.js did not load — no published benchmark data found next to this page.');
		return;
	}
	model = buildModel(window.BENCHMARK_DATA);
	if (model.runs.length === 0) {
		showEmptyState('The published data file contains no benchmark runs yet.');
		return;
	}

	document.getElementById('meta-info').textContent =
		`${model.runs.length} runs · last update ${new Date(model.lastUpdate).toISOString().slice(0, 16).replace('T', ' ')} UTC`;
	const repoLink = document.getElementById('repo-link');
	repoLink.href = model.repoUrl || '#';

	initTheme(route);
	window.addEventListener('hashchange', route);
	route();
}

main();
