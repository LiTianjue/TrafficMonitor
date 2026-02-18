<template>
  <div class="dashboard">
    <!-- Top: Flow Chart + Device Info -->
    <div class="top-section">
      <!-- Left: 24 Hour Flow Chart -->
      <el-card class="flow-card">
        <template #header>
          <span>过去24小时路口总流量变化趋势</span>
        </template>
        <div ref="flowChartRef" class="flow-chart"></div>
      </el-card>

      <!-- Right: Device Info -->
      <div class="device-info">
        <!-- Signal Controller Info -->
        <el-card class="info-card">
          <template #header>
            <span>信号机基础和状态信息</span>
          </template>
          <div class="info-content">
            <el-descriptions :column="1" border size="small">
              <el-descriptions-item label="设备IP">{{ tscConfig.ip || '--' }}</el-descriptions-item>
              <el-descriptions-item label="设备类型">{{ getTscTypeLabel(tscConfig.type) }}</el-descriptions-item>
              <el-descriptions-item label="启用状态">
                <el-tag :type="tscConfig.controlEnable ? 'success' : 'info'" size="small">
                  {{ tscConfig.controlEnable ? '已启用' : '未启用' }}
                </el-tag>
              </el-descriptions-item>
              <el-descriptions-item label="在线状态">
                <el-tag :type="tscStatus.online ? 'success' : 'danger'" size="small">
                  {{ tscStatus.online ? '在线' : '离线' }}
                </el-tag>
              </el-descriptions-item>
              <el-descriptions-item label="版本信息">{{ tscStatus.version || '--' }}</el-descriptions-item>
              <el-descriptions-item label="设备描述">{{ tscStatus.desc || '--' }}</el-descriptions-item>
              <el-descriptions-item label="故障状态">
                <el-tag :type="tscStatus.hasError ? 'danger' : 'success'" size="small">
                  {{ tscStatus.hasError ? '有故障' : '正常' }}
                </el-tag>
              </el-descriptions-item>
            </el-descriptions>
          </div>
        </el-card>

        <!-- Detector Info -->
        <el-card class="info-card">
          <template #header>
            <span>检测器基础和状态信息</span>
          </template>
          <div class="detector-list">
            <div v-if="detectorLoading" class="loading">加载中...</div>
            <div v-else-if="mergedDetectors.length === 0" class="empty">暂无检测器数据</div>
            <div v-else class="detector-grid">
              <div 
                v-for="det in mergedDetectors" 
                :key="det.ip"
                class="detector-item"
                :class="{ 'detector-error': det.hasFault }"
              >
                <div class="detector-ip">{{ det.displayName }}</div>
                <div class="detector-status">
                  <el-tag :type="det.online ? 'success' : 'danger'" size="small">
                    {{ det.online ? '在线' : '离线' }}
                  </el-tag>
                  <el-tag :type="det.hasFault ? 'danger' : 'success'" size="small">
                    {{ det.hasFault ? '故障' : '正常' }}
                  </el-tag>
                </div>
              </div>
            </div>
          </div>
        </el-card>
      </div>
    </div>

    <!-- Bottom: EventLog -->
    <el-card class="event-card">
      <template #header>
        <span>EventLog 看板信息</span>
      </template>
      <el-table :data="eventLogs" stripe size="small" max-height="300">
        <el-table-column prop="time" label="时间" width="180">
          <template #default="{ row }">
            {{ formatTime(row.time) }}
          </template>
        </el-table-column>
        <el-table-column prop="event" label="事件" width="150" />
        <el-table-column prop="detail" label="详情" />
      </el-table>
    </el-card>
  </div>
</template>

<script setup>
import { ref, onMounted, onUnmounted, nextTick } from 'vue'
import axios from 'axios'
import * as echarts from 'echarts'

const flowChartRef = ref(null)
let flowChart = null

const tscConfig = ref({})
const tscStatus = ref({ online: false, hasError: false, version: '', desc: '' })
const detectorTable = ref([])
const detectorStatus = ref([])
const mergedDetectors = ref([])
const detectorLoading = ref(true)
const eventLogs = ref([])

let statusTimer = null
let eventTimer = null

const fetchTscConfig = async () => {
  try {
    const res = await axios.get('/TscConfig')
    let data = res.data
    if (typeof data === 'string') {
      try { data = JSON.parse(data) } catch(e) {}
    }
    tscConfig.value = data || {}
  } catch (e) {
    console.error('Fetch TscConfig failed', e)
  }
}

const fetchTscStatus = async () => {
  try {
    const res = await axios.get('/TscOnlineStatus')
    let data = res.data
    if (typeof data === 'string') {
      try { data = JSON.parse(data) } catch(e) {}
    }
    tscStatus.value = data || { online: false, hasError: false, version: '', desc: '' }
  } catch (e) {
    console.error('Fetch TscStatus failed', e)
  }
}

const fetchDetectors = async () => {
  detectorLoading.value = true
  try {
    const [tableRes, statusRes] = await Promise.all([
      axios.get('/DetectorTable'),
      axios.get('/DetectorStatusTable')
    ])

    let tableData = tableRes.data
    let statusData = statusRes.data
    if (typeof tableData === 'string') {
      try { tableData = JSON.parse(tableData) } catch(e) {}
    }
    if (typeof statusData === 'string') {
      try { statusData = JSON.parse(statusData) } catch(e) {}
    }

    detectorTable.value = tableData || []
    detectorStatus.value = statusData || []

    mergeDetectors()
  } catch (e) {
    console.error('Fetch Detectors failed', e)
  } finally {
    detectorLoading.value = false
  }
}

