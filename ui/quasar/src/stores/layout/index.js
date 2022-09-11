import { defineStore } from 'pinia'
import { ref } from 'vue'
import { useAhtStore } from 'src/stores/aht'
import { useRoute } from 'vue-router'

export const useLayoutStore = defineStore('layout', {
  state: () => ({
    filter: ref('avg'),
    aht_option: ref('temperature'),
    bmx280_option: ref('temperature'),
    current_path: '/',
    wait_spinner: false,
    is_sensor_page: false,
    is_aht_page: false,
    is_ds18b20_page: false,
    is_bmx280_page: false,
    aht_is_available: false,
    ds18b20_is_available: false,
    bmx280_is_available: false,
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
    },
    get_first_available_page() {
      if (this.aht_is_available) {
        return '/aht'
      }
      if (this.ds18b20_is_available) {
        return '/ds18b20'
      }
      if (this.ds18b20_is_available) {
        return '/bmx280'
      }
      return '/'
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
    set_current_path(new_value) {
      this.current_path = new_value;
      switch(new_value){
        case '/':
          this.is_sensor_page = false
          this.is_aht_page = false
          this.is_ds18b20_page = false
          this.is_bmx280_page = false
          break
        case '/aht':
          this.is_sensor_page = true
          this.is_aht_page = true
          this.is_ds18b20_page = false
          this.is_bmx280_page = false
          break
        case '/ds18b20':
          this.is_sensor_page = true
          this.is_aht_page = false
          this.is_ds18b20_page = true
          this.is_bmx280_page = false
          break
        case '/bmx280':
          this.is_sensor_page = true
          this.is_aht_page = false
          this.is_ds18b20_page = false
          this.is_bmx280_page = true
          break
      }
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
    },
    is_selected_menu (title) {
      var result = false
      switch(title){
        case 'Settings':
          result = (this.current_path == '/')
          break
        case 'Ds18b20':
          result = (this.current_path == '/ds18b20')
          break
        case 'Bmx280':
          result = (this.current_path == '/bmx280')
          break
        case 'Aht':
          result = (this.current_path == '/aht')
          break
      }
      return result
    },
    set_aht_available (new_value) {
      this.aht_is_available = new_value;
    },
    set_ds18b20_available (new_value) {
      this.ds18b20_is_available = new_value;
    },
    set_bmx280_available (new_value) {
      this.bmx280_available = new_value;
    },
  },

})
