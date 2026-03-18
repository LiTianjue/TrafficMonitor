<template>
  <div class="scheme-config">
    <!-- Attributes Reference Section -->
    <div class="reference-section">
        <div class="reference-header" @click="showAttributes = !showAttributes">
            <h3><el-icon><InfoFilled /></el-icon> 交通属性参考</h3>
            <el-icon class="arrow-icon" :class="{ 'is-active': showAttributes }"><ArrowRight /></el-icon>
        </div>
        <el-collapse-transition>
            <div v-show="showAttributes" class="reference-content">
                <el-table :data="attributesData" size="small" border max-height="300" stripe>
                    <el-table-column prop="id" label="ID" width="80" sortable />
                    <el-table-column prop="tag" label="标签" width="150" sortable />
                    <el-table-column prop="desc" label="描述" />
                </el-table>
            </div>
        </el-collapse-transition>
    </div>

    <div class="header">
      <h2>方案配置</h2>
      <div>
        <el-button type="success" @click="saveData" :loading="saving">保存修改</el-button>
        <el-button type="primary" @click="addItem">添加方案</el-button>
      </div>
    </div>

    <el-table :data="tableData" style="width: 100%" v-loading="loading" border row-key="id">
      <el-table-column type="expand">
        <template #default="props">
            <div class="nested-table">
            <div class="nested-header">
                <h3>规则(通道参数)</h3>
                <el-button size="small" type="primary" @click="addRule(props.row)">添加规则</el-button>
            </div>
            <el-table :data="props.row.rules" border style="width: 100%" size="small">
              <el-table-column label="通道" width="220">
                  <template #default="scope">
                      <el-select v-model="scope.row.channel" size="small" placeholder="选择通道">
                          <el-option 
                            v-for="ch in channelOptions" 
                            :key="ch.id" 
                            :label="formatChannelLabel(ch)"
                            :value="ch.id"
                          />
                      </el-select>
                  </template>
              </el-table-column>
              <el-table-column label="添加类型" width="140">
                  <template #default="scope">
                      <el-select v-model="scope.row.addition_type" size="small">
                          <el-option label="不使用" :value="0"></el-option>
                          <el-option label="必选(AND)" :value="1"></el-option>
                          <el-option label="可选(OR)" :value="2"></el-option>
                          <el-option label="替换" :value="3"></el-option>
                      </el-select>
                  </template>
              </el-table-column>
              <el-table-column label="Addition Expr">
                  <template #default="scope">
                      <el-input v-model="scope.row.addition_expr" size="small"></el-input>
                  </template>
              </el-table-column>
              <el-table-column label="请求类型" width="140">
                  <template #default="scope">
                      <el-select v-model="scope.row.request_type" size="small">
                          <el-option label="不使用" :value="0"></el-option>
                          <el-option label="必选(AND)" :value="1"></el-option>
                          <el-option label="可选(OR)" :value="2"></el-option>
                          <el-option label="替换" :value="3"></el-option>
                      </el-select>
                  </template>
              </el-table-column>
              <el-table-column label="Request Expr">
                  <template #default="scope">
                      <el-input v-model="scope.row.request_expr" size="small"></el-input>
                  </template>
              </el-table-column>
              <el-table-column label="操作" width="80" align="center">
                  <template #default="scope">
                      <el-button size="small" type="danger" :icon="DeleteFilled" circle @click="deleteRule(props.row, scope.$index)"></el-button>
                  </template>
              </el-table-column>
            </el-table>
          </div>
        </template>
      </el-table-column>

      <el-table-column prop="id" label="ID" width="100">
        <template #default="scope">
           <el-input-number v-model="scope.row.id" :min="1" :max="255" size="small" controls-position="right"></el-input-number>
        </template>
      </el-table-column>
      
      <el-table-column prop="desc" label="描述">
        <template #default="scope">
           <el-input v-model="scope.row.desc" size="small"></el-input>
        </template>
      </el-table-column>

      <el-table-column prop="mode" label="模式" width="150">
        <template #default="scope">
           <el-select v-model="scope.row.mode" size="small" placeholder="选择">
             <el-option
               v-for="item in modeOptions"
               :key="item.value"
               :label="item.label"
               :value="item.value"
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
import { ElMessage } from 'element-plus'
import { DeleteFilled, InfoFilled, ArrowRight } from '@element-plus/icons-vue'

