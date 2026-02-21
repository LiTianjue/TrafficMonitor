<template>
  <div class="monitor-query">
    <div class="header">
      <h2>Data Query</h2>
    </div>

    <el-card class="query-card">
      <el-form :inline="true" :model="form" class="query-form">
        <el-form-item label="Time Range">
          <el-date-picker
            v-model="timeRange"
            type="datetimerange"
            range-separator="To"
            start-placeholder="Start"
            end-placeholder="End"
            value-format="X"
            :default-time="[new Date(0), new Date(235959000000)]"
          />
        </el-form-item>
        <el-form-item>
          <el-button-group>
            <el-button @click="setYesterday">前一天</el-button>
            <el-button @click="setToday">今天</el-button>
            <el-button @click="setTomorrow">后一天</el-button>
          </el-button-group>
        </el-form-item>
        <el-form-item>
          <el-button type="primary" @click="handleQuery" :loading="loading">查询</el-button>
        </el-form-item>
      </el-form>

      <el-form :inline="true" :model="form" class="compare-form">
        <el-form-item label="Compare Range">
          <el-date-picker
            v-model="compareRange"
            type="datetimerange"
            range-separator="To"
            start-placeholder="Compare Start"
            end-placeholder="Compare End"
            value-format="X"
            :default-time="[new Date(0), new Date(235959000000)]"
          />
        </el-form-item>
        <el-form-item>
          <el-button-group>
            <el-button @click="setCompareYesterday">前一天</el-button>
            <el-button @click="setCompareToday">今天</el-button>
            <el-button @click="setCompareTomorrow">后一天</el-button>
          </el-button-group>
        </el-form-item>
        <el-form-item>
          <el-checkbox v-model="enableCompare">数据比对</el-checkbox>
        </el-form-item>
      </el-form>

      <div class="query-types">
        <el-checkbox-group v-model="selectedTypes">
          <el-checkbox 
            v-for="attr in statisticsAttributes" 
            :key="attr.value" 
            :label="attr.value"
          >
            {{ attr.label }}
          </el-checkbox>
        </el-checkbox-group>
        <el-checkbox v-model="enableCustomType">
          自定义类型
          <el-input-number 
            v-model="customTypeValue" 
            :min="0" 
            :max="9999" 
            size="small" 
            controls-position="right"
          />
        </el-checkbox>
      </div>
    </el-card>

    <div v-loading="loading" class="chart-container">
      <div v-if="chartDataList.length === 0" class="empty-tip">
        请选择查询项目并点击查询
      </div>
      <div 
        v-for="(chartData, index) in chartDataList" 
        :key="index" 
        class="chart-item"
      >
        <div :ref="el => chartRefs[index] = el" :style="{ width: '100%', height: '400px' }"></div>
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted, onUnmounted, nextTick } from 'vue'
import axios from 'axios'
import * as echarts from 'echarts'
import { ElMessage } from 'element-plus'

const statisticsAttributes = [
  { value: 0x201, label: '总流量' },
  { value: 0x202, label: '直行总流量' },
  { value: 0x203, label: '左转总流量' },
  { value: 0x204, label: '右转总流量' },
  { value: 0x205, label: '最大排队长度' },
  { value: 0x206, label: '最大排队车辆数' },
  { value: 0x207, label: '车道空闲率' },
  { value: 0x208, label: '车道饱和率' },
  { value: 0x209, label: '红初最大车辆数' },
  { value: 0x20A, label: '绿灯损失' },
  { value: 0x20B, label: '绿信比' }
]

const form = ref({})
const timeRange = ref([])
const compareRange = ref([])
const selectedTypes = ref([])
const enableCompare = ref(false)
const enableCustomType = ref(false)
const customTypeValue = ref(0)

const loading = ref(false)
const chartDataList = ref([])
const chartRefs = ref([])
const charts = ref([])

const initTimeRange = () => {
  const now = new Date()
  const start = new Date(now)
  start.setTime(start.getTime() - 24 * 60 * 60 * 1000)
  
  const startSec = Math.floor(start.getTime() / 1000)
  const endSec = Math.floor(now.getTime() / 1000)
  timeRange.value = [startSec, endSec]
}

const setYesterday = () => {
  if (!timeRange.value || timeRange.value.length < 2) return
  const DAY_24H = 24 * 60 * 60
  timeRange.value = [
    timeRange.value[0] - DAY_24H,
    timeRange.value[1] - DAY_24H
  ]
}

