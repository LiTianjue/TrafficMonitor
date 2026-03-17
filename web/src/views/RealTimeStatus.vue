<template>
  <div class="realtime-container">
    <!-- Top: Options Bar -->
    <div class="options-bar">
      <el-checkbox v-model="options.showLaneNo">显示车道号</el-checkbox>
      <el-checkbox v-model="options.showCarNum">显示车辆数</el-checkbox>
      <el-checkbox v-model="options.showStatus">显示车道状态</el-checkbox>
      <el-checkbox v-model="options.reverseLane">车道逆序排列</el-checkbox>
    </div>

    <!-- Middle: Main Content -->
    <div class="main-content">
      <!-- Left: Intersection Area -->
      <div class="intersection-area">
        <!-- North Direction (Top) - rotation: 180 -->
        <div class="direction-north">
          <DirectionLane 
            :dir="4" 
            :lanes="northLanes" 
            :rotation="180"
            :show-lane-no="options.showLaneNo"
            :show-car-num="options.showCarNum"
            :show-status="options.showStatus"
            :reverse-lane="options.reverseLane"
          />
        </div>

        <!-- Center: East + West -->
        <div class="center-row">
          <!-- West Direction (Left) - rotation: 90 -->
          <div class="direction-west">
            <DirectionLane 
              :dir="3" 
              :lanes="westLanes" 
              :rotation="90"
              :show-lane-no="options.showLaneNo"
              :show-car-num="options.showCarNum"
              :show-status="options.showStatus"
              :reverse-lane="options.reverseLane"
            />
          </div>

          <!-- Center Intersection -->
          <div class="intersection-center">
            <div class="center-zone">
              <span class="countdown">12</span>
            </div>
          </div>

          <!-- East Direction (Right) - rotation: 270 -->
          <div class="direction-east">
            <DirectionLane 
              :dir="1" 
              :lanes="eastLanes" 
              :rotation="270"
              :show-lane-no="options.showLaneNo"
              :show-car-num="options.showCarNum"
              :show-status="options.showStatus"
              :reverse-lane="options.reverseLane"
            />
          </div>
        </div>

        <!-- South Direction (Bottom) - rotation: 0 -->
        <div class="direction-south">
          <DirectionLane 
            :dir="2" 
            :lanes="southLanes" 
            :rotation="0"
            :show-lane-no="options.showLaneNo"
            :show-car-num="options.showCarNum"
            :show-status="options.showStatus"
            :reverse-lane="options.reverseLane"
          />
        </div>
      </div>

      <!-- Right: Info Panel -->
      <div class="info-panel">
        <el-card class="info-card">
          <div class="info-section">
            <div class="info-title">系统信息</div>
            <div class="info-item">
              <span class="label">系统时间:</span>
              <span class="value">{{ formatTime(statusData.utc) }}</span>
            </div>
          </div>

          <el-divider />

          <div class="info-section">
            <div class="info-title">规则信息</div>
            <div class="info-item">
              <span class="label">控制类型:</span>
              <span class="value">{{ getCtrlTypeLabel(statusData.rule?.ctrlType) }}</span>
            </div>
            <div class="info-item">
              <span class="label">控制模式:</span>
              <span class="value">{{ getCtrlModeLabel(statusData.rule?.ctrlMode) }}</span>
            </div>
            <div class="info-item">
              <span class="label">规则描述:</span>
              <span class="value">{{ statusData.rule?.desc || 'N/A' }}</span>
            </div>
            <div class="info-item">
              <span class="label">触发编号:</span>
              <span class="value">{{ statusData.rule?.triggerId ?? '--' }}</span>
            </div>
            <div class="info-item">
              <span class="label">方案编号:</span>
              <span class="value">{{ statusData.rule?.ctrlId ?? '--' }}</span>
            </div>
            <div class="info-item">
              <span class="label">启动时间:</span>
              <span class="value">{{ formatTime(statusData.rule?.boottime) }}</span>
            </div>
            <div class="info-item">
              <span class="label">持续时长:</span>
              <span class="value">{{ formatDuration(statusData.rule?.durnation) }}</span>
            </div>
            <div class="info-item highlight">
              <span class="label">刷新时间:</span>
              <span class="value time-left">{{ formatDuration(statusData.rule?.left) }}</span>
            </div>
          </div>
        </el-card>
      </div>
    </div>

    <!-- Bottom: Stages Area -->
    <div class="stages-area">
      <div class="stages-list">
        <StageCard 
          v-for="(stage, index) in statusData.stages" 
          :key="index"
          :stage="stage"
          :channels="statusData.channels"
        />
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref, reactive, onMounted, onUnmounted } from 'vue'
import axios from 'axios'
import DirectionLane from '../components/DirectionLane.vue'
import StageCard from '../components/StageCard.vue'

const statusData = ref({
  utc: 0,
  rule: {},
  lanes: [],
  channels: [],
  stages: []
})

const options = reactive({
  showLaneNo: true,
  showCarNum: true,
  showStatus: true,
  reverseLane: false
})

const northLanes = ref([
  { laneNo: 1, turn: 1, status: 1, carNum: 2 },
  { laneNo: 2, turn: 2, status: 2, carNum: 4 },
  { laneNo: 3, turn: 4, status: 1, carNum: 1 }
])

const southLanes = ref([
  { laneNo: 1, turn: 1, status: 2, carNum: 3 },
  { laneNo: 2, turn: 2, status: 1, carNum: 5 },
  { laneNo: 3, turn: 4, status: 2, carNum: 2 }
])