const endpoint = '/PssSchemeTable'
const tableData = ref([])
const loading = ref(false)
const saving = ref(false)

// Attributes Reference
const attributesData = ref([])
const showAttributes = ref(false)

// Channel Options
const channelOptions = ref([])

const directionMap = {
  1: '东', 2: '南', 3: '西', 4: '北', 5: '其他'
}

const modeOptions = [
  { value: 0, label: '信号机自控' },
  { value: 1, label: '智能感应' },
  { value: 2, label: '智能半感应' },
  { value: 3, label: '专家模式' }
]

const typeMap = {
  0: '未定义', 1: '左转', 2: '直行', 3: '右转', 4: '圆盘'
}

const formatChannelLabel = (channel) => {
    const dir = directionMap[channel.dir] || 'Unknown'
    const type = typeMap[channel.type] || 'Unknown'
    return `${channel.id} - ${dir} - ${type}`
}

const fetchChannels = async () => {
    try {
        const res = await axios.get('/ChannelCtrlTable')
        let data = res.data
        if (typeof data === 'string') {
            try { data = JSON.parse(data) } catch(e){}
        }
        channelOptions.value = Array.isArray(data) ? data : []
    } catch(e) {
        console.error('Failed to fetch channels')
    }
}

const fetchAttributes = async () => {
  try {
    const res = await axios.get('/TrafficAttributeTable')
    let data = res.data
    if (typeof data === 'string') {
        try { data = JSON.parse(data) } catch(e){}
    }
    attributesData.value = Array.isArray(data) ? data : []
  } catch (e) {
    console.error('Failed to fetch attributes')
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
    // Ensure rules is an array for every item
    if (Array.isArray(data)) {
        data.forEach(item => {
            if (!Array.isArray(item.rules)) item.rules = []
        })
    }
    tableData.value = Array.isArray(data) ? data : []
  } catch (e) {
    ElMessage.error('加载数据失败')
  } finally {
    loading.value = false
  }
}

const addItem = () => {
  let maxId = 0
  tableData.value.forEach(item => { if (item.id > maxId) maxId = item.id })
  
  const sortedChannels = [...channelOptions.value].sort((a, b) => a.id - b.id)
  
  const defaultRules = sortedChannels.map(ch => ({
    channel: ch.id,
    addition_type: 0,
    addition_expr: '',
    request_type: 0,
    request_expr: ''
  }))
  
  tableData.value.push({
      id: maxId + 1,
      desc: '新方案',
      mode: 0,
      rules: defaultRules
  })
}

const handleDelete = (index) => {
    tableData.value.splice(index, 1)
}

const addRule = (row) => {
    row.rules.push({
        channel: 1,
        addition_type: 0,
        addition_expr: '',
        request_type: 0,
        request_expr: ''
    })
}

const deleteRule = (row, index) => {
    row.rules.splice(index, 1)
}

const saveData = async () => {
  saving.value = true
  
  // Basic validation for uniqueness of ID
  const idSet = new Set()
  for (let i = 0; i < tableData.value.length; i++) {
      const row = tableData.value[i]
      if (idSet.has(row.id)) {
          ElMessage.error(`第 ${i + 1} 行: 方案ID重复 "${row.id}"`)
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

onMounted(() => {
    fetchData()
    fetchAttributes()
    fetchChannels()
})
</script>

<style scoped>
.scheme-config {
    padding: 20px;
}
.header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-bottom: 20px;
}
.nested-table {
    padding: 10px 20px 20px 20px;
    background-color: #f5f7fa;
}
.nested-header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-bottom: 10px;
}
.nested-header h3 {
    margin: 0;
    font-size: 14px;
    color: #606266;
}

/* Reference Section */
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
