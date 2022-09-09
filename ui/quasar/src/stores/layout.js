import { defineStore } from 'pinia';
import { ref } from 'vue';
import { useAhtStore } from 'stores/aht';

export const useLayoutStore = defineStore('layout', {
  state: () => ({
    filter: ref('avg'),
    aht_option: ref('temperature'),
    bmx280_option: ref('temperature'),
    current_path: '/',
    wait_spinner: false,
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
      this.load_data()
    },
    set_bmx280_option(new_value){
      this.bmx280_option = new_value;
      this.load_data()
    },
    set_aht_option(new_value){
      this.aht_option = new_value;
      this.load_data()
    },
    set_current_path(new_value){
      this.current_path = new_value;
    },
    load_data () {
      switch(this.current_path){
        case '/':
          break
        case '/aht':
          const store = useAhtStore()
          store.update_items_array()
          break
        case '/ds18b20':
          break
        case '/bmx280':
          break
      }
    },
    wait_spinner_show () {
      this.wait_spinner = true;
    },
    wait_spinner_hide () {
      this.wait_spinner = false;
    }
  },

})
