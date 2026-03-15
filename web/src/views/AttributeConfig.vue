<template>
  <div class="attribute-config">
    <div class="header">
      <h2>属性配置</h2>
      <div>
        <el-button type="success" @click="saveData" :loading="saving">保存修改</el-button>
        <el-button type="primary" @click="addItem">添加</el-button>
      </div>
    </div>

    <el-table :data="tableData" style="width: 100%" v-loading="loading" border>
      <el-table-column prop="id" label="ID" width="60" />

      <el-table-column prop="desc" label="描述" width="120">
        <template #default="scope">
          <el-input v-model="scope.row.desc" size="small" @change="handleChange"></el-input>
        </template>
      </el-table-column>

      <el-table-column prop="tag" label="标签" width="120">
        <template #default="scope">
          <el-input v-model="scope.row.tag" size="small" @change="handleChange"></el-input>
        </template>
      </el-table-column>

      <el-table-column prop="dataSourceIP" label="数据源IP" width="150">
        <template #default="scope">
          <el-select 
            v-model="scope.row.dataSourceIP" 
            size="small" 
            placeholder="选择"
            :class="getSelectClass(scope.row.dataSourceIP, '')"
            @change="handleChange"
          >
            <el-option label="未定义" :value="''" />
            <el-option 
              v-for="ip in detectorIPs" 
              :key="ip" 
              :label="ip" 
              :value="ip" 
            />
          </el-select>
        </template>
      </el-table-column>

      <el-table-column prop="laneId" label="车道号" width="100">
        <template #default="scope">
          <el-select 
            v-model="scope.row.laneId" 
            size="small" 
            placeholder="选择"
            :class="getSelectClass(scope.row.laneId, -1)"
            @change="handleChange"
          >
            <el-option label="未定义" :value="-1" />
            <el-option v-for="i in 8" :key="i" :label="i" :value="i" />
          </el-select>
        </template>
      </el-table-column>

      <el-table-column prop="regionId" label="区域号" width="100">
        <template #default="scope">
          <el-select 
            v-model="scope.row.regionId" 
            size="small" 
            placeholder="选择"
            :class="getSelectClass(scope.row.regionId, -1)"
            @change="handleChange"
          >
            <el-option label="未定义" :value="-1" />
            <el-option v-for="i in 8" :key="i" :label="i" :value="i" />
          </el-select>
        </template>
      </el-table-column>

      <el-table-column prop="laneType" label="车道类型" width="100">
        <template #default="scope">
          <el-select 
            v-model="scope.row.laneType" 
            size="small" 
            placeholder="选择"
            :class="getSelectClass(scope.row.laneType, -1)"
            @change="handleChange"
          >
            <el-option label="未定义" :value="-1" />
            <el-option label="上行" :value="1" />
            <el-option label="下行" :value="2" />
          </el-select>
        </template>
      </el-table-column>

      <el-table-column prop="laneTurn" label="转向" width="120">
        <template #default="scope">
          <el-select 
            v-model="scope.row.laneTurn" 
            size="small" 
            placeholder="选择"
            :class="getSelectClass(scope.row.laneTurn, 0)"
            @change="handleChange"
          >
            <el-option label="未定义" :value="0" />
            <el-option label="左转" :value="1" />
            <el-option label="直行" :value="2" />
            <el-option label="右转" :value="4" />
          </el-select>
        </template>
      </el-table-column>

      <el-table-column prop="signalLightId" label="通道号" width="150">
        <template #default="scope">
          <el-select 
            v-model="scope.row.signalLightId" 
            size="small" 
            placeholder="选择"
            :class="getSelectClass(scope.row.signalLightId, -1)"
            @change="handleChange"
          >
            <el-option label="未定义" :value="-1" />
            <el-option 
              v-for="ch in channelList" 
              :key="ch.id" 
              :label="getChannelLabel(ch)" 
              :value="ch.id" 
            />
          </el-select>
        </template>
      </el-table-column>

      <el-table-column prop="attribute" label="属性" width="160">
        <template #default="scope">
          <el-select 
            v-model="scope.row.attribute" 
            size="small" 
            placeholder="选择"
            @change="handleChange"
          >
            <el-option 
              v-for="attr in attributeOptions" 
              :key="attr.value" 
              :label="attr.label" 
              :value="attr.value" 
            />
          </el-select>
        </template>
      </el-table-column>

      <el-table-column prop="timeWindowSeconds" label="时间窗口" width="100">
        <template #default="scope">
          <el-input-number 
            v-model="scope.row.timeWindowSeconds" 
            :min="1" 
            :max="3600" 
            size="small" 
            controls-position="right"
            @change="handleChange"
          />
        </template>
      </el-table-column>

      <el-table-column prop="returnType" label="返回类型" width="120">
        <template #default="scope">
          <el-select 
            v-model="scope.row.returnType" 
            size="small" 
            placeholder="选择"
            @change="handleChange"
          >
            <el-option 
              v-for="rt in returnTypeOptions" 
              :key="rt.value" 
              :label="rt.label" 
              :value="rt.value" 
            />
          </el-select>
        </template>
      </el-table-column>

      <el-table-column label="标签值" width="120" align="center">
        <template #default="scope">
          <el-button 
            size="small" 
            :type="scope.row.tagStatus === 'success' ? 'success' : scope.row.tagStatus === 'error' ? 'danger' : 'info'"
            :loading="scope.row.loading"
            @click="fetchTagValue(scope.$index, scope.row)"
          >
            {{ scope.row.tagValue !== undefined ? scope.row.tagValue : '获取' }}
          </el-button>
        </template>
      </el-table-column>

      <el-table-column label="操作" width="80" align="center">
        <template #default="scope">
          <el-button size="small" type="danger" circle @click="handleDelete(scope.$index)">
            <el-icon><DeleteFilled /></el-icon>
          </el-button>
        </template>
      </el-table-column>
    </el-table>
  </div>
