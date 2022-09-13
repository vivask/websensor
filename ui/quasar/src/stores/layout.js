import { defineStore } from 'pinia'
import { ref } from 'vue'
import { useAhtStore } from 'src/stores/aht'
import { useBmx280Store } from 'src/stores/bmx280'
import { useDs18b20Store } from 'src/stores/ds18b20'
import axios from 'axios'

export const useLayoutStore = defineStore('layout', {
  state: () => ({
    filter: ref('avg'),
    current_path: ref('/'),
    selected_menu: ref('Settings'),
    selected_submenu: ref(''),
    load_spinner: false,
    gear_spinner: false,
    is_sensor_page: false,
    is_aht_page: false,
    is_ds18b20_page: false,
    is_bmx280_page: false,
    aht_is_available: true,
    ds18b20_is_available: true,
    bmx280_is_available: true,
    peripheral_status: false
  }),

  getters: {
    get_filter () {
      return this.filter
    },
    is_active_settings () {
      return (this.selected_menu+this.selected_submenu) == ('Settings')
    },
    is_active_ath_humidity () {
      return (this.selected_menu+this.selected_submenu) == ('AHT25'+'humidity')
    },
    is_active_ath_temperature () {
      return (this.selected_menu+this.selected_submenu) == ('AHT25'+'temperature')
    },
    is_active_bmx280_temperature () {
      return (this.selected_menu+this.selected_submenu) == ('BME280'+'temperature')
    },
    is_active_bmx280_humidity () {
      return (this.selected_menu+this.selected_submenu) == ('BME280'+'humidity')
    },
    is_active_bmx280_pressusre () {
      return (this.selected_menu+this.selected_submenu) == ('BME280'+'pressusre')
    },
    is_active_ds18b20_temperature () {
      return (this.selected_menu+this.selected_submenu) == ('DS18B20')
    },
    get_selected_menu () {
      return (this.selected_menu+this.selected_submenu)
    },
    get_first_available_page () {
      if (this.aht_is_available) {
        return '#/aht?opt=temperature'
      }
      if (this.ds18b20_is_available) {
        return '#/ds18b20'
      }
      if (this.ds18b20_is_available) {
        return '#/bmx280?opt=temperature'
      }
      return '/'
    },
    get_peripheral_status () {
      return this.peripheral_status
    }
  },

  actions: {
    set_filter(new_value) {
      this.filter = new_value
      this.load_data()
    },
    set_current_path(new_value){
      //console.log("Current path: ", new_value)
      this.current_path = new_value
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
    set_selected_menu(new_value){
      this.selected_menu = new_value
    },
    set_selected_submenu(new_value){
      this.selected_submenu = new_value
    },
    load_data () {
      //console.log("CURRENT PATH: ",this.current_path)
      switch(this.current_path){
        case '/':
          break
        case '/aht':
          useAhtStore().update_items_array()
          break
        case '/ds18b20':
          useDs18b20Store().update_items_array()
          break
        case '/bmx280':
          useBmx280Store().update_items_array()
          break
      }
    },
    load_spinner_show () {
      this.load_spinner = true;
    },
    load_spinner_hide () {
      this.load_spinner = false;
    },
    gear_spinner_show () {
      this.gear_spinner = true;
    },
    gear_spinner_hide () {
      this.gear_spinner = false;
    },
    set_aht_available (new_value) {
      this.aht_is_available = new_value
    },
    set_ds18b20_available (new_value) {
      this.ds18b20_is_available = new_value
    },
    set_bmx280_available (new_value) {
      this.bmx280_is_available = new_value
    },
    update_peripheral_status () {
      axios.get("/api/v1/peripheral/info")
          .then(response => {
            this.peripheral_status = response.data.peripheral_is_active
          })
          .catch(error => {
            console.log(error);
          });
    }
  },
})
