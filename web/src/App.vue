<template>
  <div v-if="isLoginPage" class="login-container-full">
    <router-view></router-view>
  </div>
  <el-container v-else style="height: 100vh">
    <el-aside :width="isCollapse ? '64px' : '220px'" class="sidebar-container">
      <div class="logo-container" :class="{ 'collapsed': isCollapse }">
        <span v-if="!isCollapse">交通信号控制器</span>
        <span v-else>信号</span>
      </div>
      
      <el-menu
        :default-active="$route.path"
        class="el-menu-vertical"
        :collapse="isCollapse"
        router
        background-color="#304156"
        text-color="#bfcbd9"
        active-text-color="#409EFF"
        :collapse-transition="false"
      >
        <el-menu-item index="/">
          <el-icon><Odometer /></el-icon>
          <template #title>仪表盘</template>
        </el-menu-item>
        
        <el-menu-item index="/realtime">
          <el-icon><VideoPlay /></el-icon>
          <template #title>实时状态</template>
        </el-menu-item>

        <el-menu-item index="/monitor">
          <el-icon><DataLine /></el-icon>
          <template #title>监控查询</template>
        </el-menu-item>

        <el-sub-menu index="/system">
            <template #title>
                <el-icon><InfoFilled /></el-icon>
                <span>系统信息</span>
            </template>
            
            <el-menu-item index="/operation">
                <el-icon><Cpu /></el-icon>
                <template #title>运维信息查询</template>
            </el-menu-item>

            <el-menu-item index="/traffic-static">
                <el-icon><DataAnalysis /></el-icon>
                <template #title>交通统计数据查询</template>
            </el-menu-item>
        </el-sub-menu>

        <el-sub-menu index="/config">
            <template #title>
                <el-icon><Tools /></el-icon>
                <span>系统配置</span>
            </template>
            
            <el-menu-item index="/tsc">
                <el-icon><Setting /></el-icon>
                <template #title>信号机配置</template>
            </el-menu-item>

            <el-menu-item index="/detectors">
                <el-icon><Aim /></el-icon>
                <template #title>检测器配置</template>
            </el-menu-item>

            <el-menu-item index="/channels">
                <el-icon><Connection /></el-icon>
                <template #title>通道配置</template>
            </el-menu-item>

            <el-menu-item index="/attributes">
                <el-icon><Document /></el-icon>
                <template #title>属性配置</template>
            </el-menu-item>

            <el-menu-item index="/schemes">
                <el-icon><Files /></el-icon>
                <template #title>方案配置</template>
            </el-menu-item>

            <el-menu-item index="/triggers">
                <el-icon><SwitchButton /></el-icon>
                <template #title>触发器配置</template>
            </el-menu-item>

            <el-menu-item index="/lanes">
                <el-icon><Operation /></el-icon>
                <template #title>车道配置</template>
            </el-menu-item>
        </el-sub-menu>
      </el-menu>
    </el-aside>

    <el-container>
      <el-header class="navbar">
        <div class="navbar-left">
            <el-icon class="hamburger" @click="toggleCollapse">
                <component :is="isCollapse ? Expand : Fold" />
            </el-icon>
            <el-breadcrumb separator="/" class="breadcrumb">
                <el-breadcrumb-item :to="{ path: '/' }">首页</el-breadcrumb-item>
                <el-breadcrumb-item>{{ currentRouteName }}</el-breadcrumb-item>
            </el-breadcrumb>
        </div>
        <div class="navbar-right">
            <el-button-group class="config-buttons">
                <el-button size="small" @click="handleExportConfig">
                    <el-icon><Download /></el-icon>
                    导出
                </el-button>
                <el-button size="small" @click="triggerImport">
                    <el-icon><Upload /></el-icon>
                    导入
                </el-button>
                <input 
                    type="file" 
                    ref="importInput" 
                    style="display: none" 
                    accept=".json"
                    @change="handleImportConfig"
                />
            </el-button-group>
            <el-dropdown trigger="click" @command="handleCommand">
                <div class="avatar-wrapper">
                    <span class="username">管理员</span>
                    <el-icon><CaretBottom /></el-icon>
                </div>
                <template #dropdown>
                    <el-dropdown-menu>
                        <el-dropdown-item command="logout">退出登录</el-dropdown-item>
                    </el-dropdown-menu>
                </template>
            </el-dropdown>
        </div>
      </el-header>
      
      <el-main class="app-main">
        <router-view v-slot="{ Component }">
            <transition name="fade-transform" mode="out-in">
                <component :is="Component" />
            </transition>
        </router-view>
      </el-main>
    </el-container>
  </el-container>
</template>

<script setup>
import { ref, computed } from 'vue'
import { useRoute, useRouter } from 'vue-router'
import axios from 'axios'
import { ElMessage, ElLoading } from 'element-plus'
import { 
    Odometer, VideoPlay, DataLine, Tools, Setting, Aim, Connection, 
    Document, Files, SwitchButton, Operation, Expand, Fold, CaretBottom,
    Download, Upload, InfoFilled, Cpu, DataAnalysis
} from '@element-plus/icons-vue'

const route = useRoute()
const router = useRouter()
const isLoginPage = computed(() => route.path === '/login')
const isCollapse = ref(false)
const importInput = ref(null)