const mergeDetectors = () => {
  const statusMap = new Map()
  detectorStatus.value.forEach(s => {
    statusMap.set(s.ip, s)
  })

  mergedDetectors.value = detectorTable.value.map(d => {
    const s = statusMap.get(d.ip)
    const online = s ? s.online : false
    const hasFault = s ? s.hasFault : true
    const displayName = d.desc ? `${d.desc}-${d.ip}` : d.ip
    return { ...d, online, hasFault, displayName }
  })
}

const fetchEventLogs = async () => {
  try {
    const res = await axios.get('/MonitorKanban')
    let data = res.data
    if (typeof data === 'string') {
      try { data = JSON.parse(data) } catch(e) {}
    }
    eventLogs.value = data || []
  } catch (e) {
    console.error('Fetch EventLogs failed', e)
  }
}

const fetchFlowData = async () => {
  try {
    const endTime = Math.floor(Date.now() / 1000)
    const beginTime = endTime - 24 * 3600

    const res = await axios.post('/monitor', {
      beginTime,
      endTime,
      queryType: 0x201
    })

    let data = res.data
    if (typeof data === 'string') {
      try { data = JSON.parse(data) } catch(e) {}
    }
    renderFlowChart(data)
  } catch (e) {
    console.error('Fetch FlowData failed', e)
  }
}

const renderFlowChart = (data) => {
  if (!flowChart) {
    flowChart = echarts.init(flowChartRef.value)
  }

  if (!data || !data.tables || data.tables.length === 0) {
    flowChart.clear()
    flowChart.setOption({
      title: { text: '暂无数据', left: 'center', top: 'middle' }
    })
    return
  }

  const seriesData = []
  data.tables.forEach(table => {
    const points = table.datas.map(d => [d.timestamp * 1000, d.value])
    seriesData.push({
      name: table.name,
      type: 'line',
      data: points,
      smooth: true,
      showSymbol: false,
      areaStyle: { opacity: 0.3 }
    })
  })

  const option = {
    tooltip: {
      trigger: 'axis',
      formatter: function (params) {
        let res = new Date(params[0].value[0]).toLocaleString() + '<br/>'
        params.forEach(item => {
          res += item.marker + item.seriesName + ': ' + item.value[1] + '<br/>'
        })
        return res
      }
    },
    grid: {
      left: '3%',
      right: '4%',
      bottom: '3%',
      containLabel: true
    },
    xAxis: {
      type: 'time',
      boundaryGap: false
    },
    yAxis: {
      type: 'value',
      name: data.unit || ''
    },
    series: seriesData
  }

  flowChart.setOption(option)
}

const getTscTypeLabel = (type) => {
  const map = { 0: 'Unknown', 1: 'Type 1', 2: 'Type 2', 3: 'Type 3' }
  return map[type] || 'Unknown'
}

const formatTime = (timestamp) => {
  if (!timestamp) return '--'
  return new Date(timestamp * 1000).toLocaleString()
}

const resizeHandler = () => {
  if (flowChart) flowChart.resize()
}

onMounted(async () => {
  await fetchTscConfig()
  await fetchTscStatus()
  await fetchDetectors()
  await fetchEventLogs()
  await fetchFlowData()

  nextTick(() => {
    window.addEventListener('resize', resizeHandler)
  })

  statusTimer = setInterval(fetchTscStatus, 5000)
  eventTimer = setInterval(fetchEventLogs, 10000)
})

onUnmounted(() => {
  window.removeEventListener('resize', resizeHandler)
  if (flowChart) flowChart.dispose()
  if (statusTimer) clearInterval(statusTimer)
  if (eventTimer) clearInterval(eventTimer)
})
</script>

<style scoped>
.dashboard {
  padding: 20px;
  height: calc(100vh - 60px);
  display: flex;
  flex-direction: column;
  gap: 20px;
}

.top-section {
  display: flex;
  gap: 20px;
  flex: 1;
  min-height: 0;
}

.flow-card {
  flex: 2;
  display: flex;
  flex-direction: column;
}

.flow-chart {
  width: 100%;
  height: 350px;
}

.device-info {
  flex: 1;
  display: flex;
  flex-direction: column;
  gap: 20px;
  min-width: 350px;
}

.info-card {
  flex: 1;
  display: flex;
  flex-direction: column;
  overflow: hidden;
}

.info-content {
  overflow-y: auto;
}

.detector-list {
  overflow-y: auto;
  flex: 1;
}

.detector-grid {
  display: grid;
  grid-template-columns: repeat(auto-fill, minmax(80px, 1fr));
  gap: 10px;
}

.detector-item {
  padding: 10px;
  border-radius: 4px;
  background: #f5f7fa;
  text-align: center;
  border: 2px solid #67c23a;
}

.detector-item.detector-error {
  border-color: #f56c6c;
}

.detector-ip {
  font-weight: bold;
  margin-bottom: 5px;
  font-size: 12px;
}

.detector-status {
  display: flex;
  flex-direction: column;
  gap: 2px;
  align-items: center;
}

.detector-status .el-tag {
  margin: 0;
}

.loading, .empty {
  text-align: center;
  padding: 20px;
  color: #909399;
}

.event-card {
  flex: 0 0 auto;
  max-height: 300px;
}
</style>
