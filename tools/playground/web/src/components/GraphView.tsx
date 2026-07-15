import { useEffect, useMemo, useRef, useState } from 'react';
import { ActionIcon, Button, Group, Select, Text } from '@mantine/core';
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
	/** The IR-bearing stage to render; the stage bar drives which one is shown. */
	stage: Stage;
}

/**
 * Renders a control-flow graph for ANY IR-bearing stage by parsing the IR
 * text client-side with the vscode-nautilus-ir parser and synthesizing
 * Mermaid via cfgFor — this covers every per-pass snapshot, not just the
 * single stage the engine's own dump.graph option would export.
 */
export function GraphView({ stage }: GraphViewProps) {
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
		link.download = `${activeFunction ?? 'cfg'}-${stage.key}.svg`;
		link.click();
		URL.revokeObjectURL(url);
	};

	const parsed = useMemo(() => {
		try {
			return parse(makeTextDocument(stage.text));
		} catch {
			return null;
		}
	}, [stage]);

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

	return (
		<div className="graph-view">
			<Group className="graph-controls" gap="md" px="md" py={6} wrap="wrap">
				<Group gap="xs">
					<Text size="xs" c="dimmed">
						Function
					</Text>
					<Select
						size="xs"
						w={150}
						value={activeFunction ?? null}
						data={functionNames.map((name) => ({ value: name, label: name }))}
						onChange={(value) => value && setFunctionName(value)}
						comboboxProps={{ shadow: 'md' }}
						allowDeselect={false}
					/>
				</Group>
				<Group gap={6}>
					<ActionIcon variant="default" size="input-xs" onClick={() => setZoom((z) => Math.max(0.25, z - 0.25))} aria-label="Zoom out">
						−
					</ActionIcon>
					<Text size="xs" w={38} ta="center">
						{Math.round(zoom * 100)}%
					</Text>
					<ActionIcon variant="default" size="input-xs" onClick={() => setZoom((z) => Math.min(4, z + 0.25))} aria-label="Zoom in">
						+
					</ActionIcon>
					<Button size="compact-xs" variant="default" onClick={fitToView}>
						Fit
					</Button>
					<Button size="compact-xs" variant="default" onClick={downloadSvg} disabled={!svg}>
						Download SVG
					</Button>
				</Group>
			</Group>
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