</template>

<script setup>
import { ref, onMounted, reactive } from 'vue'
import axios from 'axios'
import { ElMessage, ElMessageBox } from 'element-plus'
import { DeleteFilled } from '@element-plus/icons-vue'

const endpoint = '/TrafficAttributeTable'
const tableData = ref([])
const loading = ref(false)
const saving = ref(false)

const detectorIPs = ref([])
const channelList = ref([])

const getChannelLabel = (channel) => {
  const dirLabels = { 1: '东', 2: '南', 3: '西', 4: '北', 5: '其他' }
  const typeLabels = { 0: '未定义', 1: '左转', 2: '直行', 3: '右转', 4: '圆盘' }
  return `${channel.id}-${dirLabels[channel.dir] || '其他'}-${typeLabels[channel.type] || '未定义'}`
}

const attributeOptions = [
  { value: 0, label: '排队长度' },
  { value: 1, label: '排队车辆数' },
  { value: 2, label: '车道总流量' },
  { value: 3, label: '左转流量' },
  { value: 4, label: '直行流量' },
  { value: 5, label: '右转流量' },
  { value: 6, label: '车道车辆数' },
  { value: 7, label: '大车数量' },
  { value: 8, label: '行人数量' },
  { value: 9, label: '非机动车数量' },
  { value: 10, label: '首车距离停止线距离' },
  { value: 11, label: '最大车间距离' },
  { value: 12, label: '停止线30米范围内车辆数' },
  { value: 13, label: '停止线50米范围内车辆数' },
  { value: 14, label: '停止线80米范围内车辆数' },
  { value: 15, label: '信号灯红灯时长' },
  { value: 16, label: '信号灯绿灯时长' },
  { value: 17, label: '信号灯倒计时' }
]

const returnTypeOptions = [
  { value: 0, label: '最大值' },
  { value: 1, label: '最小值' },
  { value: 2, label: '求和' },
  { value: 3, label: '平均值' },
  { value: 4, label: '数据条目数' }
]

const isUndefined = (val) => {
  return val === '' || val === -1
}

const getSelectClass = (field, undefinedVal) => {
  if (field === undefinedVal) {
    return { 'is-undefined': true, 'is-valid': false }
  }
  return { 'is-undefined': false, 'is-valid': true }
}

