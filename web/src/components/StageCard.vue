<template>
  <div class="stage-card" :class="stageStatusClass">
    <div class="grid-container">
      <!-- 位置1: 空 -->
      <div class="grid-cell empty"></div>
      
      <!-- 位置2: 北方向 -->
      <div class="grid-cell direction-north">
        <img 
          v-if="northImage" 
          :src="northImage" 
          class="direction-img" 
          :style="{ transform: `rotate(180deg)` }"
        />
      </div>
      
      <!-- 位置3: 空 -->
      <div class="grid-cell empty"></div>
      
      <!-- 位置4: 西方向 -->
      <div class="grid-cell direction-west">
        <img 
          v-if="westImage" 
          :src="westImage" 
          class="direction-img" 
          :style="{ transform: `rotate(90deg)` }"
        />
      </div>
      
      <!-- 位置5: 中心-时间信息 -->
      <div class="grid-cell center-info">
        <div class="time-display">
          <div class="time-green">{{ stage.green || 0 }}</div>
          <div class="time-divider">/</div>
          <div class="time-duration">{{ stage.durnation || 0 }}</div>
        </div>
        <div class="time-label">绿灯/总时长</div>
      </div>
      
      <!-- 位置6: 东方向 -->
      <div class="grid-cell direction-east">
        <img 
          v-if="eastImage" 
          :src="eastImage" 
          class="direction-img" 
          :style="{ transform: `rotate(-90deg)` }"
        />
      </div>
      
      <!-- 位置7: 空 -->
      <div class="grid-cell empty"></div>
      
      <!-- 位置8: 南方向 -->
      <div class="grid-cell direction-south">
        <img 
          v-if="southImage" 
          :src="southImage" 
          class="direction-img" 
        />
      </div>
      
      <!-- 位置9: 空 -->
      <div class="grid-cell empty"></div>
    </div>
  </div>
</template>

<script setup>
import { computed } from 'vue'
import waysImg from '../assets/way/ways.svg'
import turnLeftImg from '../assets/way/turn_left.svg'
import turnRightImg from '../assets/way/turn_right.svg'
import straightImg from '../assets/way/straight.svg'
import forkLeftImg from '../assets/way/fork_left.svg'
import forkRightImg from '../assets/way/fork_right.svg'

const props = defineProps({
  stage: {
    type: Object,
    required: true
  },
  channels: {
    type: Array,
    default: () => []
  }
})

// LaneDirection: EAST=1, SOUTH=2, WEST=3, NORTH=4
// EntryLamp: LP_LEFT=1, LP_STRAIGHT=2, LP_RIGHT=3, LP_MAIN=4

const LANE_DIRECTION = {
  EAST: 1,
  SOUTH: 2,
  WEST: 3,
  NORTH: 4
}

const ENTRY_LAMP = {
  LP_LEFT: 1,
  LP_STRAIGHT: 2,
  LP_RIGHT: 3,
  LP_MAIN: 4
}

// 根据阶段关联的通道ID获取各方向的通道信息
const getChannelsByDirection = (dir) => {
  if (!props.stage.channels || !props.channels) return []
  const channelIds = props.stage.channels
  return props.channels.filter(ch => ch.dir === dir && channelIds.includes(ch.id))
}

// 获取某方向上存在的灯类型组合
const getLampTypes = (dir) => {
  const dirChannels = getChannelsByDirection(dir)
  const types = new Set()
  dirChannels.forEach(ch => {
    if (ch.type === ENTRY_LAMP.LP_LEFT) types.add('left')
    if (ch.type === ENTRY_LAMP.LP_STRAIGHT) types.add('straight')
    if (ch.type === ENTRY_LAMP.LP_RIGHT) types.add('right')
    if (ch.type === ENTRY_LAMP.LP_MAIN) types.add('main')
  })
  return types
}

// 根据灯类型组合确定显示的图片
const getImageByTypes = (types) => {
  if (types.has('main')) return waysImg
  const hasLeft = types.has('left')
  const hasStraight = types.has('straight')
  const hasRight = types.has('right')
  
  if (hasLeft && hasStraight && hasRight) return waysImg
  if (hasLeft && hasStraight) return forkLeftImg
  if (hasRight && hasStraight) return forkRightImg
  if (hasLeft) return turnLeftImg
  if (hasRight) return turnRightImg
  if (hasStraight) return straightImg
  
  return null
}

const northImage = computed(() => getImageByTypes(getLampTypes(LANE_DIRECTION.NORTH)))
const southImage = computed(() => getImageByTypes(getLampTypes(LANE_DIRECTION.SOUTH)))
const westImage = computed(() => getImageByTypes(getLampTypes(LANE_DIRECTION.WEST)))
const eastImage = computed(() => getImageByTypes(getLampTypes(LANE_DIRECTION.EAST)))

// StageStatus: STAGE_FINISHED=0, STAGE_RUNNING=1, STAGE_TRANSITION=2
const STAGE_STATUS = {
  FINISHED: 0,
  RUNNING: 1,
  TRANSITION: 2
}

const stageStatusClass = computed(() => {
  switch (props.stage.stat) {
    case STAGE_STATUS.RUNNING: return 'status-running'
    case STAGE_STATUS.TRANSITION: return 'status-transition'
    case STAGE_STATUS.FINISHED: 
    default: return 'status-finished'
  }
})
</script>

<style scoped>
.stage-card {
  flex: 1;
  min-width: 120px;
  max-width: 160px;
  height: 100px;
  border-radius: 8px;
  padding: 4px;
  border: 2px solid #dcdfe6;
  background-color: #fff;
  transition: all 0.3s ease;
}

.stage-card.status-running {
  border-color: #67c23a;
  box-shadow: 0 0 8px rgba(103, 194, 58, 0.4);
}

.stage-card.status-transition {
  border-color: #e6a23c;
  box-shadow: 0 0 8px rgba(230, 162, 60, 0.4);
}

.stage-card.status-finished {
  border-color: #909399;
  opacity: 0.7;
}

.grid-container {
  display: grid;
  grid-template-columns: repeat(3, 1fr);
  grid-template-rows: repeat(3, 1fr);
  width: 100%;
  height: 100%;
  gap: 2px;
}

.grid-cell {
  display: flex;
  align-items: center;
  justify-content: center;
  border-radius: 4px;
  background-color: #f5f7fa;
}

.grid-cell.empty {
  background-color: transparent;
}

.direction-img {
  width: 28px;
  height: 28px;
  object-fit: contain;
}

.center-info {
  flex-direction: column;
  background-color: #ecf5ff;
  gap: 2px;
}

.time-display {
  display: flex;
  align-items: baseline;
  justify-content: center;
  gap: 2px;
}

.time-green {
  font-size: 18px;
  font-weight: bold;
  color: #67c23a;
}

.time-divider {
  font-size: 12px;
  color: #909399;
}

.time-duration {
  font-size: 14px;
  font-weight: 500;
  color: #606266;
}

.time-label {
  font-size: 8px;
  color: #909399;
}
</style>
