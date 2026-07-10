import { useEffect, useMemo, useRef, useState } from 'react';
import { parse } from '@nautilus-ir/parser';
import { makeTextDocument } from './lib/vscodeShim';
import { splitTraceSections } from './lib/traceSections';
import { compile, fetchExamples, type Backend, type CompileOptions, type CompileResult, type Example, type Stage } from './api';
import { Editor, type EditorMarker } from './components/Editor';
import { Toolbar, type StatusKind } from './components/Toolbar';
import { StageTabs } from './components/StageTabs';
import { GraphView } from './components/GraphView';
import { DiffEditor } from './components/DiffEditor';
import { DiagnosticsPanel, markersFromClangStderr } from './components/Diagnostics';

const SKELETON = `#include <nautilus/Engine.hpp>
#include <nautilus/val.hpp>

using namespace nautilus;

// Register one or more functions on the module. Lambdas need an explicit
// val<>-typed signature: registerFunction<val<R>(val<Args>...)>(name, lambda).
void playground_register(engine::NautilusModule& m) {
	m.registerFunction<val<int32_t>(val<int32_t>, val<int32_t>)>(
	    "add", [](val<int32_t> a, val<int32_t> b) { return a + b; });
}
`;

const DEFAULT_EXAMPLE_ID = '02_loops';