const eastLanes = ref([
  { laneNo: 1, turn: 1, status: 1, carNum: 1 },
  { laneNo: 2, turn: 2, status: 1, carNum: 3 }
])

const westLanes = ref([
  { laneNo: 1, turn: 1, status: 2, carNum: 2 },
  { laneNo: 2, turn: 2, status: 2, carNum: 4 },
  { laneNo: 3, turn: 4, status: 2, carNum: 1 }
])

let timer = null

const fetchData = async () => {
  try {
    const res = await axios.get('/PssRealTimeStatus')
    let data = res.data
    if (typeof data === 'string') {
      try { data = JSON.parse(data) } catch(e) {}
    }
    statusData.value = data
    
    if (data.lanes) {
      northLanes.value = data.lanes.filter(l => l.dir === 4)
      southLanes.value = data.lanes.filter(l => l.dir === 2)
      eastLanes.value = data.lanes.filter(l => l.dir === 1)
      westLanes.value = data.lanes.filter(l => l.dir === 3)
    }
  } catch (e) {
    console.error("Fetch status failed", e)
  }
}

onMounted(() => {
  fetchData()
  timer = setInterval(fetchData, 1000)
})

onUnmounted(() => {
  if (timer) clearInterval(timer)
})

const formatTime = (utc) => {
  if (!utc) return '--'
  return new Date(utc * 1000).toLocaleString('zh-CN')
}

const getCtrlTypeLabel = (type) => {
  const map = {
    0: '本地时段控制', 1: '本地触发控制', 2: '客户端控制', 3: 'WEB客户端控制', 4: '平台控制'
  }
  return map[type] || '未知'
}

const getCtrlModeLabel = (mode) => {
  const map = { 0: '信号机自控', 1: '智能感应', 2: '智能半感应', 3: '专家模式', 4: '方案模式' }
  return map[mode] || '未知'
}

const formatDuration = (seconds) => {
  if (!seconds || seconds <= 0) return '0秒'
  if (seconds < 60) return `${seconds}秒`
  if (seconds < 3600) {
    const min = Math.floor(seconds / 60)
    const sec = seconds % 60
    return sec > 0 ? `${min}分${sec}秒` : `${min}分`
  }
  const hour = Math.floor(seconds / 3600)
  const min = Math.floor((seconds % 3600) / 60)
  const sec = seconds % 60
  let result = `${hour}小时`
  if (min > 0) result += `${min}分`
  if (sec > 0) result += `${sec}秒`
  return result
}
</script>

<style scoped>
.realtime-container {
  display: flex;
  flex-direction: column;
  height: calc(100vh - 60px);
  padding: 10px;
  gap: 10px;
  background-color: #f0f2f5;
}

.options-bar {
  flex-shrink: 0;
  height: 40px;
  background-color: #fff;
  border-radius: 4px;
  display: flex;
  align-items: center;
  padding: 0 20px;
  gap: 30px;
}

.main-content {
  flex: 1;
  display: flex;
  gap: 10px;
  min-height: 0;
}

.intersection-area {
  flex: 3.5;
  background-color: #1a1a1a;
  border-radius: 8px;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  padding: 20px;
  position: relative;
}

.direction-north {
  margin-bottom: 10px;
}

.center-row {
  display: flex;
  align-items: center;
  gap: 10px;
}

.direction-west,
.direction-east {
  display: flex;
  align-items: center;
}

.direction-south {
  margin-top: 10px;
}

.intersection-center {
  width: 80px;
  height: 80px;
  display: flex;
  align-items: center;
  justify-content: center;
}

.center-zone {
  width: 60px;
  height: 60px;
  background-color: #2d2d2d;
  border: 2px solid #4a4a4a;
  border-radius: 4px;
  display: flex;
  align-items: center;
  justify-content: center;
}

.countdown {
  font-size: 24px;
  font-weight: bold;
  color: #2ecc71;
}

.info-panel {
  flex: 2.3;
}

.info-card {
  height: 100%;
}

.info-card :deep(.el-card__body) {
  height: 100%;
  display: flex;
  flex-direction: column;
}

.info-section {
  margin-bottom: 10px;
}

.info-title {
  font-size: 16px;
  font-weight: bold;
  color: #303133;
  margin-bottom: 12px;
  padding-bottom: 8px;
  border-bottom: 2px solid #409eff;
}

.info-item {
  display: flex;
  justify-content: flex-start;
  margin-bottom: 10px;
  font-size: 14px;
}

.info-item .label {
  color: #606266;
  min-width: 70px;
}

.info-item .value {
  color: #303133;
  font-weight: 500;
}

.info-item.highlight {
  background-color: #f0f9ff;
  padding: 10px;
  border-radius: 4px;
  margin-top: 10px;
}

.time-left {
  font-size: 20px;
  font-weight: bold;
  color: #409eff;
}

.reserved-area {
  flex-shrink: 0;
  height: 60px;
  display: flex;
  gap: 10px;
}

.reserved-left,
.reserved-right {
  flex: 1;
  background-color: #e8e8e8;
  border: 2px dashed #bbb;
  border-radius: 4px;
  display: flex;
  align-items: center;
  justify-content: center;
}

.reserved-label {
  color: #999;
  font-size: 14px;
}

.stages-area {
  flex-shrink: 0;
  height: 135px;
  background-color: #fff;
  border-radius: 4px;
  padding: 10px 20px;
  display: flex;
  flex-direction: column;
}

.stages-list {
  flex: 1;
  display: flex;
  gap: 12px;
  overflow-x: auto;
  align-items: center;
}
</style>
