import Vue from 'vue'
import App from './App.vue'
import vuetify from './plugins/vuetify'
import axios from 'axios'

Vue.config.productionTip = false

Vue.prototype.$ajax = axios
Vue.prototype.$menu_filter = 'avg'
Vue.prototype.$menu_bmx280 = 'temperature'


new Vue({
  vuetify,
  render: h => h(App)
}).$mount('#app')
