<template>
  <div class="attribute-query">
    <div class="header">
      <h2>交通属性实时查询</h2>
    </div>

    <el-card class="query-card">
      <el-form :inline="true" :model="form" class="query-form">
        <el-form-item label="时间范围">
          <el-date-picker
            v-model="timeRange"
            type="datetimerange"
            range-separator="至"
            start-placeholder="开始"
            end-placeholder="结束"
            value-format="X"
            :default-time="[new Date(0), new Date(235959000000)]"
          />
        </el-form-item>
        <el-form-item>
          <el-button-group>
            <el-button @click="setBefore10Min">前10分钟</el-button>
            <el-button @click="setRealtime">实时</el-button>
            <el-button @click="setAfter10Min">后10分钟</el-button>
          </el-button-group>
        </el-form-item>
        <el-form-item>
          <el-radio-group v-model="displayMode" size="small">
            <el-radio-button label="separate">分开展示</el-radio-button>
            <el-radio-button label="merged">合并展示</el-radio-button>
          </el-radio-group>
        </el-form-item>
        <el-form-item>
          <el-button type="primary" @click="handleQuery" :loading="loading">查询</el-button>
        </el-form-item>
      </el-form>

      <div class="query-tags">
        <el-checkbox-group v-model="selectedTags">
          <el-checkbox 
            v-for="attr in attributeList" 
            :key="attr.tag" 
            :label="attr.tag"
          >
            {{ attr.desc }}({{ attr.tag }})
          </el-checkbox>
        </el-checkbox-group>
        <div class="custom-tags">
          <el-checkbox v-model="enableCustomTags">
            <span>自定义标签</span>
          </el-checkbox>
          <el-input 
            v-for="(tag, index) in customTagValues" 
            :key="index"
            v-model="customTagValues[index]" 
            :placeholder="'自定义' + (index + 1)"
            size="small" 
            style="width: 150px; margin-left: 10px;"
            :disabled="!enableCustomTags"
            @keyup.enter="handleQuery"
          />
        </div>
      </div>
    </el-card>

    <div v-loading="loading" class="chart-container">
      <div v-if="chartDataList.length === 0" class="empty-tip">
        请选择查询标签并点击查询
      </div>
      <template v-else>
        <div v-if="displayMode === 'merged'" class="chart-item">
          <div class="chart-box" :style="{ width: '100%', height: '500px' }"></div>
        </div>
        <div 
          v-else
          v-for="(chartData, index) in chartDataList" 
          :key="index" 
          class="chart-item"
        >
          <div class="chart-box" :style="{ width: '100%', height: '400px' }"></div>
        </div>
      </template>
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted, onUnmounted, nextTick, watch } from 'vue'
import axios from 'axios'
import * as echarts from 'echarts'
import { ElMessage } from 'element-plus'

const form = ref({})
const timeRange = ref([])
const selectedTags = ref([])
const enableCustomTags = ref(false)
const customTagValues = ref(['', '', ''])
const attributeList = ref([])
const displayMode = ref('separate')

const loading = ref(false)
const chartDataList = ref([])
const chartRefs = ref({})
const charts = ref({})

const initCharts = () => {
  Object.values(charts.value).forEach(chart => {
    if (chart) chart.dispose()
  })
  charts.value = {}
  chartRefs.value = {}
}

const initTimeRange = () => {
  setRealtime()
}

const MIN_10 = 10 * 60

const setRealtime = () => {
  const now = new Date()
  const endSec = Math.floor(now.getTime() / 1000)
  const startSec = endSec - MIN_10
  timeRange.value = [startSec, endSec]
}

const setBefore10Min = () => {
  if (!timeRange.value || timeRange.value.length < 2) {
    setRealtime()
    return
  }
  timeRange.value = [
    timeRange.value[0] - MIN_10,
    timeRange.value[1] - MIN_10
  ]
}

const setAfter10Min = () => {
  if (!timeRange.value || timeRange.value.length < 2) {
    setRealtime()
    return
  }
  timeRange.value = [
    timeRange.value[0] + MIN_10,
    timeRange.value[1] + MIN_10
  ]
}

const fetchAttributeList = async () => {
  try {
    const res = await axios.get('/TrafficAttributeTable')
    let data = res.data
    if (typeof data === 'string') {
      try { data = JSON.parse(data) } catch(e) {}
    }
    attributeList.value = Array.isArray(data) ? data : []
  } catch (e) {
    console.error('fetchAttributeList error:', e)
  }
}

const handleQuery = async () => {
  if (!timeRange.value || timeRange.value.length < 2) {
    ElMessage.warning('请选择时间范围')
    return
  }

  let tagsToQuery = [...selectedTags.value]
  
  if (enableCustomTags.value) {
    customTagValues.value.forEach(tag => {
      if (tag && tag.trim()) {
        tagsToQuery.push(tag.trim())
      }
    })
  }
  
  if (tagsToQuery.length === 0) {
    ElMessage.warning('请选择查询标签')
    return
  }

  loading.value = true
  chartDataList.value = []
  initCharts()

  try {
    const beginTime = parseInt(timeRange.value[0])
    const endTime = parseInt(timeRange.value[1])

    const res = await axios.post('/TrafficAttributeQuery', {
      beginTime,
      endTime,
      queryStrings: tagsToQuery
    })

    let data = res.data
    if (typeof data === 'string') {
      try { data = JSON.parse(data) } catch(e) {}
    }

    if (!data || !data.tables || data.tables.length === 0) {
      ElMessage.warning('未返回数据')
      return
    }

    chartDataList.value = data.tables

    await nextTick()
    renderAllCharts()

  } catch (e) {
    console.error('Query error:', e)
    ElMessage.error('查询失败: ' + (e.message || ''))
  } finally {
    loading.value = false
  }
}

