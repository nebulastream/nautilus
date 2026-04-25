// Webview-side controller for the Nautilus IR graph view.
//
// Receives `render` / `highlight` messages from the extension host, drives
// Mermaid to produce the SVG, attaches svg-pan-zoom for navigation, and
// forwards user actions (block click, function switch) back to the host.

(function () {
	'use strict';

	const vscode = acquireVsCodeApi();
	const container = document.getElementById('graph-container');
	const select = /** @type {HTMLSelectElement} */ (document.getElementById('function-select'));
	const fitBtn = document.getElementById('fit-btn');
	const resetBtn = document.getElementById('reset-btn');
	const status = document.getElementById('status');

	let panZoom = null;
	let highlightedEl = null;
	let renderToken = 0;

	if (typeof window.mermaid === 'undefined') {
		report('error', 'Mermaid bundle failed to load.');
		return;
	}
	window.mermaid.initialize({
		startOnLoad: false,
		securityLevel: 'loose',
		theme: 'neutral',
		flowchart: { useMaxWidth: false, htmlLabels: true, curve: 'basis' },
	});

	// Click bridge invoked from Mermaid `click NodeId call onNautilusBlockClick(...)`.
	window.onNautilusBlockClick = function (block) {
		vscode.postMessage({ type: 'revealBlock', block: String(block) });
	};

	select.addEventListener('change', () => {
		vscode.postMessage({ type: 'selectFunction', name: select.value });
	});
	fitBtn.addEventListener('click', () => {
		if (panZoom) {
			panZoom.resize();
			panZoom.fit();
			panZoom.center();
		}
	});
	resetBtn.addEventListener('click', () => {
		if (panZoom) {
			panZoom.resetZoom();
			panZoom.center();
		}
	});

	window.addEventListener('message', event => {
		const msg = event.data;
		if (!msg || typeof msg !== 'object') {
			return;
		}
		if (msg.type === 'render') {
			void render(msg);
		} else if (msg.type === 'highlight') {
			highlight(msg.block);
		}
	});

	vscode.postMessage({ type: 'ready' });

	async function render(payload) {
		const myToken = ++renderToken;
		populateFunctions(payload.functions, payload.activeFunction);
		setStatus(`${payload.activeFunction || ''}`);

		try {
			const id = `nautilus-graph-${myToken}`;
			const result = await window.mermaid.render(id, payload.mermaid);
			if (myToken !== renderToken) {
				return; // a newer render superseded this one
			}
			container.innerHTML = result.svg;
			if (typeof result.bindFunctions === 'function') {
				result.bindFunctions(container);
			}
			setupPanZoom();
			highlight(payload.currentBlock);
		} catch (err) {
			container.innerHTML = '';
			const pre = document.createElement('pre');
			pre.className = 'error';
			pre.textContent = `Mermaid render failed: ${err && err.message ? err.message : String(err)}`;
			container.appendChild(pre);
			report('error', `Mermaid render failed: ${err && err.message ? err.message : String(err)}`);
		}
	}

	function populateFunctions(names, active) {
		const current = select.value;
		select.innerHTML = '';
		for (const name of names) {
			const opt = document.createElement('option');
			opt.value = name;
			opt.textContent = name;
			if (name === active) {
				opt.selected = true;
			}
			select.appendChild(opt);
		}
		select.disabled = names.length <= 1;
		if (!names.includes(current) && active) {
			select.value = active;
		}
	}

	function setupPanZoom() {
		if (panZoom) {
			try { panZoom.destroy(); } catch (e) { /* ignore */ }
			panZoom = null;
		}
		const svg = container.querySelector('svg');
		if (!svg || typeof window.svgPanZoom === 'undefined') {
			return;
		}
		// Mermaid sometimes sets width/height to a fixed pixel value; force
		// the SVG to fill its host so pan-zoom has something to work with.
		svg.removeAttribute('width');
		svg.removeAttribute('height');
		svg.setAttribute('width', '100%');
		svg.setAttribute('height', '100%');
		svg.style.maxWidth = 'none';
		svg.style.maxHeight = 'none';
		panZoom = window.svgPanZoom(svg, {
			zoomEnabled: true,
			controlIconsEnabled: false,
			fit: true,
			center: true,
			minZoom: 0.1,
			maxZoom: 20,
		});
	}

	function highlight(blockName) {
		if (highlightedEl) {
			highlightedEl.classList.remove('cursor-current');
			highlightedEl = null;
		}
		if (!blockName) {
			return;
		}
		// Mermaid prefixes flowchart node ids with `flowchart-` and suffixes
		// them with a numeric id. Match by attribute prefix.
		const node = container.querySelector(`g.node[id^='flowchart-${cssEscape(blockName)}-']`)
			?? container.querySelector(`g[id^='flowchart-${cssEscape(blockName)}-']`);
		if (node) {
			node.classList.add('cursor-current');
			highlightedEl = node;
		}
	}

	function setStatus(text) {
		status.textContent = text;
	}

	function report(level, message) {
		vscode.postMessage({ type: 'log', level, message });
	}

	function cssEscape(s) {
		// Block names are `Block_\d+` so this is mostly a guard against
		// any future labels that contain CSS-special characters.
		if (window.CSS && typeof window.CSS.escape === 'function') {
			return window.CSS.escape(s);
		}
		return s.replace(/[^A-Za-z0-9_-]/g, '\\$&');
	}
})();
