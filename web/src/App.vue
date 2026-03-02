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

            <el-menu-item index="/manual">
                <el-icon><Operation /></el-icon>
                <template #title>手动控制</template>
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
import { 
    Odometer, VideoPlay, DataLine, Tools, Setting, Aim, Connection, 
    Document, Files, SwitchButton, Operation, Expand, Fold, CaretBottom 
} from '@element-plus/icons-vue'

const route = useRoute()
const router = useRouter()
const isLoginPage = computed(() => route.path === '/login')
const isCollapse = ref(false)

const currentRouteName = computed(() => {
    const path = route.path
    if (path === '/') return '仪表盘'
    if (path === '/realtime') return '实时状态'
    if (path === '/monitor') return '监控查询'
    if (path === '/tsc') return '信号机配置'
    if (path === '/detectors') return '检测器配置'
    if (path === '/channels') return '通道配置'
    if (path === '/attributes') return '属性配置'
    if (path === '/schemes') return '方案配置'
    if (path === '/triggers') return '触发器配置'
    if (path === '/manual') return '手动控制'
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