export function App() {
	const [source, setSource] = useState(SKELETON);
	const [backend, setBackend] = useState<Backend>('mlir');
	const [options, setOptions] = useState<CompileOptions>({});
	const [examples, setExamples] = useState<Example[]>([]);
	const [exampleId, setExampleId] = useState('');
	const [result, setResult] = useState<CompileResult | null>(null);
	const [status, setStatus] = useState('loading…');
	const [statusKind, setStatusKind] = useState<StatusKind>('ready');
	const [busy, setBusy] = useState(false);
	const [activeKey, setActiveKey] = useState<string>('');
	const [introDismissed, setIntroDismissed] = useState(false);
	const userEditedRef = useRef(false);
	const autoCompiledRef = useRef(false);
	const sourceRef = useRef(source);
	sourceRef.current = source;
	const backendRef = useRef(backend);
	backendRef.current = backend;
	const optionsRef = useRef(options);
	optionsRef.current = options;

	const doCompile = async (overrideSource?: string) => {
		const compileSource = overrideSource ?? sourceRef.current;
		setBusy(true);
		setStatus('submitting…');
		setStatusKind('working');
		try {
			const view = await compile(compileSource, backendRef.current, optionsRef.current, (progress) => {
				if (progress.status === 'queued') {
					setStatus(progress.queuePosition ? `queued (#${progress.queuePosition})` : 'queued');
				} else if (progress.status === 'running') {
					setStatus('compiling…');
				}
			});
			if (view.result) {
				setResult(view.result);
				if (view.result.status === 'done') {
					setStatus(`done in ${(view.result.timings.totalMs / 1000).toFixed(1)}s`);
					setStatusKind('done');
				} else {
					setStatus(`failed (${view.result.diagnostics.phase ?? 'unknown'})`);
					setStatusKind('failed');
				}
				setActiveKey((current) => {
					if (current === 'graph' || view.result!.stages.some((s) => s.key === current)) {
						return current;
					}
					return view.result!.stages[0]?.key ?? '';
				});
			} else {
				setStatus(`failed: ${view.error ?? 'unknown error'}`);
				setStatusKind('failed');
			}
		} catch (error) {
			setStatus(`error: ${error instanceof Error ? error.message : String(error)}`);
			setStatusKind('failed');
		} finally {
			setBusy(false);
		}
	};

	// First run: load the default example and compile it automatically so a
	// visitor lands on a live pipeline. The server-side result cache makes
	// this effectively free; the guard keeps user edits untouched.
	useEffect(() => {
		void fetchExamples().then((loaded) => {
			setExamples(loaded);
			setStatus('ready');
			if (!autoCompiledRef.current && !userEditedRef.current) {
				autoCompiledRef.current = true;
				const example = loaded.find((x) => x.id === DEFAULT_EXAMPLE_ID) ?? loaded[0];
				if (example) {
					setSource(example.source);
					setExampleId(example.id);
					void doCompile(example.source);
				}
			}
		});
		// eslint-disable-next-line react-hooks/exhaustive-deps
	}, []);

	// Window-level fallback for Ctrl/Cmd+Enter (the editor handles its own).
	useEffect(() => {
		const handler = (event: KeyboardEvent) => {
			if ((event.ctrlKey || event.metaKey) && event.key === 'Enter' && !busy) {
				event.preventDefault();
				void doCompile();
			}
		};
		window.addEventListener('keydown', handler);
		return () => window.removeEventListener('keydown', handler);
		// eslint-disable-next-line react-hooks/exhaustive-deps
	}, [busy]);

	const markers: EditorMarker[] = useMemo(() => {
		if (result?.diagnostics.phase === 'compile') {
			return markersFromClangStderr(result.diagnostics.compilerStderr);
		}
		return [];
	}, [result]);

	const stages = result?.stages ?? [];
	const resolvedKey = activeKey || stages[0]?.key || '';
	const activeStage = stages.find((s) => s.key === resolvedKey);
	const selectedExample = examples.find((x) => x.id === exampleId);
	const statusDetail = result
		? `queue ${result.timings.queueMs} ms · sandbox ${result.timings.sandboxMs} ms · total ${result.timings.totalMs} ms`
		: undefined;

	return (
		<div className="app">
			<Toolbar
				backend={backend}
				onBackendChange={setBackend}
				options={options}
				onOptionsChange={setOptions}
				examples={examples}
				selectedExampleId={exampleId}
				onExampleSelect={(example) => {
					setSource(example.source);
					setExampleId(example.id);
					userEditedRef.current = false;
					void doCompile(example.source);
				}}
				onCompile={() => void doCompile()}
				status={status}
				statusKind={statusKind}
				statusDetail={statusDetail}
				busy={busy}
			/>
			{selectedExample && (
				<div className="example-bar">
					<b>{selectedExample.title}</b>
					<span>{selectedExample.description}</span>
				</div>
			)}
			<main className="panes">
				<section className="pane editor-pane">
					<Editor
						value={source}
						onChange={(value) => {
							userEditedRef.current = true;
							if (exampleId && value !== selectedExample?.source) {
								setExampleId('');
							}
							setSource(value);
						}}
						language="cpp"
						markers={markers}
						onSubmit={() => void doCompile()}
						wordWrap
					/>
				</section>
				<section className="pane inspector-pane">
					{result ? (
						<>
							{!introDismissed && (
								<div className="intro-strip">
									<span>
										Every stage of the Nautilus tracing-JIT pipeline, live: trace → SSA → IR → optimization
										passes → backend code.{' '}
										<a href="https://github.com/nebulastream/nautilus" target="_blank" rel="noreferrer">
											GitHub
										</a>
										{' · '}
										<a href="https://dl.acm.org/doi/10.1145/3626246.3653373" target="_blank" rel="noreferrer">
											SIGMOD&nbsp;’24 paper
										</a>
									</span>
									<button onClick={() => setIntroDismissed(true)} aria-label="Dismiss">
										✕
									</button>
								</div>
							)}
							{result.diagnostics.phase && <DiagnosticsPanel diagnostics={result.diagnostics} />}
							{stages.length > 0 && (
								<>
									<StageTabs stages={stages} activeKey={resolvedKey} onSelect={setActiveKey} />
									<div className="stage-content">
										{resolvedKey === 'graph' ? (
											<GraphView stages={stages} />
										) : (
											activeStage && <StageBody stage={activeStage} stages={stages} />
										)}
									</div>
								</>
							)}
						</>
					) : (
						<div className="placeholder">
							<h2>Welcome to the Nautilus Playground</h2>
							<p>
								Write a Nautilus module on the left — multiple <code>val&lt;T&gt;</code> functions per module —
								pick a backend, and hit <b>Compile</b> (Ctrl/Cmd+Enter).
							</p>
							<p>
								You will see every stage of the pipeline: the execution trace, the IR after SSA construction
								and after each optimization pass, a control-flow graph, and the code the selected backend
								generates.
							</p>
						</div>
					)}
				</section>
			</main>
		</div>
	);
}

