import { createRouter, createWebHistory } from 'vue-router'
import Login from './views/Login.vue'
import Dashboard from './views/Dashboard.vue'
import ConfigTable from './views/ConfigTable.vue'
import ConfigForm from './views/ConfigForm.vue'

import DetectorConfig from './views/DetectorConfig.vue'
import ChannelConfig from './views/ChannelConfig.vue'
import LaneConfig from './views/LaneConfig.vue'
import AttributeConfig from './views/AttributeConfig.vue'
import SchemeConfig from './views/SchemeConfig.vue'
import TriggerConfig from './views/TriggerConfig.vue'
import RealTimeStatus from './views/RealTimeStatus.vue'
import MonitorQuery from './views/MonitorQuery.vue'
import OperationQuery from './views/OperationQuery.vue'
import TscConfig from './views/TscConfig.vue'

const routes = [
  { path: '/login', component: Login },
  { 
    path: '/', 
    component: Dashboard,
    meta: { requiresAuth: true }
  },
  {
    path: '/monitor',
    component: MonitorQuery,
    meta: { requiresAuth: true }
  },
  {
    path: '/operation',
    component: OperationQuery,
    meta: { requiresAuth: true }
  },
  { 
    path: '/realtime', 
    component: RealTimeStatus,
    meta: { requiresAuth: true }
  },
  {
    path: '/tsc',
    component: TscConfig,
    meta: { requiresAuth: true }
  },
  {
      path: '/detectors',
      component: DetectorConfig,
      meta: { requiresAuth: true }
  },
  {
      path: '/channels',
      component: ChannelConfig,
      meta: { requiresAuth: true }
  },
  {
      path: '/attributes',
      component: AttributeConfig,
      meta: { requiresAuth: true }
  },
  {
      path: '/schemes',
      component: SchemeConfig,
      meta: { requiresAuth: true }
  },
  {
      path: '/triggers',
      component: TriggerConfig,
      meta: { requiresAuth: true }
  },
  {
      path: '/lanes',
      component: LaneConfig,
      meta: { requiresAuth: true }
  }
]

const router = createRouter({
  history: createWebHistory(),
  routes
})

router.beforeEach(async (to, from, next) => {
  if (to.meta.requiresAuth) {
    // Simple check: if we can't hit /api/me, we assume not logged in.
    // Optimization: We could store a flag in localStorage, but hitting the API is safer.
    try {
        const res = await fetch('/api/me')
        if (res.status === 200) next()
        else next('/login')
    } catch (e) {
        next('/login')
    }
  } else {
    next()
  }
})

export default router
