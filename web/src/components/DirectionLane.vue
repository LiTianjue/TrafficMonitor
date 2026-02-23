<template>
  <div class="direction-lane" :style="{ transform: `rotate(${rotation}deg)` }">
    <!-- 路口方向（信号灯位置） -->
    <div class="intersection-direction">
      <slot name="signals"></slot>
    </div>

    <!-- 车道区域 -->
    <div class="lanes-area">
      <!-- 对向车道（左侧，无具体数量，无白线） -->
      <div class="opposite-lane">
        <div class="opposite-lane-content">
          <span class="direction-label">{{ getDirectionLabel() }}</span>
        </div>
      </div>

      <!-- 双黄线（中间） -->
      <div class="double-yellow-line"></div>

      <!-- 渠化车道（右侧，有具体数量，有白线） -->
      <div class="channelized-lanes">
        <div 
          v-for="(lane, index) in displayLanes" 
          :key="lane.laneNo || index"
          class="channel-lane"
        >
          <!-- 白色车道分隔线 (从顶部延伸到50%) -->
          <div class="lane-divider"></div>
          
          <!-- 车道内容 -->
          <div class="lane-content">
            <!-- 地面箭头标识（在车道上） -->
            <div class="lane-arrow" :class="getTurnClass(lane.turn)">
              {{ getTurnArrow(lane.turn) }}
            </div>
            
            <!-- 车道信息 -->
            <div class="lane-info">
              <span v-if="showLaneNo" class="lane-no">{{ lane.laneNo }}</span>
              <span v-if="showCarNum" class="car-num">{{ lane.carNum }}</span>
              <span v-if="showStatus" class="lane-status" :style="{ color: getStatusColor(lane.status) }">
                {{ getStatusText(lane.status) }}
              </span>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup>
import { computed } from 'vue'

const props = defineProps({
  dir: {
    type: Number,
    required: true
  },
  lanes: {
    type: Array,
    default: () => []
  },
  rotation: {
    type: Number,
    default: 0
  },
  showLaneNo: {
    type: Boolean,
    default: true
  },
  showCarNum: {
    type: Boolean,
    default: true
  },
  showStatus: {
    type: Boolean,
    default: true
  },
  reverseLane: {
    type: Boolean,
    default: false
  }
})

const displayLanes = computed(() => {
  let lanes = [...props.lanes]
  if (props.reverseLane) {
    lanes = lanes.sort((a, b) => b.laneNo - a.laneNo)
  } else {
    lanes = lanes.sort((a, b) => a.laneNo - b.laneNo)
  }
  return lanes
})

const getDirectionLabel = () => {
  const labels = { 1: '东', 2: '南', 3: '西', 4: '北' }
  return labels[props.dir] || ''
}

const getTurnArrow = (turn) => {
  if (turn & 1) return '←'
  if (turn & 4) return '→'
  if (turn & 2) return '↑'
  return ''
}

const getTurnClass = (turn) => {
  if (turn & 1) return 'turn-left'
  if (turn & 4) return 'turn-right'
  if (turn & 2) return 'turn-straight'
  return ''
}

const getStatusColor = (status) => {
  switch(status) {
    case 1: return '#2ecc71'
    case 2: return '#e74c3c'
    case 3: return '#f1c40f'
    default: return '#909399'
  }
}

const getStatusText = (status) => {
  switch(status) {
    case 1: return '绿'
    case 2: return '红'
    case 3: return '黄'
    default: return ''
  }
}
</script>

<style scoped>
.direction-lane {
  display: flex;
  flex-direction: column;
  align-items: center;
}

.intersection-direction {
  margin-bottom: 4px;
  min-height: 30px;
  display: flex;
  justify-content: center;
  align-items: center;
  color: #fff;
  font-weight: bold;
}

.lanes-area {
  display: flex;
  align-items: stretch;
}

.opposite-lane {
  width: 60px;
  background-color: #2d2d2d;
  display: flex;
  align-items: center;
  justify-content: center;
  border: 1px solid #444;
}

.opposite-lane-content {
  display: flex;
  align-items: center;
  justify-content: center;
}

.direction-label {
  color: #888;
  font-size: 14px;
  font-weight: bold;
}

.double-yellow-line {
  width: 2px;
  background-color: #f4d03f;
}

.channelized-lanes {
  display: flex;
  gap: 4px;
}

.channel-lane {
  width: 50px;
  height: 80px;
  background-color: #353535;
  border-top: 2px solid #fff;
  position: relative;
  overflow: hidden;
}

.lane-divider {
  position: absolute;
  right: 0;
  top: 0;
  width: 2px;
  height: 50%;
  background-color: #fff;
}

.lane-content {
  width: 100%;
  height: 100%;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  gap: 2px;
}

.lane-arrow {
  font-size: 14px;
  font-weight: bold;
  color: #f4d03f;
}

.lane-info {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 1px;
}

.lane-no {
  color: #3498db;
  font-size: 10px;
  font-weight: bold;
}

.car-num {
  color: #e74c3c;
  font-size: 12px;
  font-weight: bold;
}

.lane-status {
  font-size: 9px;
  font-weight: bold;
}
</style>
