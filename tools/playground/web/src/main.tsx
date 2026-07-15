import React from 'react';
import ReactDOM from 'react-dom/client';
import { createTheme, MantineProvider } from '@mantine/core';
import '@mantine/core/styles.css';
import { App } from './App';
import './styles.css';

// Brand teal scale centered on #3cb4a4 — the same accent the CFG renderer
// (tools/vscode-nautilus-ir/src/cfgFromParsedIr.ts) uses for entry blocks.
const theme = createTheme({
	primaryColor: 'nautilus',
	colors: {
		nautilus: [
			'#e6f7f4',
			'#d0efe9',
			'#a8e0d7',
			'#7dd0c3',
			'#58c2b2',
			'#45bba9',
			'#3cb4a4',
			'#2c9486',
			'#1f7a6e',
			'#0f5f55',
		],
	},
	defaultRadius: 'md',
	fontFamily: "system-ui, -apple-system, 'Segoe UI', sans-serif",
	fontFamilyMonospace: "ui-monospace, 'SF Mono', Menlo, Consolas, monospace",
});

ReactDOM.createRoot(document.getElementById('root')!).render(
	<React.StrictMode>
		<MantineProvider theme={theme} forceColorScheme="light">
			<App />
		</MantineProvider>
	</React.StrictMode>,
);
