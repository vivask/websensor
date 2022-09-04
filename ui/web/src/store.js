import Vue from 'vue'
import Vuex from 'vuex'
//import axios from 'axios'

Vue.use(Vuex)

export default new Vuex.Store({
  state: {
    menu_filter: 'avg',
    menu_bmx280: 'temperature',
    menu_aht: 'temperature',
    items_array: [],
  },
  mutations: {
    update_menu_filter: (state, new_value) => {
      state.menu_filter = new_value;
    },
    update_menu_bmx280: (state, new_value) => {
      state.menu_bmx280 = new_value;
    },
    update_menu_aht: (state, new_value) => {
      state.menu_aht = new_value;
    },
    clear_items_array: (state) => {
      state.items_array = [];
    },
    update_items_array: (state, new_value) => {
      state.items_array = state.items_array.concat(new_value);
    },
  },
  getters: {
    get_menu_filter: state => {
      return state.menu_filter;
    },
    get_menu_bmx280: state => {
      return state.menu_bmx280;
    },
    get_menu_aht: state => {
      return state.menu_aht;
    },
    get_items_array: state => {
      return state.items_array;
    },
  }
})
