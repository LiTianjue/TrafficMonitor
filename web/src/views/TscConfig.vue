<template>
  <div class="config-form">
    <h2>TSC配置</h2>
    <el-form :model="formData" label-width="150px" v-loading="loading">
        <el-form-item label="类型" v-if="formData.type !== undefined">
           <el-select v-model="formData.type" placeholder="选择厂家">
             <el-option
               v-for="item in manufacturerOptions"
               :key="item.value"
               :label="item.label"
               :value="item.value"
             />
          </el-select>
       </el-form-item>
       
        <el-form-item label="IP地址">
          <el-input v-model="formData.ip"></el-input>
       </el-form-item>
       
        <el-form-item label="端口">
          <el-input-number v-model="formData.port" :min="1" :max="65535" controls-position="right"></el-input-number>
       </el-form-item>
       
        <el-form-item label="控制开关">
          <el-switch v-model="formData.controlEnable"></el-switch>
       </el-form-item>
       
       <el-form-item>
            <el-button type="primary" @click="saveData">保存配置</el-button>
       </el-form-item>
    </el-form>
  </div>
</template>

<script setup>
import { ref, onMounted } from 'vue'
import axios from 'axios'
import { ElMessage } from 'element-plus'

const endpoint = '/TscConfig'
const formData = ref({})
const loading = ref(false)

const manufacturerOptions = [
    { value: 0, label: '海康威视' },
    { value: 1, label: '国标20999' },
    { value: 2, label: '信号灯检测器' },
    { value: 3, label: '其他厂家' }
]

const fetchData = async () => {
    loading.value = true
    try {
        const res = await axios.get(endpoint)
        let data = res.data
        if (typeof data === 'string') {
            try { data = JSON.parse(data) } catch(e){}
        }
        formData.value = data
    } catch(e) {
        ElMessage.error('加载失败')
    } finally {
        loading.value = false
    }
}

const saveData = async () => {
    loading.value = true
    try {
        await axios.post(endpoint, formData.value)
        ElMessage.success('保存成功')
    } catch(e) {
        ElMessage.error('保存失败')
    } finally {
        loading.value = false
    }
}

onMounted(fetchData)
</script>

<style scoped>
.config-form {
    padding: 20px;
    max-width: 600px;
}
</style>
