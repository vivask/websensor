<template>
  <q-layout view="lHh Lpr lFf">
    <q-header elevated>
      <q-toolbar>
        <q-btn flat dense round icon="mdi-menu" class="q-mr-sm" @click="toggleLeftDrawer" />
        <q-toolbar-title>Websensor</q-toolbar-title>

        <q-btn flat round dense icon="mdi-dots-vertical" v-if="rightMenuVisible" >
          <q-menu>
            <q-list>
              <q-item tag="label" v-close-popup>
                <q-item-section avatar>
                  <q-radio
                  v-model="mainFilter"
                  val="avg"
                  checked-icon="task_alt"
                  unchecked-icon="panorama_fish_eye"
                  @click="triggerMainOptions" />
                </q-item-section>
                <q-item-section>
                  <q-item-label>Average</q-item-label>
                </q-item-section>
              </q-item>
              <q-item tag="label" v-close-popup>
                <q-item-section avatar>
                  <q-radio
                  v-model="mainFilter"
                  val="min"
                  checked-icon="task_alt"
                  unchecked-icon="panorama_fish_eye"
                  @click="triggerMainOptions" />
                </q-item-section>
                <q-item-section>
                  <q-item-label>Minimum</q-item-label>
                </q-item-section>
              </q-item>
              <q-item tag="label" v-close-popup>
                <q-item-section avatar>
                  <q-radio
                  v-model="mainFilter"
                  val="max"
                  checked-icon="task_alt"
                  unchecked-icon="panorama_fish_eye"
                  @click="triggerMainOptions" />
                </q-item-section>
                <q-item-section>
                  <q-item-label>Maximum</q-item-label>
                </q-item-section>
              </q-item>
              <q-item tag="label" v-close-popup>
                <q-item-section avatar>
                  <q-radio
                  v-model="mainFilter"
                  val="all"
                  checked-icon="task_alt"
                  unchecked-icon="panorama_fish_eye"
                  @click="triggerMainOptions" />
                </q-item-section>
                <q-item-section>
                  <q-item-label>All</q-item-label>
                </q-item-section>
              </q-item>
            </q-list>
          </q-menu>
        </q-btn>

      </q-toolbar>
    </q-header>

    <q-drawer
      v-model="leftDrawerOpen"
      show-if-above
      bordered
      :width="200"
      class="bg-grey-3"
    >

      <q-list>
        <q-item-label header >Menu</q-item-label>

        <q-item
        v-model="menuSettings"
        active-class="menu-item"
        clickable
        v-close-popup
        href="#/"
        :active="activeSettings"
        @click="setMenu('Settings', '')"
        >
          <q-item-section side>
            <q-icon name="mdi-cog" color="blue"/>
          </q-item-section>
          <q-item-section>
            <q-item-label  class="ml-15">Settings</q-item-label>
          </q-item-section>
        </q-item>
      </q-list>

      <q-expansion-item default-opened v-if="ahtActive">
        <template v-slot:header>
          <q-item-section avatar>
            <q-icon color="blue" name="sensors" />
          </q-item-section>
          <q-item-section>
            AHT25
          </q-item-section>
        </template>
        <q-list>
          <q-item
          active-class="menu-item"
          clickable
          v-close-popup
          :active="activeAhtTemperature"
          :href="getRef('aht', 'temperature')"
          @click="setMenu('AHT25', 'temperature')"
          >
            <q-item-section class="ml-20">
              <q-item-label>Temperature</q-item-label>
            </q-item-section>
            <q-item-section side>
                <q-icon name="mdi-thermometer" />
              </q-item-section>
          </q-item>
          <q-item
          active-class="menu-item"
          clickable
          v-close-popup
          :active="activeAhtHumidity"
          :href="getRef('aht', 'humidity')"
          @click="setMenu('AHT25', 'humidity')"
          >
            <q-item-section class="ml-20">
              <q-item-label>Humidity</q-item-label>
            </q-item-section>
            <q-item-section side>
                <q-icon name="mdi-water-percent" />
              </q-item-section>
          </q-item>
        </q-list>
      </q-expansion-item>

      <q-expansion-item default-opened v-if="bmx280Active">
        <template v-slot:header>
          <q-item-section avatar>
            <q-icon color="blue" name="sensors" />
          </q-item-section>
          <q-item-section>
            BME280
          </q-item-section>
        </template>
        <q-list>
          <q-item
          active-class="menu-item"
          clickable
          v-close-popup
          :active="activeBmx280Temperature"
          :href="getRef('bmx280', 'temperature')"
          @click="setMenu('BME280', 'temperature')"
          >
            <q-item-section class="ml-20">
              <q-item-label>Temperature</q-item-label>
            </q-item-section>
            <q-item-section side>
                <q-icon name="mdi-thermometer" />
              </q-item-section>
          </q-item>
          <q-item
          active-class="menu-item"
          clickable
          v-close-popup
          :active="activeBmx280Humidity"
          :href="getRef('bmx280', 'humidity')"
          @click="setMenu('BME280', 'humidity')"
          >
            <q-item-section class="ml-20">
              <q-item-label>Humidity</q-item-label>
            </q-item-section>
            <q-item-section side>
                <q-icon name="mdi-water-percent" />
              </q-item-section>
          </q-item>
          <q-item
          active-class="menu-item"
          clickable
          v-close-popup
          :active="activeBmx280Pressure"
          :href="getRef('bmx280', 'pressure')"
          @click="setMenu('BME280', 'pressure')"
          >
            <q-item-section class="ml-20">
              <q-item-label>Pressusre</q-item-label>
            </q-item-section>
            <q-item-section side>
                <q-icon name="tire_repair" />
              </q-item-section>
          </q-item>
        </q-list>
      </q-expansion-item>

      <q-expansion-item default-opened v-if="ds18b20Active">
        <template v-slot:header>
          <q-item-section avatar>
            <q-icon color="blue" name="sensors" />
          </q-item-section>
          <q-item-section>
            DS18B20
          </q-item-section>
        </template>
        <q-list>
          <q-item
          active-class="menu-item"
          clickable
          v-close-popup
          :active="activeDs18b20Temperature"
          :href="getRef('ds18b20', 'temperature')"
          @click="setMenu('DS18B20', 'temperature')"
          >
            <q-item-section class="ml-20">
              <q-item-label>Temperature</q-item-label>
            </q-item-section>
            <q-item-section side>
                <q-icon name="mdi-thermometer" />
              </q-item-section>
          </q-item>
        </q-list>
      </q-expansion-item>

    </q-drawer>
    <q-footer elevated>
        <q-toolbar>
          <q-spinner-bars color="primary" size="2em" v-if="isActivePeripheral"/>
          <q-toolbar-title>&copy; VIVASK. All rights reserved.</q-toolbar-title>
        </q-toolbar>
    </q-footer>

    <q-page-container>
      <router-view />
    </q-page-container>

  </q-layout>

  <q-dialog
  v-model="loadInner"
  maximized
  >
    <LoadInner/>
  </q-dialog>

  <q-dialog
  v-model="gearInner"
  maximized
  >
    <GearInner/>
  </q-dialog>


