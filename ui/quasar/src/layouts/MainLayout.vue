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
            <q-item-label>Settings</q-item-label>
          </q-item-section>
        </q-item>
      </q-list>

      <q-expansion-item>
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
            <q-item-section>
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
            <q-item-section>
              <q-item-label>Humidity</q-item-label>
            </q-item-section>
            <q-item-section side>
                <q-icon name="mdi-water-percent" />
              </q-item-section>
          </q-item>
        </q-list>
      </q-expansion-item>

    </q-drawer>
    <q-footer elevated>
        <q-toolbar>
          <q-toolbar-title>&copy; VIVASK. All rights reserved.</q-toolbar-title>
          <q-spinner-bars color="primary" size="2em" v-if="isActivePeripheral"/>
        </q-toolbar>
    </q-footer>

    <q-page-container>
      <router-view />
    </q-page-container>

  </q-layout>

  <q-dialog
  v-model="waitSpinner"
  maximized
  >
        <WaitSpinner/>
  </q-dialog>

</template>

<script>
import { defineComponent, ref, computed } from 'vue'
import { useLayoutStore } from 'src/stores/layout'
import WaitSpinner from 'components/WaitSpinner.vue';
import axios from 'axios'


export default defineComponent({
  name: 'MainLayout',

  components: {
    WaitSpinner
  },

  setup () {
    const store = useLayoutStore()
    const leftDrawerOpen = ref(false)
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
      waitSpinner: computed(() => store.wait_spinner),
      activeSettings: computed(() => store.is_active_settings),
      activeAhtHumidity: computed(() => store.is_active_ath_humidity),
      activeAhtTemperature: computed(() => store.is_active_ath_temperature),

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
        const ref = "#/"+base+"?opt="+option+"&filter="+store.get_filter
        //console.log(ref)
        return ref
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
          if(response.data.aht_available){
            this.store.set_aht_available(true)
          }
          if(response.data.ds18b20_available){
            this.store.set_ds18b20_available(true)
          }
          if(response.data.bmx280_available){
            this.store.set_bmx280_available(true)
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
