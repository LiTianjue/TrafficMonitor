import { createApp } from 'vue'
import ElementPlus from 'element-plus'
import 'element-plus/dist/index.css'
import 'element-plus/theme-chalk/el-message.css'
import 'element-plus/theme-chalk/el-loading.css'
import App from './App.vue'
import router from './router'

const app = createApp(App)

app.use(router)
app.use(ElementPlus)
app.mount('#app')