const currentRouteName = computed(() => {
    const path = route.path
    if (path === '/') return '仪表盘'
    if (path === '/realtime') return '实时状态'
    if (path === '/monitor') return '监控查询'
    if (path === '/operation') return '运维信息查询'
    if (path === '/traffic-static') return '交通统计数据查询'
    if (path === '/tsc') return '信号机配置'
    if (path === '/detectors') return '检测器配置'
    if (path === '/channels') return '通道配置'
    if (path === '/attributes') return '属性配置'
    if (path === '/schemes') return '方案配置'
    if (path === '/triggers') return '触发器配置'
    if (path === '/lanes') return '车道配置'
    return '系统配置'
})

const toggleCollapse = () => {
    isCollapse.value = !isCollapse.value
}

const handleCommand = (command) => {
    if (command === 'logout') {
        logout()
    }
}

const logout = async () => {
  await axios.post('/api/logout')
  router.push('/login')
}

const configKeys = [
  { key: 'TscConfig', getUrl: '/TscConfig', setUrl: '/TscConfig' },
  { key: 'DetectorTable', getUrl: '/DetectorTable', setUrl: '/DetectorTable' },
  { key: 'ChannelCtrlTable', getUrl: '/ChannelCtrlTable', setUrl: '/ChannelCtrlTable' },
  { key: 'LaneRelateInfoTable', getUrl: '/LaneRelateInfoTable', setUrl: '/LaneRelateInfoTable' },
  { key: 'TrafficAttributeTable', getUrl: '/TrafficAttributeTable', setUrl: '/TrafficAttributeTable' },
  { key: 'PssSchemeTable', getUrl: '/PssSchemeTable', setUrl: '/PssSchemeTable' },
  { key: 'PssTrigerTable', getUrl: '/PssTrigerTable', setUrl: '/PssTrigerTable' }
]

const handleExportConfig = async () => {
  try {
    const exportData = {}
    for (const config of configKeys) {
      const res = await axios.get(config.getUrl)
      let data = res.data
      if (typeof data === 'string') {
        try { data = JSON.parse(data) } catch(e) {}
      }
      exportData[config.key] = data
    }
    const blob = new Blob([JSON.stringify(exportData, null, 2)], { type: 'application/json' })
    const url = URL.createObjectURL(blob)
    const link = document.createElement('a')
    link.href = url
    link.download = `traffic_config_${Date.now()}.json`
    link.click()
    URL.revokeObjectURL(url)
    ElMessage.success('配置导出成功')
  } catch (error) {
    console.error('Export config failed:', error)
    ElMessage.error('配置导出失败')
  }
}

const triggerImport = () => {
  importInput.value?.click()
}

const handleImportConfig = async (event) => {
  const file = event.target.files[0]
  if (!file) return
  
  try {
    const text = await file.text()
    const importData = JSON.parse(text)
    
    const loading = ElLoading.service({
      lock: true,
      text: '正在导入配置...',
      background: 'rgba(0, 0, 0, 0.7)'
    })
    
    let successCount = 0
    let failCount = 0
    
    for (const config of configKeys) {
      if (importData[config.key] !== undefined) {
        try {
          await axios.post(config.setUrl, importData[config.key])
          successCount++
        } catch (e) {
          console.error(`Import ${config.key} failed:`, e)
          failCount++
        }
      }
    }
    
    loading.close()
    
    if (failCount === 0) {
      ElMessage.success(`配置导入成功，共导入 ${successCount} 项`)
    } else {
      ElMessage.warning(`导入完成，成功 ${successCount} 项，失败 ${failCount} 项`)
    }
  } catch (error) {
    console.error('Import config failed:', error)
    ElMessage.error('配置导入失败：文件格式错误')
  }
  
  event.target.value = ''
}
</script>

<style>
body {
  margin: 0;
  font-family: 'Helvetica Neue', Helvetica, 'PingFang SC', 'Hiragino Sans GB', 'Microsoft YaHei', Arial, sans-serif;
  background-color: #f0f2f5;
}

.login-container-full {
    height: 100vh;
    width: 100%;
}

.sidebar-container {
    background-color: #304156;
    height: 100%;
    transition: width 0.28s;
    overflow-x: hidden;
}

.logo-container {
    height: 60px;
    line-height: 60px;
    text-align: center;
    color: #fff;
    font-weight: 600;
    font-size: 20px;
    background-color: #2b2f3a;
    overflow: hidden;
    white-space: nowrap;
}

.el-menu-vertical {
    border-right: none;
}

.navbar {
    height: 50px;
    overflow: hidden;
    position: relative;
    background: #fff;
    box-shadow: 0 1px 4px rgba(0,21,41,.08);
    display: flex;
    align-items: center;
    justify-content: space-between;
    padding: 0 20px;
}

.navbar-left {
    display: flex;
    align-items: center;
}

.hamburger {
    display: inline-block;
    cursor: pointer;
    width: 20px;
    height: 20px;
    margin-right: 15px;
    color: #606266;
}

.breadcrumb {
    font-size: 14px;
    line-height: 50px;
}

.navbar-right {
    display: flex;
    align-items: center;
    gap: 15px;
}

.config-buttons .el-button {
    background-color: transparent;
    border-color: transparent;
    color: #606266;
}

.config-buttons .el-button:hover {
    color: #409eff;
}

.avatar-wrapper {
    cursor: pointer;
    display: flex;
    align-items: center;
    color: #606266;
}

.username {
    margin-right: 5px;
    font-weight: 500;
}

.app-main {
    padding: 20px;
    background-color: #f0f2f5;
    min-height: calc(100vh - 50px);
}

/* Transition */
.fade-transform-enter-active,
.fade-transform-leave-active {
  transition: all 0.3s;
}

.fade-transform-enter-from {
  opacity: 0;
  transform: translateX(-30px);
}

.fade-transform-leave-to {
  opacity: 0;
  transform: translateX(30px);
}
</style>