const setTomorrow = () => {
  if (!timeRange.value || timeRange.value.length < 2) return
  const DAY_24H = 24 * 60 * 60
  timeRange.value = [
    timeRange.value[0] + DAY_24H,
    timeRange.value[1] + DAY_24H
  ]
}

const setToday = () => {
  const now = new Date()
  const today = new Date(now.getFullYear(), now.getMonth(), now.getDate())
  const startSec = Math.floor(today.getTime() / 1000)
  const endSec = Math.floor(now.getTime() / 1000)
  timeRange.value = [startSec, endSec]
}

const initCompareRange = () => {
  const now = new Date()
  const start = new Date(now)
  start.setTime(start.getTime() - 24 * 60 * 60 * 1000)
  
  const startSec = Math.floor(start.getTime() / 1000)
  const endSec = Math.floor(now.getTime() / 1000)
  compareRange.value = [startSec, endSec]
}

const setCompareYesterday = () => {
  if (!compareRange.value || compareRange.value.length < 2) return
  const DAY_24H = 24 * 60 * 60
  compareRange.value = [
    compareRange.value[0] - DAY_24H,
    compareRange.value[1] - DAY_24H
  ]
}

const setCompareTomorrow = () => {
  if (!compareRange.value || compareRange.value.length < 2) return
  const DAY_24H = 24 * 60 * 60
  compareRange.value = [
    compareRange.value[0] + DAY_24H,
    compareRange.value[1] + DAY_24H
  ]
}

const setCompareToday = () => {
  const now = new Date()
  const today = new Date(now.getFullYear(), now.getMonth(), now.getDate())
  const startSec = Math.floor(today.getTime() / 1000)
  const endSec = Math.floor(now.getTime() / 1000)
  compareRange.value = [startSec, endSec]
}

const handleQuery = async () => {
  if (!timeRange.value || timeRange.value.length < 2) {
    ElMessage.warning('请选择时间范围')
    return
  }

  let typesToQuery = []
  console.log('enableCustomType:', enableCustomType.value, 'customTypeValue:', customTypeValue.value, 'selectedTypes:', selectedTypes.value)
  if (enableCustomType.value && customTypeValue.value > 0) {
    typesToQuery = [customTypeValue.value]
  } else {
    typesToQuery = [...selectedTypes.value]
  }
  console.log('typesToQuery:', typesToQuery)
  
  if (typesToQuery.length === 0) {
    ElMessage.warning('请选择查询项目')
    return
  }

  if (enableCompare.value && (!compareRange.value || compareRange.value.length < 2)) {
    ElMessage.warning('请选择对比时间范围')
    return
  }

  loading.value = true
  chartDataList.value = []
  charts.value = []

  try {
    const beginTime = parseInt(timeRange.value[0])
    const endTime = parseInt(timeRange.value[1])

    for (const queryType of typesToQuery) {
      console.log('Query params:', beginTime, endTime, queryType, typeof queryType)
      let result = await fetchQueryData(beginTime, endTime, queryType)

      if (enableCompare.value) {
        const compareBeginTime = parseInt(compareRange.value[0])
        const compareEndTime = parseInt(compareRange.value[1])
        const compareResult = await fetchQueryData(compareBeginTime, compareEndTime, queryType)
        
        result = mergeCompareData(result, compareResult, beginTime, compareBeginTime)
      }

      chartDataList.value.push(result)
    }

    if (chartDataList.value.length === 0 || !chartDataList.value[0]) {
      ElMessage.warning('未返回数据')
      return
    }

    await nextTick()
    renderAllCharts()

  } catch (e) {
    console.error('Query error:', e)
    ElMessage.error('查询失败: ' + (e.message || ''))
  } finally {
    loading.value = false
  }
}

const fetchQueryData = async (beginTime, endTime, queryType) => {
  try {
    const res = await axios.post('/monitor', {
      beginTime,
      endTime,
      queryType
    })

    let data = res.data
    if (typeof data === 'string') {
      try { data = JSON.parse(data) } catch(e) {}
    }
    return data
  } catch (e) {
    console.error('fetchQueryData error:', e)
    throw e
  }
}

