<template>
  <div class="config-table">
    <div class="header">
      <h2>{{ title }}</h2>
      <el-button type="primary" @click="openAddDialog">Add Item</el-button>
    </div>

    <el-table :data="tableData" style="width: 100%" v-loading="loading" border>
      <el-table-column v-for="col in columns" :key="col" :prop="col" :label="col">
          <template #default="scope">
              <span v-if="isObject(scope.row[col])">{{ JSON.stringify(scope.row[col]) }}</span>
              <span v-else>{{ scope.row[col] }}</span>
          </template>
      </el-table-column>
      <el-table-column label="Actions" width="180">
        <template #default="scope">
          <el-button size="small" @click="handleEdit(scope.$index, scope.row)">Edit</el-button>
          <el-button size="small" type="danger" @click="handleDelete(scope.$index)">Delete</el-button>
        </template>
      </el-table-column>
    </el-table>

    <el-dialog v-model="dialogVisible" :title="editingIndex >= 0 ? 'Edit Item' : 'Add Item'">
      <el-form :model="currentItem" label-width="120px">
        <el-form-item v-for="(val, key) in currentItem" :key="key" :label="key">
            <!-- Simple heuristic for inputs -->
            <el-input v-if="typeof val === 'string'" v-model="currentItem[key]"></el-input>
            <el-input-number v-else-if="typeof val === 'number'" v-model="currentItem[key]"></el-input-number>
            <el-switch v-else-if="typeof val === 'boolean'" v-model="currentItem[key]"></el-switch>
            <el-input v-else type="textarea" :model-value="JSON.stringify(val)" @input="v => updateJson(key, v)"></el-input>
        </el-form-item>
        
        <div v-if="Object.keys(currentItem).length === 0">
             <el-input type="textarea" v-model="rawJson" rows="10" placeholder="Paste JSON object here"></el-input>
        </div>
      </el-form>
      <template #footer>
        <span class="dialog-footer">
          <el-button @click="dialogVisible = false">Cancel</el-button>
          <el-button type="primary" @click="saveItem">Confirm</el-button>
        </span>
      </template>
    </el-dialog>
  </div>
</template>

<script setup>
import { ref, onMounted, watch } from 'vue'
import axios from 'axios'
import { ElMessage, ElMessageBox } from 'element-plus'

const props = defineProps({
  endpoint: String,
  title: String
})

const tableData = ref([])
const columns = ref([])
const loading = ref(false)
const dialogVisible = ref(false)
const editingIndex = ref(-1)
const currentItem = ref({})
const rawJson = ref('')

const fetchData = async () => {
  loading.value = true
  try {
    const res = await axios.get(props.endpoint)
    let data = res.data
    // Handle Crow returning string instead of JSON object if content-type mismatch
    if (typeof data === 'string') {
        try {
            data = JSON.parse(data)
        } catch(e) { console.error("Parse error", e)}
    }
    
    tableData.value = Array.isArray(data) ? data : []
    updateColumns()
  } catch (e) {
    ElMessage.error('Failed to fetch data')
  } finally {
    loading.value = false
  }
}

const updateColumns = () => {
  if (tableData.value.length > 0) {
    columns.value = Object.keys(tableData.value[0])
  }
}

const isObject = (val) => {
    return val && typeof val === 'object'
}

const updateJson = (key, v) => {
    try {
        currentItem.value[key] = JSON.parse(v)
    } catch(e) {
        // ignore invalid json while typing
    }
}

const openAddDialog = () => {
  editingIndex.value = -1
  rawJson.value = '{}'
  // If we have data, we can clone structure from first item
  if (tableData.value.length > 0) {
      const template = { ...tableData.value[0] }
      // Reset values
      for(let k in template) {
          if (typeof template[k] === 'number') template[k] = 0
          else if (typeof template[k] === 'string') template[k] = ''
          else if (typeof template[k] === 'boolean') template[k] = false
          else template[k] = {} // or []
      }
      currentItem.value = template
  } else {
      currentItem.value = {}
  }
  dialogVisible.value = true
}

const handleEdit = (index, row) => {
  editingIndex.value = index
  currentItem.value = JSON.parse(JSON.stringify(row)) // Deep copy
  dialogVisible.value = true
}

const handleDelete = async (index) => {
    try {
        await ElMessageBox.confirm('Are you sure?', 'Warning', { type: 'warning' })
        tableData.value.splice(index, 1)
        await saveData()
    } catch(e) {}
}

const saveItem = async () => {
  if (Object.keys(currentItem.value).length === 0 && rawJson.value) {
      try {
          currentItem.value = JSON.parse(rawJson.value)
      } catch(e) {
          ElMessage.error('Invalid JSON')
          return
      }
  }

  if (editingIndex.value >= 0) {
    tableData.value[editingIndex.value] = currentItem.value
  } else {
    tableData.value.push(currentItem.value)
  }
  
  await saveData()
  dialogVisible.value = false
}

const saveData = async () => {
  loading.value = true
  try {
    // Send the whole array
    await axios.post(props.endpoint, tableData.value)
    ElMessage.success('Saved successfully')
    updateColumns() // In case keys changed
  } catch (e) {
    ElMessage.error('Failed to save')
    // Reload to be safe
    fetchData()
  } finally {
    loading.value = false
  }
}

watch(() => props.endpoint, fetchData)
onMounted(fetchData)
</script>

<style scoped>
.config-table {
    padding: 20px;
}
.header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-bottom: 20px;
}
</style>