const fetchDetectorIPs = async () => {
  try {
    const res = await axios.get('/DetectorTable')
    let data = res.data
    if (typeof data === 'string') {
      try { data = JSON.parse(data) } catch(e) {}
    }
    if (Array.isArray(data)) {
      detectorIPs.value = data.map(d => d.desc ? `${d.desc}-${d.ip}` : d.ip)
    }
  } catch (e) {
    console.error('Failed to fetch detectors', e)
  }
}

const fetchChannelIds = async () => {
  try {
    const res = await axios.get('/ChannelCtrlTable')
    let data = res.data
    if (typeof data === 'string') {
      try { data = JSON.parse(data) } catch(e) {}
    }
    if (Array.isArray(data)) {
      channelList.value = data.sort((a, b) => a.id - b.id)
    }
  } catch (e) {
    console.error('Failed to fetch channels', e)
  }
}

const fetchData = async () => {
  loading.value = true
  try {
    const res = await axios.get(endpoint)
    let data = res.data
    if (typeof data === 'string') {
      try { data = JSON.parse(data) } catch(e) {}
    }
    tableData.value = Array.isArray(data) ? data.map(item => ({
      ...item,
      loading: false,
      tagStatus: 'pending',
      tagValue: undefined
    })) : []
  } catch (e) {
    ElMessage.error('加载数据失败')
  } finally {
    loading.value = false
  }
}

const renumberIds = () => {
  tableData.value.forEach((item, index) => {
    item.id = index + 1
  })
}

const addItem = () => {
  const newId = tableData.value.length > 0 
    ? Math.max(...tableData.value.map(t => t.id)) + 1 
    : 1
  
  tableData.value.push({
    id: newId,
    desc: '',
    tag: '',
    dataSourceIP: '',
    signalLightId: -1,
    laneId: -1,
    regionId: -1,
    laneType: -1,
    laneTurn: 0,
    attribute: 0,
    timeWindowSeconds: 2,
    returnType: 0,
    loading: false,
    tagStatus: 'pending',
    tagValue: undefined
  })
  handleChange()
}

const handleDelete = (index) => {
  tableData.value.splice(index, 1)
  renumberIds()
  handleChange()
}

const handleChange = () => {
  // Mark as modified
}

const validateData = () => {
  const descSet = new Set()
  const tagSet = new Set()
  
  for (let i = 0; i < tableData.value.length; i++) {
    const row = tableData.value[i]
    
    if (row.desc) {
      if (descSet.has(row.desc)) {
        ElMessage.error(`第 ${i + 1} 行: 描述重复 "${row.desc}"`)
        return false
      }
      descSet.add(row.desc)
    }
    
    if (row.tag) {
      if (tagSet.has(row.tag)) {
        ElMessage.error(`第 ${i + 1} 行: 标签重复 "${row.tag}"`)
        return false
      }
      tagSet.add(row.tag)
    }
  }
  return true
}

const saveData = async () => {
  if (!validateData()) {
    return
  }

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

const fetchTagValue = async (index, row) => {
  if (!row.tag) {
    ElMessage.warning('标签为空')
    return
  }

  tableData.value[index].loading = true
  tableData.value[index].tagStatus = 'loading'
  
  try {
    const res = await axios.post('/fetchTagValue', { tag: row.tag })
    let data = res.data
    if (typeof data === 'string') {
      try { data = JSON.parse(data) } catch(e) {}
    }
    
    tableData.value[index].tagValue = data.value
    tableData.value[index].tagStatus = 'success'
  } catch (e) {
    tableData.value[index].tagValue = 'Error'
    tableData.value[index].tagStatus = 'error'
    ElMessage.error('获取标签值失败')
  } finally {
    tableData.value[index].loading = false
  }
}

onMounted(() => {
  fetchDetectorIPs()
  fetchChannelIds()
  fetchData()
})
</script>

<style scoped>
.attribute-config {
  padding: 20px;
}

.header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 20px;
}

:deep(.is-undefined .el-input__wrapper) {
  background-color: #fef0f0;
  box-shadow: 0 0 0 1px #f56c6c inset;
}

:deep(.is-valid .el-input__wrapper) {
  background-color: #f0f9eb;
  box-shadow: 0 0 0 1px #67c23a inset;
}

:deep(.el-table .el-input-number) {
  width: 80px;
}
</style>
