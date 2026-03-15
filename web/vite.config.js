import { defineConfig } from 'vite'
import vue from '@vitejs/plugin-vue'

export default defineConfig({
  plugins: [vue()],
  esbuild: {
    drop: ['console', 'debugger'],
  },
  build: {
    sourcemap: false,
    minify: 'esbuild',
    chunkSizeWarningLimit: 1500,
    rollupOptions: {
      output: {
        manualChunks(id) {
          if (id.includes('node_modules')) {
            return 'vendor'
          }
        }
      }
    }
  },
  server: {
    proxy: {
      '/api': 'http://localhost:18080',
      '/TscConfig': 'http://localhost:18080',
      '/DetectorTable': 'http://localhost:18080',
      '/ChannelCtrlTable': 'http://localhost:18080',
      '/LaneRelateInfoTable': 'http://localhost:18080',
      '/TSC/DetectorTable': 'http://localhost:18080',
      '/TSC/ChannelCtrlTable': 'http://localhost:18080',
      '/TSC/LaneRelateInfoTable': 'http://localhost:18080',
      '/TrafficAttributeTable': 'http://localhost:18080',
      '/PssSchemeTable': 'http://localhost:18080',
      '/PssTrigerTable': 'http://localhost:18080',
      '/PssRealTimeStatus': 'http://localhost:18080'
    }
  }
})
