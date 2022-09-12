<template>
  <q-layout view="lHh Lpr lFf">
    <q-header elevated>
      <q-toolbar>
        <q-btn flat dense round icon="mdi-menu" class="q-mr-sm" @click="toggleLeftDrawer" />
        <q-toolbar-title>Websensor</q-toolbar-title>

        <q-btn flat round dense icon="mdi-apps" v-if="ahtMenuVisible">
          <q-menu>
            <q-list>
              <q-item tag="label" v-close-popup>
                <q-item-section avatar>
                  <q-radio
                  v-model="ahtOption"
                  val="temperature"
                  checked-icon="task_alt"
                  unchecked-icon="panorama_fish_eye"
                  @click="triggerAhtOptions" />
                </q-item-section>
                <q-item-section>
                  <q-item-label>Temperature</q-item-label>
                </q-item-section>
              </q-item>
              <q-item tag="label" v-close-popup>
                <q-item-section avatar>
                  <q-radio
                  v-model="ahtOption"
                  val="humidity"
                  checked-icon="task_alt"
                  unchecked-icon="panorama_fish_eye"
                  @click="triggerAhtOptions" />
                </q-item-section>
                <q-item-section>
                  <q-item-label>Humidity</q-item-label>
                </q-item-section>
              </q-item>
            </q-list>
          </q-menu>
        </q-btn>

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

      <q-scroll-area class="fit">
      <q-list>
        <q-item-label
          header
        >
          Menu
        </q-item-label>

            <template v-for="(menuItem, index) in menuList" :key="index">
              <q-item
              clickable
              :active="is_selected_menu(menuItem.title)"
              v-ripple
              :href="menuItem.link"
              >
                <q-item-section avatar>
                  <q-icon :name="menuItem.icon" :color="menuItem.color" />
                </q-item-section>
                <q-item-section>
                  {{ menuItem.title }}
                </q-item-section>
              </q-item>
              <q-separator :key="'sep' + index" v-if="menuItem.separator" />
            </template>

      </q-list>
    </q-scroll-area>


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


const linksList = [
  {
    title: 'Settings',
    icon: 'mdi-cog',
    color: 'blue',
    link: '#/',
    separator: true
  },
  //{ title: 'Aht', icon: 'mdi-water-percent', color: "blue", link: '#/aht', separator: false },
  //{ title: 'Ds18b20', icon: 'mdi-thermometer', color: "blue", link: '#/ds18b20', separator: false },
  //{ title: 'Bmx280', icon: 'mdi-gauge', color: "blue", link: '#/bmx280', separator: false }
]


export default defineComponent({
  name: 'MainLayout',

  components: {
    WaitSpinner
  },

  setup () {
    const store = useLayoutStore()
    const leftDrawerOpen = ref(false)
    const mainFilter = ref('avg')
    const ahtOption = ref('temperature')
    const timer = ref(null)

    return {
      store,
      menuList: linksList,
      leftDrawerOpen,
      toggleLeftDrawer () {
        leftDrawerOpen.value = !leftDrawerOpen.value
      },
      rightMenuVisible: computed(() => store.is_sensor_page),
      ahtMenuVisible: computed(() => store.is_aht_page),
      mainFilter,
      ahtOption,
      waitSpinner: computed(() => store.wait_spinner),
      triggerMainOptions () {
        store.set_filter(mainFilter);
      },
      triggerAhtOptions () {
        store.set_aht_option(ahtOption);
      },
      is_selected_menu: function(title) {
        return store.is_selected_menu(title)
      },
      isActivePeripheral: computed(() => store.get_peripheral_status),
      timer,
    }
  },
  methods: {
    update_peripheral_status: function() {
      this.store.update_peripheral_status()
    }
  },
  mounted () {
    axios.get("/api/v1/settings/info")
        .then(response => {
          if(response.data.aht_available){
            this.store.set_aht_available(true)
            this.menuList.push({ title: 'Aht', icon: 'mdi-water-percent', color: "blue", link: '#/aht', separator: false });
          }
          if(response.data.ds18b20_available){
            this.store.set_ds18b20_available(true)
            this.menuList.push({ title: 'Ds18b20', icon: 'mdi-thermometer', color: "blue", link: '#/ds18b20', separator: false });
          }
          if(response.data.bmx280_available){
            this.store.set_bmx280_available(true)
            this.menuList.push({ title: 'Bmx280', icon: 'mdi-gauge', color: "blue", link: '#/bmx280', separator: false });
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
