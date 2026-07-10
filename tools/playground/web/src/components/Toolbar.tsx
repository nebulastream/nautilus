import { Anchor, Badge, Button, Checkbox, Divider, Group, Popover, Select, Text, Tooltip } from '@mantine/core';
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

const STATUS_COLORS: Record<StatusKind, string> = {
	ready: 'gray',
	working: 'yellow',
	done: 'nautilus',
	failed: 'red',
};

/** Stylised nautilus-shell spiral, drawn inline so the page stays self-contained. */
function BrandMark() {
	return (
		<svg viewBox="0 0 32 32" width="24" height="24" aria-hidden="true">
			<circle cx="16" cy="16" r="15" fill="#3cb4a4" opacity="0.14" />
			<path
				d="M16 27 C9.9 27 5 22.1 5 16 C5 9.9 9.9 5 16 5 C20.4 5 24 8.6 24 13 C24 16.9 20.9 20 17 20 C13.7 20 11 17.3 11 14 C11 11.2 13.2 9 16 9"
				fill="none"
				stroke="#2c9486"
				strokeWidth="2.4"
				strokeLinecap="round"
			/>
		</svg>
	);
}

/**
 * Single unified navbar: brand, example + backend pickers, pass options,
 * project links, status, and the compile action all live here — no
 * secondary banners.
 */
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
	const selectedExample = examples.find((x) => x.id === selectedExampleId);

	const exampleSelect = (
		<Select
			className="example-select"
			size="xs"
			w={210}
			value={selectedExampleId || null}
			placeholder="Custom code"
			data={examples.map((x) => ({ value: x.id, label: x.title }))}
			onChange={(id) => {
				const example = examples.find((x) => x.id === id);
				if (example) {
					onExampleSelect(example);
				}
			}}
			comboboxProps={{ shadow: 'md' }}
			allowDeselect={false}
		/>
	);

	return (
		<header className="toolbar">
			<Group gap="lg" wrap="wrap" style={{ width: '100%' }}>
				<Group gap={9}>
					<BrandMark />
					<Text fw={650} size="md">
						Nautilus{' '}
						<Text span c="dimmed" fw={400}>
							Playground
						</Text>
					</Text>
				</Group>

				<Group gap="xs">
					<Text size="xs" c="dimmed">
						Example
					</Text>
					{selectedExample?.description ? (
						<Tooltip label={selectedExample.description} maw={360} multiline withArrow>
							{exampleSelect}
						</Tooltip>
					) : (
						exampleSelect
					)}
				</Group>

				<Group gap="xs">
					<Text size="xs" c="dimmed">
						Backend
					</Text>
					<Select
						size="xs"
						w={150}
						value={backend}
						data={BACKENDS.map((b) => ({ value: b, label: BACKEND_LABELS[b] }))}
						onChange={(value) => value && onBackendChange(value as Backend)}
						comboboxProps={{ shadow: 'md' }}
						allowDeselect={false}
					/>
				</Group>

				<Popover position="bottom-start" shadow="md" width={260}>
					<Popover.Target>
						<Button size="xs" variant="default">
							Passes
						</Button>
					</Popover.Target>
					<Popover.Dropdown>
						<Text size="xs" c="dimmed" mb={8}>
							Opt-in optimization passes
						</Text>
						<Checkbox
							size="xs"
							mb={6}
							label="Loop-invariant code motion"
							checked={options.enableLICM ?? false}
							onChange={(e) => onOptionsChange({ ...options, enableLICM: e.currentTarget.checked })}
						/>
						<Checkbox
							size="xs"
							mb={6}
							label="Local CSE"
							checked={options.enableLocalCSE ?? false}
							onChange={(e) => onOptionsChange({ ...options, enableLocalCSE: e.currentTarget.checked })}
						/>
						<Checkbox
							size="xs"
							label="Strength reduction"
							checked={options.enableStrengthReduction ?? false}
							onChange={(e) => onOptionsChange({ ...options, enableStrengthReduction: e.currentTarget.checked })}
						/>
					</Popover.Dropdown>
				</Popover>

				<div style={{ flex: 1 }} />

				<Group gap="sm">
					<Anchor href="https://github.com/nebulastream/nautilus" target="_blank" rel="noreferrer" size="xs" c="dimmed">
						GitHub
					</Anchor>
					<Anchor
						href="https://dl.acm.org/doi/10.1145/3626246.3653373"
						target="_blank"
						rel="noreferrer"
						size="xs"
						c="dimmed"
					>
						SIGMOD ’24 paper
					</Anchor>
					<Divider orientation="vertical" />
					<Tooltip label={statusDetail} disabled={!statusDetail} withArrow>
						<Badge
							className={`status-pill status-${statusKind}`}
							variant="light"
							color={STATUS_COLORS[statusKind]}
							size="lg"
							radius="xl"
							style={{ textTransform: 'none', fontWeight: 500 }}
						>
							{status}
						</Badge>
					</Tooltip>
					<Tooltip label="Ctrl/Cmd + Enter" withArrow>
						<Button className="compile-button" size="xs" onClick={onCompile} disabled={busy} loading={busy}>
							Compile
						</Button>
					</Tooltip>
				</Group>
			</Group>
		</header>
	);
}