const mergeCompareData = (mainData, compareData, mainBeginTime, compareBeginTime) => {
  if (!mainData || !mainData.tables) return mainData
  if (!compareData || !compareData.tables) return mainData

  const timeOffset = mainBeginTime - compareBeginTime
  const mergedData = { ...mainData }

  let changeRatios = {}

  mergedData.tables = mainData.tables.map(mainTable => {
    const compareTable = compareData.tables.find(t => t.name === mainTable.name)
    if (!compareTable) return mainTable

    const compareDatasRenamed = compareTable.datas.map(d => ({
      ...d,
      timestamp: d.timestamp + timeOffset,
      name: mainTable.name + '(比对)'
    }))

    const mainValues = mainTable.datas.map(d => d.value)
    const compareValues = compareTable.datas.map(d => d.value)
    const mainAvg = mainValues.reduce((a, b) => a + b, 0) / (mainValues.length || 1)
    const compareAvg = compareValues.reduce((a, b) => a + b, 0) / (compareValues.length || 1)
    if (compareAvg !== 0) {
      changeRatios[mainTable.name] = ((mainAvg - compareAvg) / compareAvg * 100).toFixed(2)
    }

    return {
      ...mainTable,
      name: mainTable.name,
      datas: [
        ...mainTable.datas.map(d => ({ ...d, name: mainTable.name })),
        ...compareDatasRenamed
      ]
    }
  })

  mergedData.changeRatios = changeRatios
  return mergedData
}

const renderAllCharts = () => {
  chartDataList.value.forEach((data, index) => {
    if (chartRefs.value[index]) {
      let chart = echarts.getInstanceByDom(chartRefs.value[index])
      if (!chart) {
        chart = echarts.init(chartRefs.value[index])
      }
      charts.value[index] = chart
      renderChart(chart, data)
    }
  })
}

const renderChart = (chart, data) => {
  if (!data || !data.tables || data.tables.length === 0) {
    chart.clear()
    chart.setOption({
      title: { text: '暂无数据', left: 'center', top: 'middle' }
    })
    return
  }

  const series = []
  data.tables.forEach(table => {
    const dataByName = {}
    table.datas.forEach(d => {
      const name = d.name || table.name
      if (!dataByName[name]) {
        dataByName[name] = []
      }
      dataByName[name].push([d.timestamp * 1000, d.value])
    })

    Object.entries(dataByName).forEach(([name, points]) => {
      series.push({
        name: name,
        seriesName: name,
        type: 'line',
        data: points,
        smooth: true,
        showSymbol: false
      })
    })
  })

  series.forEach(s => {
    s.data.sort((a, b) => a[0] - b[0])
  })

  const legendData = series.map(s => s.name)

  let graphicElements = []
  if (data.changeRatios) {
    let idx = 0
    Object.entries(data.changeRatios).forEach(([name, ratio]) => {
      const r = parseFloat(ratio)
      const sign = r >= 0 ? '+' : ''
      const color = r >= 0 ? '#67c23a' : '#f56c6c'
      graphicElements.push({
        type: 'text',
        right: 10,
        top: 20 + idx * 20,
        style: {
          text: `${name}: ${sign}${r}%`,
          fontSize: 12,
          fill: color,
          fontWeight: 'bold'
        }
      })
      idx++
    })
  }

  const option = {
    title: {
      text: data.desc || '查询数据',
      left: 'center'
    },
    graphic: graphicElements.length > 0 ? graphicElements : undefined,
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
      name: data.unit || ''
    },
    series: series
  }

  chart.setOption(option)
}

const resizeHandler = () => {
  charts.value.forEach(chart => {
    if (chart) chart.resize()
  })
}

onMounted(() => {
  initTimeRange()
  initCompareRange()
  window.addEventListener('resize', resizeHandler)
})

onUnmounted(() => {
  window.removeEventListener('resize', resizeHandler)
  charts.value.forEach(chart => {
    if (chart) chart.dispose()
  })
})
</script>

<style scoped>
.monitor-query {
  padding: 20px;
}

.header {
  margin-bottom: 20px;
}

.query-card {
  margin-bottom: 20px;
}

.query-form {
  margin-bottom: 10px;
}

.compare-form {
  margin-bottom: 15px;
  padding-bottom: 15px;
  border-bottom: 1px dashed #eee;
}

.query-types {
  padding: 10px 0;
}

.query-types .el-checkbox-group {
  display: flex;
  flex-wrap: wrap;
  gap: 10px;
}

.query-types .el-checkbox {
  margin-right: 0;
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
