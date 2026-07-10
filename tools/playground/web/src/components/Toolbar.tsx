import { BACKENDS, type Backend, type CompileOptions, type Example } from '../api';

interface ToolbarProps {
	backend: Backend;
	onBackendChange: (backend: Backend) => void;
	options: CompileOptions;
	onOptionsChange: (options: CompileOptions) => void;
	examples: Example[];
	onExampleSelect: (example: Example) => void;
	onCompile: () => void;
	status: string;
	busy: boolean;
}

const BACKEND_LABELS: Record<Backend, string> = {
	mlir: 'MLIR (LLVM JIT)',
	cpp: 'C++ source',
	bc: 'Bytecode',
	tbc: 'Tiered bytecode',
	asmjit: 'AsmJit',
};

export function Toolbar({
	backend,
	onBackendChange,
	options,
	onOptionsChange,
	examples,
	onExampleSelect,
	onCompile,
	status,
	busy,
}: ToolbarProps) {
	return (
		<header className="toolbar">
			<span className="brand">Nautilus Playground</span>

			<select
				className="example-select"
				value=""
				onChange={(e) => {
					const example = examples.find((x) => x.id === e.target.value);
					if (example) {
						onExampleSelect(example);
					}
				}}
			>
				<option value="" disabled>
					Examples…
				</option>
				{examples.map((example) => (
					<option key={example.id} value={example.id} title={example.description}>
						{example.title}
					</option>
				))}
			</select>

			<label className="backend-label">
				Backend{' '}
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

			<button className="compile-button" onClick={onCompile} disabled={busy}>
				{busy ? 'Compiling…' : 'Compile'}
			</button>
			<span className="status-pill">{status}</span>
		</header>
	);
}
