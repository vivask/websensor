import Vue from 'vue'
import Vuex from 'vuex'
//import axios from 'axios'

Vue.use(Vuex)

export default new Vuex.Store({
  state: {
    menu_filter: 'avg',
    menu_bmx280: 'temperature'
  },
  mutations: {
    update_menu_filter: (state, new_value) => {
      state.menu_filter = new_value;
    },
    update_menu_bmx280: (state, new_value) => {
      state.menu_bmx280 = new_value;
    },
  },
  getters: {
    get_menu_filter: state => {
      return state.menu_filter;
    },
    get_menu_bmx280: state => {
      return state.menu_bmx280;
    }
  }
})
