// Webview-side controller for the Nautilus IR graph view.
//
// Receives `render` / `highlight` messages from the extension host, drives
// Mermaid to produce the SVG, attaches svg-pan-zoom for navigation, and
// forwards user actions (block click, function switch) back to the host.
//
// Security posture:
//   - Mermaid runs with `securityLevel: 'strict'`, which sanitises label
//     contents through DOMPurify before producing the SVG.
//   - `htmlLabels` is disabled, so labels are SVG <text>, not HTML.
//   - Mermaid's SVG output is run through DOMPurify a second time in the
//     SVG profile and inserted as a DocumentFragment — never as a string,
//     so there is no `innerHTML` / `DOMParser` sink for tainted content.
//   - Click handlers are wired manually after rendering; we do *not* use
//     Mermaid's `click ... call ...` directive (which would require
//     `securityLevel: 'loose'`).

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
	if (typeof window.DOMPurify === 'undefined') {
		report('error', 'DOMPurify bundle failed to load.');
		return;
	}
	window.mermaid.initialize({
		startOnLoad: false,
		securityLevel: 'strict',
		theme: 'neutral',
		flowchart: { useMaxWidth: false, htmlLabels: false, curve: 'basis' },
	});

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
			const svgEl = sanitizeSvg(result.svg);
			clearChildren(container);
			container.appendChild(svgEl);
			attachClickHandlers(Array.isArray(payload.blocks) ? payload.blocks : []);
			setupPanZoom(svgEl);
			highlight(payload.currentBlock);
		} catch (err) {
			showError(`Mermaid render failed: ${err && err.message ? err.message : String(err)}`);
		}
	}

	// Sanitize Mermaid's SVG output via DOMPurify, returning a live SVG
	// element. `RETURN_DOM_FRAGMENT` skips any string sink: DOMPurify
	// produces the DOM directly from the cleaned tree.
	function sanitizeSvg(svgText) {
		const fragment = window.DOMPurify.sanitize(svgText, {
			USE_PROFILES: { svg: true, svgFilters: true },
			RETURN_DOM_FRAGMENT: true,
		});
		const svgEl = fragment.querySelector('svg');
		if (!svgEl) {
			throw new Error('Mermaid output contained no <svg> element');
		}
		return svgEl;
	}

	function attachClickHandlers(blocks) {
		for (const block of blocks) {
			if (typeof block !== 'string' || !/^Block_\d+$/.test(block)) {
				continue; // defensive: only known-shape block ids are wired
			}
			const node = findBlockNode(block);
			if (node) {
				node.style.cursor = 'pointer';
				node.addEventListener('click', () => {
					vscode.postMessage({ type: 'revealBlock', block });
				});
			}
		}
	}

	function findBlockNode(blockName) {
		const safe = cssEscape(blockName);
		return (
			container.querySelector(`g.node[id^='flowchart-${safe}-']`) ||
			container.querySelector(`g[id^='flowchart-${safe}-']`)
		);
	}

	function populateFunctions(names, active) {
		const current = select.value;
		clearChildren(select);
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

	function setupPanZoom(svgEl) {
		if (panZoom) {
			try { panZoom.destroy(); } catch (e) { /* ignore */ }
			panZoom = null;
		}
		if (!svgEl || typeof window.svgPanZoom === 'undefined') {
			return;
		}
		// Mermaid sometimes sets width/height to a fixed pixel value; force
		// the SVG to fill its host so pan-zoom has something to work with.
		svgEl.removeAttribute('width');
		svgEl.removeAttribute('height');
		svgEl.setAttribute('width', '100%');
		svgEl.setAttribute('height', '100%');
		svgEl.style.maxWidth = 'none';
		svgEl.style.maxHeight = 'none';
		panZoom = window.svgPanZoom(svgEl, {
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
		if (typeof blockName !== 'string' || !/^Block_\d+$/.test(blockName)) {
			return;
		}
		const node = findBlockNode(blockName);
		if (node) {
			node.classList.add('cursor-current');
			highlightedEl = node;
		}
	}

	function setStatus(text) {
		status.textContent = text;
	}

	function showError(message) {
		clearChildren(container);
		const pre = document.createElement('pre');
		pre.className = 'error';
		pre.textContent = message;
		container.appendChild(pre);
		report('error', message);
	}

	function clearChildren(node) {
		while (node.firstChild) {
			node.removeChild(node.firstChild);
		}
	}

	function report(level, message) {
		vscode.postMessage({ type: 'log', level, message });
	}

	function cssEscape(s) {
		if (window.CSS && typeof window.CSS.escape === 'function') {
			return window.CSS.escape(s);
		}
		return s.replace(/[^A-Za-z0-9_-]/g, '\\$&');
	}
})();