const renderAllCharts = () => {
  initCharts()
  nextTick(() => {
    const containers = document.querySelectorAll('.chart-container .chart-item .chart-box')
    
    if (displayMode.value === 'merged') {
      if (containers[0]) {
        let chart = echarts.getInstanceByDom(containers[0])
        if (!chart) {
          chart = echarts.init(containers[0])
        }
        charts.value[0] = chart
        renderMergedChart(chart, chartDataList.value)
      }
    } else {
      chartDataList.value.forEach((data, index) => {
        if (containers[index]) {
          let chart = echarts.getInstanceByDom(containers[index])
          if (!chart) {
            chart = echarts.init(containers[index])
          }
          charts.value[index] = chart
          renderChart(chart, data)
        }
      })
    }
  })
}

const renderChart = (chart, data) => {
  if (!data || !data.datas || data.datas.length === 0) {
    chart.clear()
    chart.setOption({
      title: { text: '暂无数据', left: 'center', top: 'middle' }
    })
    return
  }

  const points = data.datas.map(d => [d.timestamp * 1000, d.value])
  points.sort((a, b) => a[0] - b[0])

  const option = {
    title: {
      text: data.name || '查询数据',
      left: 'center'
    },
    tooltip: {
      trigger: 'axis',
      formatter: function (params) {
        if (!params || params.length === 0) return ''
        const time = new Date(params[0].value[0]).toLocaleString()
        let res = time + '<br/>'
        res += params[0].marker + params[0].seriesName + ': ' + params[0].value[1] + '<br/>'
        return res
      }
    },
    grid: {
      left: '3%',
      right: '4%',
      bottom: '3%',
      top: 50,
      containLabel: true
    },
    xAxis: {
      type: 'time',
      boundaryGap: false
    },
    yAxis: {
      type: 'value',
      name: ''
    },
    series: [
      {
        name: data.name,
        type: 'line',
        data: points,
        smooth: true,
        showSymbol: false
      }
    ]
  }

  chart.setOption(option)
}

const renderMergedChart = (chart, dataList) => {
  if (!dataList || dataList.length === 0) {
    chart.clear()
    chart.setOption({
      title: { text: '暂无数据', left: 'center', top: 'middle' }
    })
    return
  }

  const series = []
  dataList.forEach(table => {
    if (!table.datas || table.datas.length === 0) return
    
    const points = table.datas.map(d => [d.timestamp * 1000, d.value])
    points.sort((a, b) => a[0] - b[0])
    
    series.push({
      name: table.name,
      seriesName: table.name,
      type: 'line',
      data: points,
      smooth: true,
      showSymbol: false
    })
  })

  const legendData = series.map(s => s.name)

  const option = {
    title: {
      text: '标签数据查询',
      left: 'center'
    },
    tooltip: {
      trigger: 'axis',
      formatter: function (params) {
        if (!params || params.length === 0) return ''
        const time = new Date(params[0].value[0]).toLocaleString()
        let res = time + '<br/>'
        params.forEach(item => {
          res += item.marker + item.seriesName + ': ' + item.value[1] + '<br/>'
        })
        return res
      }
    },
    legend: {
      data: legendData,
      top: 40
    },
    grid: {
      left: '3%',
      right: '4%',
      bottom: '3%',
      top: 80,
      containLabel: true
    },
    xAxis: {
      type: 'time',
      boundaryGap: false
    },
    yAxis: {
      type: 'value',
      name: ''
    },
    series: series
  }

  chart.setOption(option)
}

const resizeHandler = () => {
  Object.values(charts.value).forEach(chart => {
    if (chart) chart.resize()
  })
}

onMounted(() => {
  initTimeRange()
  fetchAttributeList()
  window.addEventListener('resize', resizeHandler)
})

watch(displayMode, () => {
  if (chartDataList.value.length > 0) {
    nextTick(() => {
      renderAllCharts()
    })
  }
})

onUnmounted(() => {
  window.removeEventListener('resize', resizeHandler)
  initCharts()
})
</script>

<style scoped>
.attribute-query {
  padding: 20px;
}

.header {
  margin-bottom: 20px;
}

.query-card {
  margin-bottom: 20px;
}

.query-form {
  margin-bottom: 15px;
}

.query-tags {
  padding: 10px 0;
}

.query-tags .el-checkbox-group {
  display: flex;
  flex-wrap: wrap;
  gap: 10px;
  margin-bottom: 10px;
}

.query-tags .el-checkbox {
  margin-right: 0;
}

.custom-tags {
  display: flex;
  align-items: center;
  flex-wrap: wrap;
  gap: 10px;
}

.chart-container {
  background: #fff;
  padding: 20px;
  border-radius: 4px;
  box-shadow: 0 2px 12px 0 rgba(0,0,0,0.1);
  min-height: 400px;
}

.chart-item {
  margin-bottom: 30px;
}

.chart-item:last-child {
  margin-bottom: 0;
}

.empty-tip {
  text-align: center;
  padding: 100px 0;
  color: #909399;
  font-size: 14px;
}
</style>
