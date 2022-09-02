<template>
  <v-app id="inspire">
    
    <v-app-bar color="indigo accent-4" elevation="4" app clipped-left flat dark>
        <v-app-bar-nav-icon @click.stop="drawer = !drawer"></v-app-bar-nav-icon>
        <v-app-bar-title>Websensor</v-app-bar-title>
        <v-spacer></v-spacer>
        <v-menu
          v-if="bmx280Visible"
          left
          bottom
        >
          <template v-slot:activator="{ on, attrs }">
            <v-btn
              icon
              v-bind="attrs"
              v-on="on"
            >
              <v-icon>mdi-water-percent</v-icon>
            </v-btn>
          </template>
          
          <template>
            <v-container
              class="pl-5 mt-10"
              fluid
            >
              <v-radio-group v-model="radioGroupBmx280">
                <v-radio
                  v-for="(option, index) in bmx280_options"
                  :key="index"
                  :label="`${option.title}`"
                  :value="index"
                  @click="menuBmx280RadioClick()"
                ></v-radio>
              </v-radio-group>
            </v-container>
          </template>
        </v-menu>                
        <v-menu
          v-if="rightMenuVisible"
          left
          bottom
        >
          <template v-slot:activator="{ on, attrs }">
            <v-btn
              icon
              v-bind="attrs"
              v-on="on"
            >
              <v-icon>mdi-dots-vertical</v-icon>
            </v-btn>
          </template>
          
          <template>
            <v-container
              class="pl-5 mt-10"
              fluid
            >
              <v-radio-group v-model="radioGroup">
                <v-radio
                  v-for="(option, index) in options"
                  :key="index"
                  :label="`${option.title}`"
                  :value="index"
                  @click="menuRadioClick()"
                ></v-radio>
              </v-radio-group>
            </v-container>
          </template>
        </v-menu>        
    </v-app-bar>

    <v-navigation-drawer v-model="drawer" fixed app clipped>
        <v-divider></v-divider>

        <v-list
          dense
          nav
        >
          <v-list-item
            v-for="(item, index) in items"
            :key="index"
            @click="menuActionClick(index)"
          >
            <v-list-item-icon>
              <v-icon>{{ item.icon }}</v-icon>
            </v-list-item-icon>

            <v-list-item-content>
              <v-list-item-title>{{ item.title }}</v-list-item-title>
            </v-list-item-content>
          </v-list-item>
        </v-list>
    </v-navigation-drawer>

    <v-main>
      <v-container fluid fill-height>
        
        <template>
          <v-container v-if="settingsVisible">
            <Settings ref="SettingsRef"></Settings>
          </v-container>
        </template>

        <template>
          <v-container v-if="ds18b20Visible">
            <Ds18b20 ref="Ds18b20Ref"></Ds18b20>
          </v-container>
        </template>

        <template>
          <v-container v-if="bmx280Visible">
            <Bmx280 ref="Bmx280Ref"></Bmx280>
          </v-container>
        </template>

        <template>
          <v-container v-if="wait">
            <Wait/>
          </v-container>
        </template>

      </v-container>
    </v-main>
    <v-footer color="indigo accent-4" app fixed>
      <span class="white--text">&copy; VIVASK. All rights reserved.</span>
    </v-footer>
  </v-app>
  
</template>

<script>

import Wait from './components/Wait.vue';
import Settings from  './components/Settings.vue';
import Ds18b20 from './components/Ds18b20.vue';
import Bmx280 from './components/Bmx280.vue';

