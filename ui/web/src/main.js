import Vue from 'vue'
import App from './App.vue'
import vuetify from './plugins/vuetify'
import axios from 'axios'
import store from './store'
import VModal from 'vue-js-modal';

Vue.config.productionTip = false

Vue.prototype.$ajax = axios

Vue.use(VModal);

new Vue({
  vuetify,
  store,
  render: h => h(App)
}).$mount('#app')