function CopyButton({ text }: { text: string }) {
	const [copied, setCopied] = useState(false);
	return (
		<button
			className="chip-button"
			onClick={() => {
				void navigator.clipboard.writeText(text).then(() => {
					setCopied(true);
					setTimeout(() => setCopied(false), 1500);
				});
			}}
		>
			{copied ? 'Copied ✓' : 'Copy'}
		</button>
	);
}

function StageBody({ stage, stages }: { stage: Stage; stages: Stage[] }) {
	const [showDiff, setShowDiff] = useState(true);
	const [traceFunction, setTraceFunction] = useState('all');

	// Predecessor snapshot for pass stages: nearest earlier IR-bearing stage.
	const previousIr = useMemo(() => {
		if (stage.phase !== 'pass') {
			return null;
		}
		const irStages = stages
			.filter((s) => s.lang === 'nautilus-ir' && s.order < stage.order)
			.sort((a, b) => a.order - b.order);
		return irStages[irStages.length - 1] ?? null;
	}, [stage, stages]);

	// Function names (case preserved) from the first parsable IR stage, used
	// to label the trace sections' uppercase headers.
	const irFunctionNames = useMemo(() => {
		const irStage = stages.find((s) => s.lang === 'nautilus-ir');
		if (!irStage) {
			return [];
		}
		try {
			return parse(makeTextDocument(irStage.text)).functions.map((f) => f.name);
		} catch {
			return [];
		}
	}, [stages]);

	const traceSections = useMemo(
		() => (stage.lang === 'nautilus-trace' ? splitTraceSections(stage.text, irFunctionNames) : []),
		[stage, irFunctionNames],
	);

	const traceText =
		traceFunction === 'all'
			? stage.text
			: (traceSections.find((s) => s.header === traceFunction)?.text ?? stage.text);

	const showDiffView = stage.phase === 'pass' && showDiff && previousIr !== null;

	return (
		<div className="stage-body">
			<div className="stage-toolbar">
				{stage.phase === 'pass' && (
					<>
						<span className="stage-chip" title="Per-pass dumps are written when the pass changed the IR, once per fixed-point iteration">
							after last iteration that changed the IR
						</span>
						{previousIr && (
							<label className="stage-toggle">
								<input type="checkbox" checked={showDiff} onChange={(e) => setShowDiff(e.target.checked)} />
								Show changes vs. {previousIr.title}
							</label>
						)}
					</>
				)}
				{traceSections.length > 1 && (
					<label className="stage-toggle">
						Function{' '}
						<select value={traceFunction} onChange={(e) => setTraceFunction(e.target.value)}>
							<option value="all">All functions</option>
							{traceSections.map((section) => (
								<option key={section.header} value={section.header}>
									{section.name}
								</option>
							))}
						</select>
					</label>
				)}
				{stage.truncated && <span className="stage-chip stage-chip-warn">output truncated at 2 MB</span>}
				<div className="stage-toolbar-spacer" />
				<CopyButton text={stage.text} />
			</div>
			{showDiffView ? (
				<DiffEditor original={previousIr.text} modified={stage.text} language="nautilus-ir" />
			) : (
				<Editor value={stage.lang === 'nautilus-trace' ? traceText : stage.text} language={stage.lang} readOnly />
			)}
		</div>
	);
}
