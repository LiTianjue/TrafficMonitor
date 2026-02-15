<template>
  <div class="config-form">
    <h2>{{ title }}</h2>
    <el-form :model="formData" label-width="150px" v-loading="loading">
       <el-form-item v-for="(val, key) in formData" :key="key" :label="key">
            <el-input v-if="typeof val === 'string'" v-model="formData[key]"></el-input>
            <el-input-number v-else-if="typeof val === 'number'" v-model="formData[key]"></el-input-number>
            <el-switch v-else-if="typeof val === 'boolean'" v-model="formData[key]"></el-switch>
            <el-input v-else type="textarea" :model-value="JSON.stringify(val)" @input="v => updateJson(key, v)"></el-input>
       </el-form-item>
       <el-form-item>
           <el-button type="primary" @click="saveData">Save Configuration</el-button>
       </el-form-item>
    </el-form>
  </div>
</template>

<script setup>
import { ref, onMounted, watch } from 'vue'
import axios from 'axios'
import { ElMessage } from 'element-plus'

const props = defineProps({
  endpoint: String,
  title: String
})

const formData = ref({})
const loading = ref(false)

const fetchData = async () => {
    loading.value = true
    try {
        const res = await axios.get(props.endpoint)
        let data = res.data
        if (typeof data === 'string') {
            try { data = JSON.parse(data) } catch(e){}
        }
        formData.value = data
    } catch(e) {
        ElMessage.error('Failed to load')
    } finally {
        loading.value = false
    }
}

const updateJson = (key, v) => {
    try {
        formData.value[key] = JSON.parse(v)
    } catch(e){}
}

const saveData = async () => {
    loading.value = true
    try {
        await axios.post(props.endpoint, formData.value)
        ElMessage.success('Saved')
    } catch(e) {
        ElMessage.error('Failed to save')
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
