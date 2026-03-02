<template>
  <div class="channel-config">
    <div class="header">
      <h2>通道配置</h2>
      <div>
        <el-button type="success" @click="saveData" :loading="saving">保存修改</el-button>
        <el-button type="info" @click="fetchAndSyncFromTSC" :loading="syncing">从TSC同步</el-button>
        <el-button type="primary" @click="addItem">添加通道</el-button>
      </div>
    </div>

    <el-table :data="tableData" style="width: 100%" v-loading="loading" border>
      <el-table-column prop="id" label="ID" width="100">
        <template #default="scope">
           <el-input-number v-model="scope.row.id" :min="1" :max="255" size="small" controls-position="right"></el-input-number>
        </template>
      </el-table-column>
      
      <el-table-column prop="dir" label="方向" width="150">
        <template #default="scope">
           <el-select v-model="scope.row.dir" size="small" placeholder="选择">
             <el-option
               v-for="item in directionOptions"
               :key="item.value"
               :label="item.label"
               :value="item.value"
             />
           </el-select>
        </template>
      </el-table-column>

      <el-table-column prop="type" label="类型">
        <template #default="scope">
           <el-select v-model="scope.row.type" size="small" placeholder="选择类型">
             <el-option
               v-for="item in typeOptions"
               :key="item.value"
               :label="item.label"
               :value="item.value"
             >
                <div style="display: flex; align-items: center">
                   <el-icon v-if="item.icon" style="margin-right: 8px"><component :is="item.icon" /></el-icon>
                   <span>{{ item.label }}</span>
                </div>
             </el-option>
             <!-- Custom trigger content for selected value -->
             <template #prefix>
                <el-icon v-if="getTypeIcon(scope.row.type)" style="margin-top: 6px">
                    <component :is="getTypeIcon(scope.row.type)" />
                </el-icon>
             </template>
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
import { SignalLeft, SignalStraight, SignalRight, SignalRound, SignalUnknown } from '../components/TrafficIcons.js'

const endpoint = '/ChannelCtrlTable'
const tableData = ref([])
const loading = ref(false)
const saving = ref(false)
const syncing = ref(false)
const tscConfig = ref({})

const directionOptions = [
  { value: 1, label: '东' },
  { value: 2, label: '南' },
  { value: 3, label: '西' },
  { value: 4, label: '北' },
  { value: 5, label: '其他' }
]

const typeOptions = [
    { value: 0, label: '未定义', icon: SignalUnknown },
    { value: 1, label: '左转箭头灯', icon: SignalLeft },
    { value: 2, label: '直行箭头灯', icon: SignalStraight },
    { value: 3, label: '右转箭头灯', icon: SignalRight },
    { value: 4, label: '圆盘主灯', icon: SignalRound },
]

const getTypeIcon = (val) => {
    const opt = typeOptions.find(o => o.value === val)
    return opt ? opt.icon : null
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
  // Find max ID
  let maxId = 0
  tableData.value.forEach(item => {
      if (item.id > maxId) maxId = item.id
  })
  
  tableData.value.push({
      id: maxId + 1,
      dir: 1,
      type: 0
  })
}

const handleDelete = (index) => {
    tableData.value.splice(index, 1)
}

const saveData = async () => {
  saving.value = true
  
  // Validation: Duplicate ID Check
  const idSet = new Set()
  for (let i = 0; i < tableData.value.length; i++) {
      const row = tableData.value[i]
      if (idSet.has(row.id)) {
          ElMessage.error(`第 ${i + 1} 行: 通道ID重复 "${row.id}"`)
          saving.value = false
          return
      }
      idSet.add(row.id)
  }

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
        const res = await axios.get('/TSC/ChannelCtrlTable')
        let data = res.data
        if (typeof data === 'string') {
            try { data = JSON.parse(data) } catch(e){}
        }
        
        await ElMessageBox.confirm(
            '这将用TSC的数据覆盖您当前的本地通道配置。是否继续?',
            '确认同步',
            {
                confirmButtonText: '确定',
                cancelButtonText: '取消',
                type: 'warning',
            }
        )

        tableData.value = Array.isArray(data) ? data : []
        ElMessage.success('同步成功')
    } catch (e) {
        if (e !== 'cancel') {
            ElMessage.error('从TSC同步失败')
        }
    } finally {
        syncing.value = false
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

onMounted(() => {
    fetchTscConfig()
    fetchData()
})
</script>

<style scoped>
.channel-config {
    padding: 20px;
}
.header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-bottom: 20px;
}
</style>
