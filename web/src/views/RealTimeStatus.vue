<template>
  <div class="realtime-container">
    <!-- Left: Simulation Area -->
    <div class="simulation-area">
      <div class="intersection">
        <!-- North Bound Lanes (Going North, located at Bottom) -->
        <div class="lane-group group-north">
            <div v-for="lane in getLanesByDir(4)" :key="lane.laneNo" class="lane vertical">
                <div class="lane-content">
                    <component :is="getTurnIcon(lane.turn)" class="signal-icon" :style="{ color: getStatusColor(lane.status) }" />
                    <span class="car-num">{{ lane.carNum }}</span>
                </div>
            </div>
        </div>

        <!-- South Bound Lanes (Going South, located at Top) -->
        <div class="lane-group group-south">
            <div v-for="lane in getLanesByDir(2)" :key="lane.laneNo" class="lane vertical">
                <div class="lane-content down">
                    <component :is="getTurnIcon(lane.turn)" class="signal-icon" :style="{ color: getStatusColor(lane.status) }" />
                    <span class="car-num">{{ lane.carNum }}</span>
                </div>
            </div>
        </div>

        <!-- East Bound Lanes (Going East, located at Left) -->
        <div class="lane-group group-east">
            <div v-for="lane in getLanesByDir(1)" :key="lane.laneNo" class="lane horizontal">
                <div class="lane-content">
                    <component :is="getTurnIcon(lane.turn)" class="signal-icon" :style="{ color: getStatusColor(lane.status) }" />
                    <span class="car-num">{{ lane.carNum }}</span>
                </div>
            </div>
        </div>

        <!-- West Bound Lanes (Going West, located at Right) -->
        <div class="lane-group group-west">
            <div v-for="lane in getLanesByDir(3)" :key="lane.laneNo" class="lane horizontal">
                <div class="lane-content right-to-left">
                    <component :is="getTurnIcon(lane.turn)" class="signal-icon" :style="{ color: getStatusColor(lane.status) }" />
                    <span class="car-num">{{ lane.carNum }}</span>
                </div>
            </div>
        </div>
        
        <!-- Center Box -->
        <div class="center-zone"></div>
      </div>
    </div>

    <!-- Right: Info Panel -->
    <div class="info-panel">
      <el-card class="info-card">
        <template #header>
          <div class="card-header">
            <span>System Status</span>
            <el-tag type="success" effect="dark">Live</el-tag>
          </div>
        </template>
        
        <div class="info-item">
            <span class="label">Time (UTC):</span>
            <span class="value">{{ formatTime(statusData.utc) }}</span>
        </div>

        <el-divider content-position="left">Control Rule</el-divider>
        
        <div class="info-item">
            <span class="label">Control ID:</span>
            <span class="value">{{ statusData.rule?.ctrlId }}</span>
        </div>
        <div class="info-item">
            <span class="label">Trigger ID:</span>
            <span class="value">{{ statusData.rule?.triggerId }}</span>
        </div>
        <div class="info-item">
            <span class="label">Description:</span>
            <span class="value">{{ statusData.rule?.desc || 'N/A' }}</span>
        </div>
        <div class="info-item">
            <span class="label">Type:</span>
            <span class="value">{{ getCtrlTypeLabel(statusData.rule?.ctrlType) }}</span>
        </div>
        <div class="info-item">
            <span class="label">Mode:</span>
            <span class="value">{{ getCtrlModeLabel(statusData.rule?.ctrlMode) }}</span>
        </div>
        
        <div class="info-item">
            <span class="label">Duration:</span>
            <span class="value">{{ statusData.rule?.durnation }}s</span>
        </div>
        <div class="info-item">
            <span class="label">Time Left:</span>
            <span class="value time-left">{{ statusData.rule?.left }}s</span>
        </div>

      </el-card>
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted, onUnmounted } from 'vue'
import axios from 'axios'
import { SignalLeft, SignalStraight, SignalRight, SignalRound, SignalUnknown } from '../components/TrafficIcons.js'

const statusData = ref({
    utc: 0,
    rule: {},
    lanes: [],
    channels: []
})

let timer = null

const fetchData = async () => {
    try {
        const res = await axios.get('/PssRealTimeStatus')
        let data = res.data
        if (typeof data === 'string') {
            try { data = JSON.parse(data) } catch(e) {}
        }
        statusData.value = data
    } catch (e) {
        console.error("Fetch status failed", e)
    }
}

onMounted(() => {
    fetchData()
    timer = setInterval(fetchData, 1000)
})

onUnmounted(() => {
    if (timer) clearInterval(timer)
})

