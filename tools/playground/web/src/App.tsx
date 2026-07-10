import { useEffect, useMemo, useState } from 'react';
import { compile, fetchExamples, type Backend, type CompileOptions, type CompileResult, type Example } from './api';
import { Editor, type EditorMarker } from './components/Editor';
import { Toolbar } from './components/Toolbar';
import { StageTabs } from './components/StageTabs';
import { GraphView } from './components/GraphView';
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

export function App() {
	const [source, setSource] = useState(SKELETON);
	const [backend, setBackend] = useState<Backend>('mlir');
	const [options, setOptions] = useState<CompileOptions>({});
	const [examples, setExamples] = useState<Example[]>([]);
	const [result, setResult] = useState<CompileResult | null>(null);
	const [status, setStatus] = useState('idle');
	const [busy, setBusy] = useState(false);
	const [activeKey, setActiveKey] = useState<string>('');

	useEffect(() => {
		void fetchExamples().then(setExamples);
	}, []);

	const markers: EditorMarker[] = useMemo(() => {
		if (result?.diagnostics.phase === 'compile') {
			return markersFromClangStderr(result.diagnostics.compilerStderr);
		}
		return [];
	}, [result]);

	const doCompile = async () => {
		setBusy(true);
		setStatus('submitting…');
		try {
			const view = await compile(source, backend, options, (progress) => {
				if (progress.status === 'queued') {
					setStatus(progress.queuePosition ? `queued (#${progress.queuePosition})` : 'queued');
				} else if (progress.status === 'running') {
					setStatus('compiling…');
				}
			});
			if (view.result) {
				setResult(view.result);
				setStatus(
					view.result.status === 'done'
						? `done in ${(view.result.timings.totalMs / 1000).toFixed(1)}s`
						: `failed (${view.result.diagnostics.phase ?? 'unknown'})`,
				);
				// Keep the current tab if it still exists in the new result.
				if (view.result.stages.length > 0 && activeKey !== 'graph' && !view.result.stages.some((s) => s.key === activeKey)) {
					setActiveKey(view.result.stages[0].key);
				}
			} else {
				setStatus(`failed: ${view.error ?? 'unknown error'}`);
			}
		} catch (error) {
			setStatus(`error: ${error instanceof Error ? error.message : String(error)}`);
		} finally {
			setBusy(false);
		}
	};

	const stages = result?.stages ?? [];
	const activeStage = stages.find((s) => s.key === activeKey);

	return (
		<div className="app">
			<Toolbar
				backend={backend}
				onBackendChange={setBackend}
				options={options}
				onOptionsChange={setOptions}
				examples={examples}
				onExampleSelect={(example) => setSource(example.source)}
				onCompile={() => void doCompile()}
				status={status}
				busy={busy}
			/>
			<main className="panes">
				<section className="pane editor-pane">
					<Editor value={source} onChange={setSource} language="cpp" markers={markers} />
				</section>
				<section className="pane inspector-pane">
					{result ? (
						<>
							{result.diagnostics.phase && <DiagnosticsPanel diagnostics={result.diagnostics} />}
							{stages.length > 0 && (
								<>
									<StageTabs stages={stages} activeKey={activeKey || stages[0].key} onSelect={setActiveKey} />
									<div className="stage-content">
										{(activeKey || stages[0].key) === 'graph' ? (
											<GraphView stages={stages} />
										) : (
											<StageBody stage={activeStage ?? stages[0]} />
										)}
									</div>
								</>
							)}
						</>
					) : (
						<div className="placeholder">
							<p>
								Write a Nautilus module on the left, pick a backend, and hit <b>Compile</b>.
							</p>
							<p>
								You will see every stage of the pipeline: the execution trace, the IR after SSA
								construction and after each optimization pass, a control-flow graph, and the code the
								selected backend generates.
							</p>
						</div>
					)}
				</section>
			</main>
		</div>
	);
}

function StageBody({ stage }: { stage: { key: string; lang: string; text: string; truncated: boolean; phase: string } }) {
	return (
		<div className="stage-body">
			{stage.phase === 'pass' && (
				<div className="stage-note">Snapshot after the last fixed-point iteration in which this pass changed the IR.</div>
			)}
			{stage.truncated && <div className="stage-note">Output truncated at 2 MB.</div>}
			<Editor value={stage.text} language={stage.lang} readOnly />
		</div>
	);
}
