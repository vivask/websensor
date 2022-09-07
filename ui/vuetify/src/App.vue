<template>
  <mobile> 

  <v-app id="inspire">
    <modal name="wait-spinner"   
      :height=0
      :width=0
      :adaptive="true"
      >
        <WaitSpinner/>
    </modal>  
    
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
              <v-icon>mdi-gauge</v-icon>
            </v-btn>
          </template>
          
          <template>
            <v-container
              class="pl-5 mt-10"
              fluid
            >
              <v-radio-group v-model="radioGroupBmx280">
                <v-radio
                  v-for="(item, index) in bmx280_options"
                  :key="index"
                  :label="`${item.title}`"
                  :value="index"
                  @click="menu_bmx280_radio_click(index)"
                ></v-radio>
              </v-radio-group>
            </v-container>
          </template>
        </v-menu> 

        <v-menu
          v-if="ahtVisible"
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
              <v-radio-group v-model="radioGroupAht">
                <v-radio
                  v-for="(item, index) in aht_options"
                  :key="index"
                  :label="`${item.title}`"
                  :value="index"
                  @click="menu_aht_radio_click(index)"
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
                  v-for="(item, index) in options"
                  :key="index"
                  :label="`${item.title}`"
                  :value="index"
                  @click="menu_radio_click(index)"
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
            @click="menu_action_click(index)"
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
          <v-container v-if="ds18b20Visible">
            <Ds18b20 ref="Ds18b20Ref"></Ds18b20>
          </v-container>
          <v-container v-if="bmx280Visible">
            <Bmx280 ref="Bmx280Ref"></Bmx280>
          </v-container>
          <v-container v-if="ahtVisible">
            <Aht ref="AhtRef"></Aht>
          </v-container>
        </template>

      </v-container>
    </v-main>
    <v-footer color="indigo accent-4" app fixed>
      <span class="white--text">&copy; VIVASK. All rights reserved.</span>
    </v-footer>
  </v-app>
  </mobile>
</template>

<script>

import WaitSpinner from './components/WaitSpinner.vue';
import Settings from  './components/Settings.vue';
import Ds18b20 from './components/Ds18b20.vue';
import Bmx280 from './components/Bmx280.vue';
import Aht from './components/Aht.vue';

export default {
  name: "App",
  components: {
    WaitSpinner,
    Settings,
    Ds18b20,
    Bmx280,
    Aht,
  },
  data() {
    return {
      menu: null,
      items: [
        { title: 'Settings', icon: 'mdi-cog', path: '/', name: 'settings' },
//        { title: 'Ds18b20', icon: 'mdi-thermometer', path: '/ds18b20', name: 'ds18b20' },
//        { title: 'Bmx280', icon: 'mdi-gauge', path: '/bmx280', name: 'bmx280' },
//        { title: 'Aht', icon: 'mdi-water-percent', path: '/aht', name: 'aht' }
],
      options: [
        { title: 'Average', name: 'avg'},
        { title: 'Minimum', name: 'min'},
        { title: 'Maximum', name: 'max'},
        { title: 'All', name: 'all'},
      ],
      bmx280_options: [
        { title: 'Temperature', name: 'temperature'},
        { title: 'Humidity', name: 'humidity'},
        { title: 'Pressure', name: 'pressure'},
      ],
      aht_options: [
        { title: 'Temperature', name: 'temperature'},
        { title: 'Humidity', name: 'humidity'},
      ],
      drawer: true,
      radioGroup: 0,
      radioGroupBmx280: 0,
      radioGroupAht: 0,
      menuIndex: 0,
      rightMenuVisible: false,
      settingsVisible: true,
      ds18b20Visible: false,
      bmx280Visible: false,
      ahtVisible: false,
      begin_idx: 0,
      end_idx: 50,
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
          this.items.push({ title: 'Bmx280', icon: 'mdi-gauge', path: '/bmx280', name: 'bmx280' });
        }
        if(response.data.aht_available){
          this.items.push({ title: 'Aht', icon: 'mdi-water-percent', path: '/aht', name: 'aht' });
        }
      })
      .catch(error => {
        console.log(error);
      });
  },
  methods: {
    router: function(route){
      //console.log('ROUTE: ', route);
      if(route === 'settings'){
        this.ds18b20Visible = false;
        this.bmx280Visible = false;
        this.rightMenuVisible = false;
        if(this.settingsVisible){
          this.$refs.SettingsRef.load_data();
        }
        this.settingsVisible = true;
        return;
      }
      if(route === 'ds18b20'){
        this.settingsVisible = false;
        this.bmx280Visible = false;
        this.ahtVisible = false;
        this.rightMenuVisible = true;
        if(this.ds18b20Visible){
          this.$refs.Ds18b20Ref.load_data();
        }
        this.ds18b20Visible = true;
        return;
      }
      if(route === 'bmx280'){
        this.settingsVisible = false;
        this.ds18b20Visible = false;
        this.ahtVisible = false;
        this.rightMenuVisible = true;
        if(this.bmx280Visible){
          this.$refs.Bmx280Ref.load_data();
        }
        this.bmx280Visible = true;
        return;
      }
      if(route === 'aht'){
        this.settingsVisible = false;
        this.ds18b20Visible = false;
        this.bmx280Visible = false;
        this.rightMenuVisible = true;
        if(this.ahtVisible){
          this.$refs.AhtRef.load_data();
        }
        this.ahtVisible = true;
        return;
      }
    },
    menu_action_click: function(index){
      this.menuIndex = index;      
      const route = this.items[index].name;
      this.router(route);
    },
    menu_radio_click: function(index){
      this.$store.commit('update_menu_filter', this.options[index].name);
      const route = this.items[this.menuIndex].name;
      this.router(route);
    },
    menu_bmx280_radio_click: function(index){
      this.$store.commit('update_menu_bmx280', this.bmx280_options[index].name);
      const route = this.items[this.menuIndex].name;
      this.router(route);
    },
    menu_aht_radio_click: function(index){
      this.$store.commit('update_menu_aht', this.aht_options[index].name);
      const route = this.items[this.menuIndex].name;
      this.router(route);
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
  .wait{
    height: 100%;
    width: 100%;
  }
</style>