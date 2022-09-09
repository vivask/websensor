import { defineStore } from 'pinia';
import { ref } from 'vue';
//import axios from 'axios'

export const useLayoutStore = defineStore('layout', {
  state: () => ({
    filter: ref('avg'),
    aht_option: ref('temperature'),
    bmx280_option: ref('temperature'),
    current_path: '/',
  }),

  getters: {
    get_filter () {
      return this.filter
    },
    get_aht_option () {
      return this.aht_option
    },
    get_bmx280_option () {
      return this.bmx280_option
    },
    get_current_path () {
      return this.current_path
    }
  },

  actions: {
    set_filter(new_value) {
      this.filter = new_value;
    },
    set_bmx280_option(new_value){
      this.bmx280_option = new_value;
    },
    set_aht_option(new_value){
      this.aht_option = new_value;
    },
    set_current_path(new_value){
      this.current_path = new_value;
    },
  },

})
