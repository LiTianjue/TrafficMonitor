<template>
  <div class="detector-config">
    <div class="header">
      <h2>Detector Configuration</h2>
      <div>
        <el-button type="success" @click="saveData" :loading="saving">Save Changes</el-button>
        <el-button type="info" @click="fetchAndSyncFromTSC" :loading="syncing">Sync from TSC</el-button>
        <el-button type="primary" @click="addItem">Add Detector</el-button>
      </div>
    </div>

    <el-table :data="tableData" style="width: 100%" v-loading="loading" border>
      <el-table-column prop="id" label="ID" width="100">
        <template #default="scope">
           {{ scope.row.id }}
        </template>
      </el-table-column>
      
      <el-table-column prop="desc" label="Description">
        <template #default="scope">
           <el-input v-model="scope.row.desc" size="small"></el-input>
        </template>
      </el-table-column>

      <el-table-column prop="dir" label="Direction" width="120">
        <template #default="scope">
           <el-select v-model="scope.row.dir" size="small" placeholder="Select" @change="handleDirectionChange(scope.row)">
             <el-option
               v-for="item in directionOptions"
               :key="item.value"
               :label="item.label"
               :value="item.value"
             />
           </el-select>
        </template>
      </el-table-column>
      
      <el-table-column prop="ip" label="IP Address" width="180">
        <template #default="scope">
           <el-input v-model="scope.row.ip" size="small"></el-input>
        </template>
      </el-table-column>

      <el-table-column label="Actions" width="100" align="center">
        <template #default="scope">
          <el-button size="small" type="danger" icon="DeleteFilled" circle @click="handleDelete(scope.$index)"></el-button>
        </template>
      </el-table-column>
    </el-table>
  </div>
</template>

<script setup>
import { ref, onMounted, watch } from 'vue'
import axios from 'axios'
import { ElMessage, ElMessageBox } from 'element-plus'
import { DeleteFilled } from '@element-plus/icons-vue'

const endpoint = '/DetectorTable'
const tableData = ref([])
const loading = ref(false)
const saving = ref(false)
const syncing = ref(false)
const tscConfig = ref({}) // To check manufacturer

const directionOptions = [
  { value: 1, label: '东' },
  { value: 2, label: '南' },
  { value: 3, label: '西' },
  { value: 4, label: '北' },
  { value: 5, label: '其他' }
]

const fetchTscConfig = async () => {
    try {
        const res = await axios.get('/TscConfig')
        let data = res.data
        if (typeof data === 'string') {
            try { data = JSON.parse(data) } catch(e){}
        }
        tscConfig.value = data
    } catch(e) {
        console.error('Failed to fetch TSC Config')
    }
}

const fetchAndSyncFromTSC = async () => {
    // Check if TSC is HIKVision before allowing sync
    if (tscConfig.value.type !== 0) {
        ElMessage.warning('Sync from TSC is only available for 海康威视 (Type 0)')
        return
    }

    syncing.value = true
    try {
        const res = await axios.get('/TSC/DetectorTable')
        let data = res.data
        if (typeof data === 'string') {
            try { data = JSON.parse(data) } catch(e){}
        }
        
        // Confirmation dialog
        await ElMessageBox.confirm(
            'This will overwrite your current local detector configurations with the data from the TSC. Continue?',
            'Confirm Sync',
            {
                confirmButtonText: 'OK',
                cancelButtonText: 'Cancel',
                type: 'warning',
            }
        )

        // Apply the fetched data (normalize IDs for consistency)
        tableData.value = normalizeIds(data)
        ElMessage.success('Sync from TSC completed successfully')
    } catch(e) {
        ElMessage.error('Failed to sync from TSC')
    } finally {
        syncing.value = false
    }
}

const isValidIP = (ip) => {
    const regex = /^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/
    return regex.test(ip)
}

const handleDirectionChange = (row) => {
    const dirOption = directionOptions.find(opt => opt.value === row.dir)
    if (!dirOption) return

    const defaultDesc = 'New Detector'
    // If description is empty or equals to default, update it
    if (!row.desc || row.desc === defaultDesc) {
        row.desc = `${dirOption.label}方向检测器`
    }
}

const normalizeIds = (data) => {
    if (!Array.isArray(data)) return []
    const sortedData = data.sort((a, b) => a.id - b.id)
    const normalizedData = sortedData.map((item, index) => ({
        ...item,
        id: index + 1
    }))
    return normalizedData
}

const fetchData = async () => {
  loading.value = true
  try {
    const res = await axios.get(endpoint)
    let data = res.data
    if (typeof data === 'string') {
        try { data = JSON.parse(data) } catch(e){}
    }
    tableData.value = Array.isArray(data) ? normalizeIds(data) : []
  } catch (e) {
    ElMessage.error('Failed to fetch data')
  } finally {
    loading.value = false
  }
}

const addItem = () => {
  // Find the next available ID: max existing ID + 1
  let maxId = 0
  tableData.value.forEach(item => {
      // Normalize IDs if they are null or out of sync
      if (item.id && item.id > maxId) {
        maxId = item.id
      }
  })

  const newDetector = {
      id: maxId + 1, // Will be the new max ID
      desc: 'New Detector',
      dir: 1,
      ip: '0.0.0.0'
  }
  tableData.value.push(newDetector)
}

const handleDelete = (index) => {
    tableData.value.splice(index, 1)
}

const saveData = async () => {
  saving.value = true
  
  // Validation: IP format and uniqueness
  const ipSet = new Set()
  for (let i = 0; i < tableData.value.length; i++) {
      const row = tableData.value[i]
      
      // Validate IP format
      if (!isValidIP(row.ip)) {
          ElMessage.error(`Row ${i + 1}: Invalid IP address "${row.ip}"`)
          saving.value = false
          return
      }

      // Validate IP uniqueness
      if (ipSet.has(row.ip)) {
          ElMessage.error(`Row ${i + 1}: Duplicate IP address "${row.ip}"`)
          saving.value = false
          return
      }
      ipSet.add(row.ip)
  }

  // Reset IDs before sending
  const finalData = tableData.value.map((item, index) => ({
      ...item,
      id: index + 1
  }))

  try {
    await axios.post(endpoint, finalData)
    ElMessage.success('Saved successfully')
    // Reload to display the corrected IDs
    await fetchData()
  } catch (e) {
    ElMessage.error('Failed to save')
  } finally {
    saving.value = false
  }
}

onMounted(() => {
    fetchData()
    fetchTscConfig()
})
</script>

<style scoped>
.detector-config {
    padding: 20px;
}
.header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-bottom: 20px;
}
</style>