</template>

<script>
import { defineComponent, ref, computed } from 'vue'
import { useLayoutStore } from 'src/stores/layout'
import LoadInner from 'components/LoadInner.vue';
import GearInner from 'components/GearInner.vue';
import axios from 'axios'


export default defineComponent({
  name: 'MainLayout',

  components: {
    LoadInner,
    GearInner
},

  setup () {
    const store = useLayoutStore()
    const leftDrawerOpen = ref(false)
    const menuSettings = ref(false)
    const mainFilter = ref('avg')
    const timer = ref(null)

    return {
      store,
      leftDrawerOpen,
      toggleLeftDrawer () {
        leftDrawerOpen.value = !leftDrawerOpen.value
      },
      rightMenuVisible: computed(() => store.is_sensor_page),
      mainFilter,
      ahtActive: computed(() => store.aht_is_available),
      ds18b20Active: computed(() => store.ds18b20_is_available),
      bmx280Active: computed(() => store.bmx280_is_available),
      loadInner: computed(() => store.load_spinner),
      gearInner: computed(() => store.gear_spinner),
      activeSettings: computed(() => store.is_active_settings),
      activeAhtHumidity: computed(() => store.is_active_ath_humidity),
      activeAhtTemperature: computed(() => store.is_active_ath_temperature),
      activeBmx280Temperature: computed(() => store.is_active_bmx280_temperature),
      activeBmx280Humidity: computed(() => store.is_active_bmx280_humidity),
      activeBmx280Pressure: computed(() => store.is_active_bmx280_pressusre),
      activeDs18b20Temperature: computed(() => store.is_active_ds18b20_temperature),
      menuSettings,
      triggerMainOptions () {
        store.set_filter(mainFilter)
      },
      isActivePeripheral: computed(() => store.get_peripheral_status),
      timer,
      setMenu(menu, submenu) {
        store.set_selected_menu(menu)
        store.set_selected_submenu(submenu)
        if(menu != 'Settings'){
          store.set_filter(mainFilter)
        }
      },
      isActiveMenu(menu, submenu) {
        return this.store.get_selected_menu == (menu+submenu)
      },
      getRef(base, option) {
        if (option != null){
          return "#/"+base+"?opt="+option+"&filter="+store.get_filter
        }
        return "#/"+base+"?filter="+store.get_filter
      }
    }
  },
  methods: {
    update_peripheral_status: function() {
      this.store.update_peripheral_status()
    },
  },
  mounted () {
    axios.get("/api/v1/settings/info")
        .then(response => {
          if( !response.data.aht_available ){
            this.store.set_aht_available(false)
          }
          if( !response.data.ds18b20_available ){
            this.store.set_ds18b20_available(false)
          }
          if( !response.data.bmx280_available ){
            this.store.set_bmx280_available(false)
          }
          if( this.store.get_first_available_page != '/' ) {
            clearInterval(this.timer)
            this.timer = setInterval(this.update_peripheral_status, 1000)
          }
        })
        .catch(error => {
          console.log(error);
        });
  },
})
</script>
