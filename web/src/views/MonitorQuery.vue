<template>
  <div class="monitor-query">
    <div class="header">
      <h2>System Monitor Query</h2>
    </div>

    <el-card class="query-card">
      <el-form :inline="true" :model="form" class="demo-form-inline">
        <el-form-item label="Time Range">
          <el-date-picker
            v-model="timeRange"
            type="datetimerange"
            range-separator="To"
            start-placeholder="Start date"
            end-placeholder="End date"
            value-format="X" 
          />
        </el-form-item>
        <el-form-item>
          <el-button type="primary" @click="handleQuery" :loading="loading">Query</el-button>
        </el-form-item>
      </el-form>
    </el-card>

    <div v-loading="loading" class="chart-container">
        <div ref="chartRef" style="width: 100%; height: 500px;"></div>
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted, onUnmounted, nextTick } from 'vue'
import axios from 'axios'
import * as echarts from 'echarts'
import { ElMessage } from 'element-plus'

const form = ref({})
// Default to last 24 hours
const end = new Date()
const start = new Date()
start.setTime(start.getTime() - 3600 * 1000 * 24)
// Value format "X" returns timestamp string in seconds
const timeRange = ref([Math.floor(start.getTime()/1000), Math.floor(end.getTime()/1000)])

const loading = ref(false)
const chartRef = ref(null)
let myChart = null

const initChart = () => {
    if (chartRef.value) {
        myChart = echarts.init(chartRef.value)
    }
}

const handleQuery = async () => {
    if (!timeRange.value || timeRange.value.length < 2) {
        ElMessage.warning('Please select a time range')
        return
    }

    loading.value = true
    try {
        // timeRange.value are strings of seconds due to value-format="X"
        const beginTime = parseInt(timeRange.value[0])
        const endTime = parseInt(timeRange.value[1])

        const res = await axios.post('/monitor', {
            beginTime,
            endTime,
            queryType: 0
        })

        let data = res.data
        if (typeof data === 'string') {
            try { data = JSON.parse(data) } catch(e){}
        }

        renderChart(data)
    } catch (e) {
        console.error(e)
        ElMessage.error('Query failed')
    } finally {
        loading.value = false
    }
}

const renderChart = (data) => {
    if (!myChart) initChart()
    if (!data.tables || data.tables.length === 0) {
        myChart.clear()
        ElMessage.info('No data returned')
        return
    }

    const series = []
    const legendData = []
    let xAxisData = []

    // Assuming all tables share roughly same timestamps, but let's be safe.
    // ECharts handles time axis well.
    
    data.tables.forEach(table => {
        legendData.push(table.name)
        const points = table.datas.map(d => [d.timestamp * 1000, d.value])
        series.push({
            name: table.name,
            type: 'line',
            data: points,
            smooth: true,
            showSymbol: false
        })
    })

    const option = {
        title: {
            text: data.desc || 'Monitor Data',
            subtext: `Unit: ${data.unit || ''}`
        },
        tooltip: {
            trigger: 'axis',
            formatter: function (params) {
                let res = new Date(params[0].value[0]).toLocaleString() + '<br/>'
                params.forEach(item => {
                    res += item.marker + item.seriesName + ': ' + item.value[1] + '<br/>'
                })
                return res
            }
        },
        legend: {
            data: legendData
        },
        grid: {
            left: '3%',
            right: '4%',
            bottom: '3%',
            containLabel: true
        },
        xAxis: {
            type: 'time',
            boundaryGap: false
        },
        yAxis: {
            type: 'value'
        },
        series: series
    }

    myChart.setOption(option)
}

const resizeHandler = () => {
    if (myChart) myChart.resize()
}

onMounted(() => {
    window.addEventListener('resize', resizeHandler)
})

onUnmounted(() => {
    window.removeEventListener('resize', resizeHandler)
    if (myChart) myChart.dispose()
})
</script>

<style scoped>
.monitor-query {
    padding: 20px;
}
.query-card {
    margin-bottom: 20px;
}
.chart-container {
    background: #fff;
    padding: 20px;
    border-radius: 4px;
    box-shadow: 0 2px 12px 0 rgba(0,0,0,0.1);
}
</style>
