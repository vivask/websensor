import Vue from 'vue'
import App from './App.vue'
import vuetify from './plugins/vuetify'
import Router from 'vue-router'
import axios from 'axios'

Vue.config.productionTip = false

Vue.prototype.$ajax = axios

Vue.use(Router);

const routes = [
  {
    path: "",
    component: () => import('@/components/Settings.vue'),
  },
  {
    path: "/ds18b20",
    component: {
      render: (h) => h("div", ["Board Page", h("router-view")]),
    },
  }
]

const router = new Router({
  routes,
  mode: "history",
});

new Vue({
  vuetify,
  router,
  render: h => h(App)
}).$mount('#app')
