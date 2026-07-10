import { BACKENDS, type Backend, type CompileOptions, type Example } from '../api';

export type StatusKind = 'ready' | 'working' | 'done' | 'failed';

interface ToolbarProps {
	backend: Backend;
	onBackendChange: (backend: Backend) => void;
	options: CompileOptions;
	onOptionsChange: (options: CompileOptions) => void;
	examples: Example[];
	selectedExampleId: string;
	onExampleSelect: (example: Example) => void;
	onCompile: () => void;
	status: string;
	statusKind: StatusKind;
	statusDetail?: string;
	busy: boolean;
}

const BACKEND_LABELS: Record<Backend, string> = {
	mlir: 'MLIR (LLVM JIT)',
	cpp: 'C++ source',
	bc: 'Bytecode',
	tbc: 'Tiered bytecode',
	asmjit: 'AsmJit',
};

/** Stylised nautilus-shell spiral, drawn inline so the page stays self-contained. */
function BrandMark() {
	return (
		<svg className="brand-mark" viewBox="0 0 32 32" width="22" height="22" aria-hidden="true">
			<circle cx="16" cy="16" r="15" fill="#3cb4a4" opacity="0.16" />
			<path
				d="M16 27 C9.9 27 5 22.1 5 16 C5 9.9 9.9 5 16 5 C20.4 5 24 8.6 24 13 C24 16.9 20.9 20 17 20 C13.7 20 11 17.3 11 14 C11 11.2 13.2 9 16 9"
				fill="none"
				stroke="#3cb4a4"
				strokeWidth="2.4"
				strokeLinecap="round"
			/>
		</svg>
	);
}

export function Toolbar({
	backend,
	onBackendChange,
	options,
	onOptionsChange,
	examples,
	selectedExampleId,
	onExampleSelect,
	onCompile,
	status,
	statusKind,
	statusDetail,
	busy,
}: ToolbarProps) {
	return (
		<header className="toolbar">
			<span className="brand">
				<BrandMark />
				<span className="brand-name">
					Nautilus <span className="brand-sub">Playground</span>
				</span>
			</span>

			<label className="toolbar-field">
				Example
				<select
					className="example-select"
					value={selectedExampleId}
					onChange={(e) => {
						const example = examples.find((x) => x.id === e.target.value);
						if (example) {
							onExampleSelect(example);
						}
					}}
				>
					{selectedExampleId === '' && <option value="">Custom code</option>}
					{examples.map((example) => (
						<option key={example.id} value={example.id} title={example.description}>
							{example.title}
						</option>
					))}
				</select>
			</label>

			<label className="toolbar-field">
				Backend
				<select value={backend} onChange={(e) => onBackendChange(e.target.value as Backend)}>
					{BACKENDS.map((b) => (
						<option key={b} value={b}>
							{BACKEND_LABELS[b]}
						</option>
					))}
				</select>
			</label>

			<details className="options-popover">
				<summary>Options</summary>
				<div className="options-body">
					<label>
						<input
							type="checkbox"
							checked={options.enableLICM ?? false}
							onChange={(e) => onOptionsChange({ ...options, enableLICM: e.target.checked })}
						/>{' '}
						Loop-invariant code motion
					</label>
					<label>
						<input
							type="checkbox"
							checked={options.enableLocalCSE ?? false}
							onChange={(e) => onOptionsChange({ ...options, enableLocalCSE: e.target.checked })}
						/>{' '}
						Local CSE
					</label>
					<label>
						<input
							type="checkbox"
							checked={options.enableStrengthReduction ?? false}
							onChange={(e) => onOptionsChange({ ...options, enableStrengthReduction: e.target.checked })}
						/>{' '}
						Strength reduction
					</label>
				</div>
			</details>

			<div className="toolbar-spacer" />

			<span className={`status-pill status-${statusKind}`} title={statusDetail}>
				{status}
			</span>
			<button className="compile-button" onClick={onCompile} disabled={busy} title="Ctrl/Cmd+Enter">
				{busy ? 'Compiling…' : 'Compile'}
			</button>
		</header>
	);
}