// Helpers
const getLanesByDir = (dir) => {
    if (!statusData.value.lanes) return []
    return statusData.value.lanes.filter(l => l.dir === dir).sort((a,b) => a.laneNo - b.laneNo)
}

const getTurnIcon = (turn) => {
    // TurnType: 1=Left, 2=Straight, 4=Right
    // Bitwise check
    if (turn & 1) return SignalLeft
    if (turn & 4) return SignalRight
    if (turn & 2) return SignalStraight
    return SignalUnknown
}

const getStatusColor = (status) => {
    // 0 unknown, 1 green, 2 red, 3 yellow
    switch(status) {
        case 1: return '#2ecc71' // Green
        case 2: return '#e74c3c' // Red
        case 3: return '#f1c40f' // Yellow
        default: return '#909399' // Grey
    }
}

const formatTime = (utc) => {
    if (!utc) return '--'
    // Assuming utc is unix timestamp in seconds
    return new Date(utc * 1000).toLocaleString()
}

const getCtrlTypeLabel = (type) => {
    const map = {
        0: 'Local Control', 1: 'Local Trigger', 2: 'Client Control', 3: 'Web Control', 4: 'Platform Control'
    }
    return map[type] || 'Unknown'
}

const getCtrlModeLabel = (mode) => {
    const map = { 0: 'System', 1: 'Scheme', 2: 'Idle' }
    return map[mode] || 'Unknown'
}

</script>

<style scoped>
.realtime-container {
    display: flex;
    height: calc(100vh - 60px); /* Adjust based on header height */
    padding: 20px;
    gap: 20px;
}

.simulation-area {
    flex: 1;
    background-color: #34495e;
    border-radius: 8px;
    display: flex;
    align-items: center;
    justify-content: center;
    position: relative;
    min-width: 500px;
}

.intersection {
    width: 600px;
    height: 600px;
    position: relative;
    background-color: #2c3e50; /* Dark road color */
    /* Use clip-path or simple divs to make a cross shape? 
       Actually, laying out lanes on a square canvas is easier.
    */
}

/* Lane Groups Positioning */
.lane-group {
    position: absolute;
    display: flex;
    background-color: #2c3e50;
}

/* 
Coordinates logic:
Center is 50%, 50%.
Road width approx 120px.
*/

/* North Group (Bottom of intersection, heading Up) */
.group-north {
    bottom: 0;
    left: 50%;
    transform: translateX(-50%);
    height: 40%;
    align-items: flex-start; /* Cars start from bottom */
    border-right: 2px dashed #f1c40f; /* Divider line */
}

/* South Group (Top of intersection, heading Down) */
.group-south {
    top: 0;
    left: 50%;
    transform: translateX(-50%);
    height: 40%;
    align-items: flex-end; 
    border-left: 2px dashed #f1c40f;
}

/* East Group (Left of intersection, heading Right) */
.group-east {
    left: 0;
    top: 50%;
    transform: translateY(-50%);
    width: 40%;
    flex-direction: column;
    border-bottom: 2px dashed #f1c40f;
}

/* West Group (Right of intersection, heading Left) */
.group-west {
    right: 0;
    top: 50%;
    transform: translateY(-50%);
    width: 40%;
    flex-direction: column;
    border-top: 2px dashed #f1c40f;
}

.lane {
    border: 1px solid #7f8c8d;
    display: flex;
    align-items: center;
    justify-content: center;
    position: relative;
}

.lane.vertical {
    width: 40px;
    height: 100%;
}

.lane.horizontal {
    height: 40px;
    width: 100%;
}

.lane-content {
    display: flex;
    flex-direction: column;
    align-items: center;
    gap: 5px;
}

/* Rotate icons based on direction */
.group-north .signal-icon { transform: rotate(0deg); }
.group-south .signal-icon { transform: rotate(180deg); }
.group-east .signal-icon { transform: rotate(90deg); }
.group-west .signal-icon { transform: rotate(-90deg); }

.car-num {
    font-size: 12px;
    color: #fff;
    background: #333;
    padding: 2px 5px;
    border-radius: 4px;
}

.center-zone {
    position: absolute;
    top: 40%;
    left: 40%;
    width: 20%;
    height: 20%;
    border: 1px dashed #95a5a6;
    background: transparent;
}

.info-panel {
    width: 350px;
}

.info-card {
    height: 100%;
}

.card-header {
    display: flex;
    justify-content: space-between;
    align-items: center;
}

.info-item {
    display: flex;
    justify-content: space-between;
    margin-bottom: 15px;
    font-size: 14px;
}

.label {
    color: #606266;
    font-weight: bold;
}

.value {
    color: #303133;
}

.time-left {
    font-size: 24px;
    font-weight: bold;
    color: #409EFF;
}
</style>
