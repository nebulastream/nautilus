// Compare view: delta tables of an arbitrary target run against an arbitrary
// baseline run, reusing the run-detail table renderer.

import { renderDeltaTables, DELTA_THRESHOLD_PCT } from './run.js';
import { shortHash, firstLine, formatDate } from '../format.js';

function runOption(run) {
	return `${shortHash(run.commit.id)} · ${formatDate(run.commit.timestamp)} · ${firstLine(run.commit.message).slice(0, 60)}`;
}

export function renderCompare(container, model, baseSha, targetSha, navigate) {
	container.innerHTML = '';
	const title = document.createElement('h2');
	title.textContent = 'Compare runs';
	container.appendChild(title);

	if (model.runs.length < 2) {
		const p = document.createElement('p');
		p.className = 'subtitle';
		p.textContent = 'Need at least two recorded runs to compare.';
		container.appendChild(p);
		return () => {};
	}

	let baseIdx = baseSha ? model.findRunIndex(baseSha) : -1;
	let targetIdx = targetSha ? model.findRunIndex(targetSha) : -1;
	if (baseIdx === -1) {
		baseIdx = model.runs.length - 2;
	}
	if (targetIdx === -1) {
		targetIdx = model.runs.length - 1;
	}
	const baseRun = model.runs[baseIdx];
	const targetRun = model.runs[targetIdx];

	const toolbar = document.createElement('div');
	toolbar.className = 'toolbar';
	const mkSelect = (selectedIdx, onChange) => {
		const select = document.createElement('select');
		model.runs.forEach((run, i) => {
			const option = document.createElement('option');
			option.value = String(i);
			option.textContent = runOption(run);
			option.selected = i === selectedIdx;
			select.appendChild(option);
		});
		select.onchange = () => onChange(Number(select.value));
		return select;
	};
	const label = (text) => {
		const span = document.createElement('span');
		span.className = 'label';
		span.textContent = text;
		return span;
	};
	toolbar.appendChild(label('Baseline:'));
	toolbar.appendChild(
		mkSelect(baseIdx, (i) => navigate(`#/compare/${model.runs[i].commit.id}..${targetRun.commit.id}`)),
	);
	toolbar.appendChild(label('Target:'));
	toolbar.appendChild(
		mkSelect(targetIdx, (i) => navigate(`#/compare/${baseRun.commit.id}..${model.runs[i].commit.id}`)),
	);
	container.appendChild(toolbar);

	const note = document.createElement('p');
	note.className = 'section-note';
	note.style.margin = '0 0 16px';
	note.textContent =
		`Values are the target run (${shortHash(targetRun.commit.id)}); ` +
		`deltas vs baseline ${shortHash(baseRun.commit.id)}; green/red beyond ±${DELTA_THRESHOLD_PCT}%.`;
	container.appendChild(note);

	renderDeltaTables(container, model, targetRun, baseRun, shortHash(baseRun.commit.id));
	return () => {};
}