export default {
  name: "App",
  components: {
    Wait,
    Settings,
    Ds18b20,
    Bmx280,
},
  data() {
    return {
      menu: null,
      items: [
        { title: 'Settings', icon: 'mdi-cog', path: '/', name: 'settings' },
//        { title: 'Ds18b20', icon: 'mdi-thermometer', path: '/ds18b20', name: 'ds18b20' },
//        { title: 'Bmx280', icon: 'mdi-water-percent', path: '/bmx280', name: 'bmx280' }
      ],
      options: [
        { title: 'Average'},
        { title: 'Minimum'},
        { title: 'Maximum'},
        { title: 'All'},
      ],
      bmx280_options: [
        { title: 'Temperature'},
        { title: 'Humidity'},
        { title: 'Pressure'},
      ],
      drawer: true,
      miniVariant: false,
      radioGroup: 0,
      radioGroupBmx280: 0,
      menuIndex: 0,
      rightMenuVisible: false,
      settingsVisible: true,
      settingsMounted: false,
      ds18b20Visible: false,
      ds18b20Mounted: false,
      bmx280Visible: false,
      bmx280Mounted: false,
      begin_idx: 0,
      end_idx: 50,
      wait: false,
    };
  },
  mounted() {
    this.$ajax
      .get("/api/v1/settings/info")
      .then(response => {
        if(response.data.ds18b20_available){
          this.items.push({ title: 'Ds18b20', icon: 'mdi-thermometer', path: '/ds18b20', name: 'ds18b20' });
        }
        if(response.data.bmx280_available){
          this.items.push({ title: 'Bmx280', icon: 'mdi-water-percent', path: '/bmx280', name: 'bmx280' });
        }
        this.settingsMounted = true;
      })
      .catch(error => {
        console.log(error);
      });
  },
  methods: {
    router: function(route){
      //this.wait = true;
      if(route === 'settings'){
        console.log(route);
        this.ds18b20Visible = false;
        this.bmx280Visible = false;
        this.rightMenuVisible = false;
        this.settingsVisible = true;
        if(this.settingsMounted){
          this.$refs.SettingsRef.load_data();
        }else{
          this.settingsMounted = true;  
        }
        this.ds18b20Mounted = false;
        this.bmx280Mounted = false;
        this.wait = false;
        return;
      }
      if(route === 'ds18b20'){
        console.log(route);
        this.settingsVisible = false;
        this.bmx280Visible = false;
        this.rightMenuVisible = true;
        this.ds18b20Visible = true;
        if(this.ds18b20Mounted){
          this.$refs.Ds18b20Ref.load_data(this.get_filter());
        }else{
          this.ds18b20Mounted = true;
        }
        this.settingsMounted = false;
        this.bmx280Mounted = false; 
        this.wait = false;
        return;
      }
      if(route === 'bmx280'){
        console.log(route);
        this.settingsVisible = false;
        this.ds18b20Visible = false;
        this.rightMenuVisible = true;
        this.bmx280Visible = true;
        if(this.bmx280Mounted){
          this.$refs.Bmx280Ref.load_data(this.get_filter(), this.get_bmx280_option());
        }else{
          this.bmx280Mounted = true;
        }
        this.settingsMounted = false;
        this.ds18b20Mounted = false;
        this.wait = false;
        return;
      }
    },
    menuActionClick(index){
      this.menuIndex = index;
      const route = this.items[index].name;
      this.router(route);
    },
    menuRadioClick(){
      const route = this.items[this.menuIndex].name;
      this.router(route);
    },
    menuBmx280RadioClick(){
      const route = this.items[this.menuIndex].name;
      this.router(route);
    },
    get_filter(){
      switch(this.radioGroup){
        case 0:
          return 'avg';
        case 1:
          return 'min';
        case 2:
          return 'max';
        case 3:
          return 'all';
      }
    },
    get_bmx280_option(){
      switch(this.radioGroupBmx280){
        case 0:
          return 'temperature';
        case 1:
          return 'humidity';
        case 2:
          return 'pressure';
      }
    },     

  },
};
</script>

<style>
  #app {
    font-family: "Avenir", Helvetica, Arial, sans-serif;
    -webkit-font-smoothing: antialiased;
    -moz-osx-font-smoothing: grayscale;
    text-align: center;
    color: #2c3e50;
    margin-top: 60px;
  }
</style>