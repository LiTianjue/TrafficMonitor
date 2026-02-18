<template>
  <div class="trigger-config">
    <!-- Attributes Reference Section -->
    <div class="reference-section">
        <div class="reference-header" @click="showAttributes = !showAttributes">
            <h3><el-icon><InfoFilled /></el-icon> Traffic Attributes Reference</h3>
            <el-icon class="arrow-icon" :class="{ 'is-active': showAttributes }"><ArrowRight /></el-icon>
        </div>
        <el-collapse-transition>
            <div v-show="showAttributes" class="reference-content">
                <el-table :data="attributesData" size="small" border max-height="300" stripe>
                    <el-table-column prop="id" label="ID" width="80" sortable />
                    <el-table-column prop="tag" label="Tag" width="150" sortable />
                    <el-table-column prop="desc" label="Description" />
                </el-table>
            </div>
        </el-collapse-transition>
    </div>

    <div class="header">
      <h2>Trigger Configuration</h2>
      <div>
        <el-button type="success" @click="saveData" :loading="saving">Save Changes</el-button>
        <el-button type="primary" @click="addItem">Add Trigger</el-button>
      </div>
    </div>

    <el-table :data="tableData" style="width: 100%" v-loading="loading" border>
      <el-table-column prop="id" label="ID" width="100">
        <template #default="scope">
           <el-input-number v-model="scope.row.id" :min="1" :max="255" size="small" controls-position="right"></el-input-number>
        </template>
      </el-table-column>
      
      <el-table-column prop="desc" label="Description" width="200">
        <template #default="scope">
           <el-input v-model="scope.row.desc" size="small"></el-input>
        </template>
      </el-table-column>

      <el-table-column prop="prio" label="Priority" width="120">
        <template #header>
          <span>Priority</span>
          <el-tooltip content="0 is highest priority, 255 is lowest" placement="top">
            <el-icon style="margin-left: 4px; vertical-align: middle; cursor: help"><InfoFilled /></el-icon>
          </el-tooltip>
        </template>
        <template #default="scope">
           <el-input-number v-model="scope.row.prio" :min="0" :max="255" size="small" controls-position="right"></el-input-number>
        </template>
      </el-table-column>

      <el-table-column prop="schemeId" label="Scheme" width="250">
        <template #default="scope">
           <el-select v-model="scope.row.schemeId" size="small" placeholder="Select Scheme">
             <el-option
               v-for="scheme in schemeOptions"
               :key="scheme.id"
               :label="`${scheme.id} - ${scheme.desc}`"
               :value="scheme.id"
             />
           </el-select>
        </template>
      </el-table-column>

      <el-table-column prop="expr" label="Expression">
        <template #default="scope">
           <el-input v-model="scope.row.expr" size="small" placeholder="e.g. T1 > 0 AND HOUR > 6"></el-input>
        </template>
      </el-table-column>

      <el-table-column label="Actions" width="100" align="center">
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
import { ElMessage } from 'element-plus'
import { DeleteFilled, InfoFilled, ArrowRight } from '@element-plus/icons-vue'

const endpoint = '/PssTrigerTable'
const tableData = ref([])
const schemeOptions = ref([])
const attributesData = ref([])
const loading = ref(false)
const saving = ref(false)
const showAttributes = ref(false)

const fetchData = async () => {
  loading.value = true
  try {
    // Fetch Triggers
    const resTriggers = await axios.get(endpoint)
    let data = resTriggers.data
    if (typeof data === 'string') {
        try { data = JSON.parse(data) } catch(e){}
    }
    tableData.value = Array.isArray(data) ? data : []

    // Fetch Schemes for Dropdown
    const resSchemes = await axios.get('/PssSchemeTable')
    let schemes = resSchemes.data
    if (typeof schemes === 'string') {
        try { schemes = JSON.parse(schemes) } catch(e){}
    }
    const tscSelfOption = { id: 0, desc: '信号机自控' }
    schemeOptions.value = [tscSelfOption, ...(Array.isArray(schemes) ? schemes : [])]

    // Fetch Attributes for Reference
    const resAttrs = await axios.get('/TrafficAttrbuteTable')
    let attrs = resAttrs.data
    if (typeof attrs === 'string') {
        try { attrs = JSON.parse(attrs) } catch(e){}
    }
    attributesData.value = Array.isArray(attrs) ? attrs : []

  } catch (e) {
    ElMessage.error('Failed to fetch data')
  } finally {
    loading.value = false
  }
}

const addItem = () => {
  let maxId = 0
  tableData.value.forEach(item => { if (item.id > maxId) maxId = item.id })
  
  tableData.value.push({
      id: maxId + 1,
      desc: 'New Trigger',
      prio: 1,
      schemeId: null,
      expr: ''
  })
}

const handleDelete = (index) => {
    tableData.value.splice(index, 1)
}

const saveData = async () => {
  saving.value = true
  
  // Validation: Unique ID
  const idSet = new Set()
  for (let i = 0; i < tableData.value.length; i++) {
      const row = tableData.value[i]
      if (idSet.has(row.id)) {
          ElMessage.error(`Row ${i + 1}: Duplicate Trigger ID "${row.id}"`)
          saving.value = false
          return
      }
      idSet.add(row.id)
  }

  try {
    await axios.post(endpoint, tableData.value)
    ElMessage.success('Saved successfully')
  } catch (e) {
    ElMessage.error('Failed to save')
  } finally {
    saving.value = false
  }
}

onMounted(fetchData)
</script>

<style scoped>
.trigger-config {
    padding: 20px;
}
.header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-bottom: 20px;
}

/* Reference Section - Reuse Style */
.reference-section {
    margin-bottom: 20px;
    border: 1px solid #ebeef5;
    border-radius: 4px;
    overflow: hidden;
}
.reference-header {
    display: flex;
    align-items: center;
    justify-content: space-between;
    padding: 10px 15px;
    background-color: #f5f7fa;
    cursor: pointer;
    user-select: none;
}
.reference-header:hover {
    background-color: #eef1f6;
}
.reference-header h3 {
    margin: 0;
    font-size: 14px;
    color: #606266;
    display: flex;
    align-items: center;
    gap: 8px;
}
.reference-content {
    padding: 15px;
    border-top: 1px solid #ebeef5;
}
.arrow-icon {
    transition: transform 0.3s;
}
.arrow-icon.is-active {
    transform: rotate(90deg);
}
</style>
