import Vue from 'vue'
import Router from 'vue-router'

Vue.use(Router)

export default new Router({
  mode: 'history',
  base: process.env.BASE_URL,
  routes: [
    {
      path: '/',
      name: 'settings',
      component: () => import('@/components/Settings.vue')
    },
    {
      path: '/ds18b20',
      name: 'ds18b20',
      component: () => import('@/components/Ds18b20.vue')
    },
    {
      path: '/bmx280',
      name: 'bmx280',
      component: () => import('@/components/Bmx280.vue')
    },
    {
      path: '/ds18b20/chart',
      name: 'ds18b20_chart',
      component: () => import('@/components/Ds18b20Chart.vue')
    },
    {
      path: '/bmx280/chart',
      name: 'bmx280_chart',
      component: () => import('@/components/Bmx280Chart.vue')
    }
  ]
})