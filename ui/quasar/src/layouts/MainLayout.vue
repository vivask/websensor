<template>
  <q-layout view="lHh Lpr lFf">
    <q-header elevated>
      <q-toolbar>
        <q-btn flat dense round icon="mdi-menu" class="q-mr-sm" @click="toggleLeftDrawer" />
        <q-toolbar-title>Websensor</q-toolbar-title>

        <q-btn flat round dense icon="mdi-dots-vertical" v-if="rightMenuVisible" >
          <q-menu>
            <q-list>
              <q-item tag="label" v-ripple>
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
              <q-item tag="label" v-ripple>
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
              <q-item tag="label" v-ripple>
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
              <q-item tag="label" v-ripple>
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

        <q-btn flat round dense icon="mdi-water-percent" v-if="ahtMenuVisible">
          <q-menu>
            <q-list>
              <q-item tag="label" v-ripple>
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
              <q-item tag="label" v-ripple>
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
              :active="this.store.isSelected"
              v-ripple
              tag="a"
              target="_blank"
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


    <q-page-container>
      <router-view />
    </q-page-container>
  </q-layout>
</template>

<script>
import { defineComponent, ref } from 'vue'
import { useLayoutStore } from 'stores/layout'



const linksList = [
  {
    title: 'Settings',
    icon: 'mdi-cog',
    color: 'blue',
    link: '/',
    clicked: true,
    separator: true
  },
  {
    title: 'Ds18b20',
    icon: 'mdi-thermometer',
    color: 'blue',
    link: '#/ds18b20',
    clicked: false,
    separator: false
  },
  {
    title: 'Bmx280',
    icon: 'mdi-gauge',
    color: 'blue',
    link: '#/bmx280',
    clicked: false,
    separator: false
  },
  {
    title: 'Aht',
    icon: 'mdi-water-percent',
    color: 'blue',
    link: '#/aht',
    clicked: false,
    separator: false
  }
]


export default defineComponent({
  name: 'MainLayout',

  setup () {
    const store = useLayoutStore()
    const leftDrawerOpen = ref(false)
    return {
      store,
      menuList: linksList,
      leftDrawerOpen,
      toggleLeftDrawer () {
        leftDrawerOpen.value = !leftDrawerOpen.value
      },
      rightMenuVisible: true,
      ahtMenuVisible: true,
      mainFilter: ref('avg'),
      ahtOption: ref('temperature'),
    }
  },
  methods: {
    triggerMainOptions () {
      //this.store.set_filter(this.mainFilter);
      //console.log(EssentialLink.get_title())
    },
    triggerAhtOptions () {
      //this.store.set_aht_option(this.ahtOption);
      console.log(this.store.get_current_path)
    },
    menuClick (index) {
      this.store.set_current_path(this.menuList[index].link)
      //this.$router.push(this.menuList[index].link)
    }
  }
})
</script>
