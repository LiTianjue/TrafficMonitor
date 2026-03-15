<template>
  <div class="lane-config">
    <div class="header">
      <h2>车道配置</h2>
      <div>
        <el-button type="success" @click="saveData" :loading="saving">保存修改</el-button>
        <el-button type="info" @click="fetchAndSyncFromTSC" :loading="syncing">从TSC同步</el-button>
        <el-button type="primary" @click="addItem">添加车道</el-button>
      </div>
    </div>

    <el-table :data="tableData" style="width: 100%" v-loading="loading" border>
      <el-table-column type="index" label="序号" width="80" align="center" />
      
      <el-table-column prop="ip" label="检测器" width="200">
        <template #default="scope">
           <el-select v-model="scope.row.ip" size="small" placeholder="选择检测器" filterable>
             <el-option
               v-for="item in detectorOptions"
               :key="item.ip"
               :label="item.desc + '(' + item.ip + ')'"
               :value="item.ip"
             />
           </el-select>
        </template>
      </el-table-column>
      
      <el-table-column prop="laneNo" label="逻辑车道号" width="120">
        <template #default="scope">
           <el-input-number v-model="scope.row.laneNo" :min="1" :max="8" size="small" controls-position="right"></el-input-number>
        </template>
      </el-table-column>
      
      <el-table-column prop="turn" label="转向" width="180">
        <template #default="scope">
           <el-select v-model="scope.row.turn" size="small" placeholder="选择">
             <el-option
               v-for="item in turnOptions"
               :key="item.value"
               :label="item.label"
               :value="item.value"
             />
           </el-select>
        </template>
      </el-table-column>

      <el-table-column prop="channel" label="关联通道" width="180">
        <template #default="scope">
           <el-select v-model="scope.row.channel" size="small" placeholder="选择通道">
             <el-option
               v-for="item in channelOptions"
               :key="item.id"
               :label="getChannelLabel(item)"
               :value="item.id"
             />
           </el-select>
        </template>
      </el-table-column>

      <el-table-column label="操作" width="100" align="center">
        <template #default="scope">
          <el-button size="small" type="danger" :icon="DeleteFilled" circle @click="handleDelete(scope.$index)"></el-button>
        </template>
      </el-table-column>
    </el-table>
  </div>
</template>

<script setup>
import { ref, onMounted } from 'vue'
import axios from 'axios'
import { ElMessage, ElMessageBox } from 'element-plus'
import { DeleteFilled } from '@element-plus/icons-vue'

const endpoint = '/LaneRelateInfoTable'
const tableData = ref([])
const loading = ref(false)
const saving = ref(false)
const syncing = ref(false)
const tscConfig = ref({})
const detectorTable = ref([])
const channelTable = ref([])

const turnOptions = [
  { value: 0x00, label: '未知' },
  { value: 0x01, label: '左转' },
  { value: 0x02, label: '直行' },
  { value: 0x04, label: '右转' },
  { value: 0x03, label: '左直' },
  { value: 0x06, label: '右直' },
  { value: 0x07, label: '左直右' }
]

const directionLabels = { 1: '东', 2: '南', 3: '西', 4: '北', 5: '其他' }
const typeLabels = { 0: '未定义', 1: '左转', 2: '直行', 3: '右转', 4: '圆盘' }

const detectorOptions = ref([])

const channelOptions = ref([])

const getChannelLabel = (channel) => {
  const dir = directionLabels[channel.dir] || '其他'
  const type = typeLabels[channel.type] || '未定义'
  return `${channel.id}-${dir}-${type}`
}

const fetchDetectorTable = async () => {
  try {
    const res = await axios.get('/DetectorTable')
    let data = res.data
    if (typeof data === 'string') {
      try { data = JSON.parse(data) } catch(e){}
    }
    detectorTable.value = Array.isArray(data) ? data : []
    detectorOptions.value = detectorTable.value.map(item => ({
      ip: item.ip,
      desc: item.desc || '检测器'
    }))
  } catch (e) {
    console.error('Failed to fetch DetectorTable', e)
  }
}

const fetchChannelTable = async () => {
  try {
    const res = await axios.get('/ChannelCtrlTable')
    let data = res.data
    if (typeof data === 'string') {
      try { data = JSON.parse(data) } catch(e){}
    }
    channelTable.value = Array.isArray(data) ? data : []
    channelOptions.value = channelTable.value
  } catch (e) {
    console.error('Failed to fetch ChannelCtrlTable', e)
  }
}

const fetchTscConfig = async () => {
  try {
    const res = await axios.get('/TscConfig')
    let data = res.data
    if (typeof data === 'string') {
      try { data = JSON.parse(data) } catch(e){}
    }
    tscConfig.value = data || {}
  } catch (e) {
    console.error('Failed to fetch TscConfig', e)
  }
}

const fetchData = async () => {
  loading.value = true
  try {
    const res = await axios.get(endpoint)
    let data = res.data
    if (typeof data === 'string') {
      try { data = JSON.parse(data) } catch(e){}
    }
    tableData.value = Array.isArray(data) ? data : []
  } catch (e) {
    ElMessage.error('加载数据失败')
  } finally {
    loading.value = false
  }
}

const addItem = () => {
  tableData.value.push({
    ip: '',
    laneNo: 1,
    turn: 0x01,
    channel: 0
  })
}

const handleDelete = (index) => {
  tableData.value.splice(index, 1)
}

const saveData = async () => {
  saving.value = true
  try {
    await axios.post(endpoint, tableData.value)
    ElMessage.success('保存成功')
  } catch (e) {
    ElMessage.error('保存失败')
  } finally {
    saving.value = false
  }
}

const fetchAndSyncFromTSC = async () => {
  if (tscConfig.value.type !== 0) {
    ElMessage.warning('从TSC同步仅适用于海康威视 (Type 0)')
    return
  }

  syncing.value = true
  try {
    const res = await axios.get('/TSC/LaneRelateInfoTable')
    let data = res.data
    if (typeof data === 'string') {
      try { data = JSON.parse(data) } catch(e){}
    }
    
    await ElMessageBox.confirm(
      '这将用TSC的数据覆盖您当前的车道配置。是否继续?',
      '确认同步',
      {
        confirmButtonText: '确定',
        cancelButtonText: '取消',
        type: 'warning',
      }
    )

    if (Array.isArray(data) && data.length > 0) {
      const syncedData = data.map((item, index) => ({
        ip: item.ip || '',
        laneNo: item.laneNo || (index + 1),
        turn: item.turn !== undefined ? item.turn : 0x01,
        channel: item.channel !== undefined ? item.channel : 0
      }))
      tableData.value = syncedData
    }
    ElMessage.success('同步成功')
  } catch (e) {
    if (e !== 'cancel') {
      ElMessage.error('从TSC同步失败')
    }
  } finally {
    syncing.value = false
  }
}

onMounted(() => {
  fetchTscConfig()
  fetchDetectorTable()
  fetchChannelTable()
  fetchData()
})
</script>

<style scoped>
.lane-config {
    padding: 20px;
}
.header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-bottom: 20px;
}
</style>
