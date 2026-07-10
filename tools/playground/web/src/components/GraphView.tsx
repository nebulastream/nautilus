import { useEffect, useMemo, useRef, useState } from 'react';
import mermaid from 'mermaid';
import { parse } from '@nautilus-ir/parser';
import { cfgFor } from '@nautilus-ir/cfgFromParsedIr';
import { makeTextDocument } from '../lib/vscodeShim';
import type { Stage } from '../api';

mermaid.initialize({
	startOnLoad: false,
	securityLevel: 'strict',
	flowchart: { htmlLabels: false },
	theme: 'neutral',
});

let renderCounter = 0;

interface GraphViewProps {
	/** All stages of the current result; the IR-bearing ones are selectable. */
	stages: Stage[];
}

/**
 * Renders a control-flow graph for ANY IR-bearing stage by parsing the IR
 * text client-side with the vscode-nautilus-ir parser and synthesizing
 * Mermaid via cfgFor — this covers every per-pass snapshot, not just the
 * single stage the engine's own dump.graph option would export.
 */
export function GraphView({ stages }: GraphViewProps) {
	const irStages = useMemo(() => stages.filter((s) => s.lang === 'nautilus-ir'), [stages]);
	const [stageKey, setStageKey] = useState<string>('');
	const [functionName, setFunctionName] = useState<string>('');
	const [svg, setSvg] = useState<string>('');
	const [error, setError] = useState<string>('');
	const [zoom, setZoom] = useState(1);
	const canvasRef = useRef<HTMLDivElement>(null);

	const fitToView = () => {
		const canvas = canvasRef.current;
		const rendered = canvas?.querySelector('svg');
		if (!canvas || !rendered) {
			return;
		}
		const graphWidth = rendered.getBoundingClientRect().width / zoom;
		if (graphWidth > 0) {
			setZoom(Math.min(2, Math.max(0.25, (canvas.clientWidth - 40) / graphWidth)));
		}
	};

	const downloadSvg = () => {
		const blob = new Blob([svg], { type: 'image/svg+xml' });
		const url = URL.createObjectURL(blob);
		const link = document.createElement('a');
		link.href = url;
		link.download = `${activeFunction ?? 'cfg'}-${activeStage?.key ?? 'ir'}.svg`;
		link.click();
		URL.revokeObjectURL(url);
	};

	const activeStage = irStages.find((s) => s.key === stageKey) ?? irStages[irStages.length - 1];

	const parsed = useMemo(() => {
		if (!activeStage) {
			return null;
		}
		try {
			return parse(makeTextDocument(activeStage.text));
		} catch {
			return null;
		}
	}, [activeStage]);

	const functionNames = useMemo(() => (parsed ? parsed.functions.map((f) => f.name) : []), [parsed]);
	const activeFunction = functionNames.includes(functionName) ? functionName : functionNames[0];

	useEffect(() => {
		let cancelled = false;
		setError('');
		if (!parsed || !activeFunction) {
			setSvg('');
			return;
		}
		const cfg = cfgFor(parsed, activeFunction);
		if (!cfg) {
			setSvg('');
			return;
		}
		renderCounter += 1;
		mermaid
			.render(`playground-cfg-${renderCounter}`, cfg.mermaid)
			.then(({ svg: rendered }) => {
				if (!cancelled) {
					setSvg(rendered);
				}
			})
			.catch((err: unknown) => {
				if (!cancelled) {
					setSvg('');
					setError(err instanceof Error ? err.message : String(err));
				}
			});
		return () => {
			cancelled = true;
		};
	}, [parsed, activeFunction]);

	if (irStages.length === 0) {
		return <div className="graph-empty">Compile first — the graph is built from the dumped IR stages.</div>;
	}

	return (
		<div className="graph-view">
			<div className="graph-controls">
				<label>
					Stage{' '}
					<select value={activeStage?.key ?? ''} onChange={(e) => setStageKey(e.target.value)}>
						{irStages.map((s) => (
							<option key={s.key} value={s.key}>
								{s.title}
							</option>
						))}
					</select>
				</label>
				<label>
					Function{' '}
					<select value={activeFunction ?? ''} onChange={(e) => setFunctionName(e.target.value)}>
						{functionNames.map((name) => (
							<option key={name} value={name}>
								{name}
							</option>
						))}
					</select>
				</label>
				<span className="graph-zoom">
					<button onClick={() => setZoom((z) => Math.max(0.25, z - 0.25))} aria-label="Zoom out">
						−
					</button>
					<span>{Math.round(zoom * 100)}%</span>
					<button onClick={() => setZoom((z) => Math.min(4, z + 0.25))} aria-label="Zoom in">
						+
					</button>
					<button onClick={fitToView}>Fit</button>
					<button onClick={downloadSvg} disabled={!svg}>
						Download SVG
					</button>
				</span>
			</div>
			{error && <div className="graph-error">Graph rendering failed: {error}</div>}
			<div className="graph-canvas" ref={canvasRef}>
				<div
					className="graph-svg"
					style={{ transform: `scale(${zoom})`, transformOrigin: 'top left' }}
					// Mermaid output with securityLevel:'strict' + htmlLabels:false
					// is DOMPurify-sanitised SVG.
					dangerouslySetInnerHTML={{ __html: svg }}
				/>
			</div>
		</div>
	);
}
