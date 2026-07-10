import { defineConfig } from 'vite';
import react from '@vitejs/plugin-react';
import path from 'node:path';

// The Nautilus IR parser and CFG builder are reused directly from the
// vscode-nautilus-ir extension sources. They import the `vscode` module only
// for Range/Position/TextDocument types, which the shim provides.
const extensionSrc = path.resolve(__dirname, '../../vscode-nautilus-ir/src');

export default defineConfig({
	plugins: [react()],
	resolve: {
		alias: {
			vscode: path.resolve(__dirname, 'src/lib/vscodeShim.ts'),
			'@nautilus-ir': extensionSrc,
		},
	},
	server: {
		fs: {
			// Allow importing the extension sources from outside the web root.
			allow: [path.resolve(__dirname, '../..')],
		},
		proxy: {
			'/api': 'http://localhost:8080',
		},
	},
	build: {
		chunkSizeWarningLimit: 4096,
	},
});
