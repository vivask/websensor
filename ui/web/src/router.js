/* eslint-disable */
import Vue from 'vue'
import Router from 'vue-router'
import Settings from './components/Settings.vue'
import Ds18b20 from './components/Ds18b20.vue'
import Ds18b20Chart from './components/Ds18b20Chart.vue'
import Bmx280 from './components/Bmx280.vue'
import Bmx280Chart from './components/Bmx280Chart.vue'

Vue.use(Router)

export default new Router({
  mode: 'history',
  base: process.env.BASE_URL,
  routes: [
    {
      path: '/',
      name: 'settings',
      component: Settings
    },
    {
      path: '/ds18b20',
      name: 'ds18b20',
      component: Ds18b20
    },
    {
      path: '/bmx280',
      name: 'bmx280',
      component: Bmx280
    },
    {
      path: '/ds18b20/chart',
      name: 'ds18b20_chart',
      component: Ds18b20Chart
    },
    {
      path: '/bmx280/chart',
      name: 'bmx280_chart',
      component: Bmx280Chart
    }
  ]
})