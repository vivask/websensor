<template>
  <q-layout view="lHh Lpr lFf">
    <q-header elevated>
      <q-toolbar>
        <q-btn flat dense round icon="mdi-menu" class="q-mr-sm" @click="toggleLeftDrawer" />
        <q-toolbar-title>Websensor</q-toolbar-title>

        <q-btn
          to="/aht"
          flat
          icon-right="account_circle"
        />

        <q-btn flat round dense icon="mdi-dots-vertical" v-if="rightMenuVisible" >
          <q-menu>
            <q-list>
              <q-item tag="label" v-ripple>
                <q-item-section avatar>
                  <q-radio
                  v-model="mainOptionsGroup"
                  val="avg"
                  checked-icon="task_alt"
                  unchecked-icon="panorama_fish_eye"
                  @click="triggerMainOptions" />
                </q-item-section>
                <q-item-section>
                  <q-item-label>Average</q-item-label>
                </q-item-section>
              </q-item>
              <q-item tag="label" v-ripple>
                <q-item-section avatar>
                  <q-radio
                  v-model="mainOptionsGroup"
                  val="min"
                  checked-icon="task_alt"
                  unchecked-icon="panorama_fish_eye"
                  @click="triggerMainOptions" />
                </q-item-section>
                <q-item-section>
                  <q-item-label>Minimum</q-item-label>
                </q-item-section>
              </q-item>
              <q-item tag="label" v-ripple>
                <q-item-section avatar>
                  <q-radio
                  v-model="mainOptionsGroup"
                  val="max"
                  checked-icon="task_alt"
                  unchecked-icon="panorama_fish_eye"
                  @click="triggerMainOptions" />
                </q-item-section>
                <q-item-section>
                  <q-item-label>Maximum</q-item-label>
                </q-item-section>
              </q-item>
              <q-item tag="label" v-ripple>
                <q-item-section avatar>
                  <q-radio
                  v-model="mainOptionsGroup"
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

        <q-btn flat round dense icon="mdi-water-percent" v-if="ahtMenuVisible">
          <q-menu>
            <q-list>
              <q-item tag="label" v-ripple>
                <q-item-section avatar>
                  <q-radio
                  v-model="ahtOptionsGroup"
                  val="temperature"
                  checked-icon="task_alt"
                  unchecked-icon="panorama_fish_eye"
                  @click="triggerAhtOptions" />
                </q-item-section>
                <q-item-section>
                  <q-item-label>Temperature</q-item-label>
                </q-item-section>
              </q-item>
              <q-item tag="label" v-ripple>
                <q-item-section avatar>
                  <q-radio
                  v-model="ahtOptionsGroup"
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

      </q-toolbar>
    </q-header>

    <q-drawer
      v-model="leftDrawerOpen"
      show-if-above
      bordered
    >
      <q-list>
        <q-item-label
          header
        >
          Essential Links
        </q-item-label>

        <EssentialLink
          v-for="link in essentialLinks"
          :key="link.title"
          v-bind="link"
        />
      </q-list>
    </q-drawer>

    <q-page-container>
      <router-view />
    </q-page-container>
  </q-layout>
</template>

<script>
import { defineComponent, ref } from 'vue'
import EssentialLink from 'components/EssentialLink.vue'


const linksList = [
  { title: 'Settings', icon: 'mdi-cog', link: '/', name: 'settings', visible: true },
  { title: 'Ds18b20', icon: 'mdi-thermometer', link: '#/ds18b20', name: 'ds18b20', visible: false },
  { title: 'Bmx280', icon: 'mdi-gauge', link: '#/bmx280', name: 'bmx280', visible: false },
  { title: 'Aht', icon: 'mdi-water-percent', link: '#/aht', name: 'aht' }
]


export default defineComponent({
  name: 'MainLayout',

  components: {
    EssentialLink
  },

  setup () {
    const leftDrawerOpen = ref(false)

    return {
      essentialLinks: linksList,
      leftDrawerOpen,
      toggleLeftDrawer () {
        leftDrawerOpen.value = !leftDrawerOpen.value
      },
      rightMenuVisible: false,
      ahtMenuVisible: false,
      mainOptionsGroup: ref('avg'),
      ahtOptionsGroup: ref('temperature'),
    }
  },
  methods: {
    triggerMainOptions () {
      console.log("triggered", this.mainOptionsGroup)
    },
    triggerAhtOptions () {
      console.log("triggered", this.ahtOptionsGroup)
    }
  }
})
</script>
