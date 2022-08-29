<template>
  <v-app id="inspire">
    
    <v-app-bar color="indigo accent-4" elevation="4" app clipped-left flat dark>
        <v-app-bar-nav-icon @click.stop="drawer = !drawer"></v-app-bar-nav-icon>
        <v-app-bar-title>Websensor</v-app-bar-title>
        <v-spacer></v-spacer>
        <v-btn icon v-if="visible" @click="showChart()">
          <v-icon>mdi-chart-line</v-icon>
        </v-btn>
        <v-menu
          v-if="visible"
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
        <router-view></router-view>
      </v-container>
    </v-main>
    <v-footer color="indigo accent-4" app fixed>
      <span class="white--text">&copy; VIVASK. All rights reserved.</span>
    </v-footer>
  </v-app>
</template>

<script>
import Ds18b20Ref from './components/Ds18b20.vue'
export default {
  components: { 'foo': Ds18b20Ref },
  name: "App",
  data() {
    return {
      menu: null,
      items: [
        { title: 'Settings', icon: 'mdi-cog', path: '/', name: 'settings' },
//        { title: 'Ds18b20', icon: 'mdi-thermometer', path: '/ds18b20', name: 'ds18b20' },
//        { title: 'Bmx280', icon: 'mdi-water-percent', path: '/bmx280', name: 'bmx280' }
      ],
      options: [
        { title: 'All'},
        { title: 'Average'},
        { title: 'Minimum'},
        { title: 'Maximum'},
      ],
      drawer: true,
      miniVariant: false,
      visible: false,
      radioGroup: 0,
      menuIndex: 0,
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
      })
      .catch(error => {
        console.log(error);
      });
  },
  methods: {
    menuActionClick(index){
      this.menuIndex = index;
      const path = this.items[this.menuIndex].path;
      if(path == '/'){
        this.visible = false;  
        this.$router.push(path);
      }else{
        this.visible = true;
        this.$router.push(this.getRoute(path));
      }
    },
    showChart(){
      if(this.$route.name == 'ds18b20') {
        this.$router.push('/ds18b20/chart');
        return;
      }
      if(this.$route.name == 'bmx280') {
        this.$router.push('/bmx280/chart');
            return;
      }
    },
    menuRadioClick(){
      const path = this.items[this.menuIndex].path;
      if( path != '/' ){
        switch(path){
          case '/ds18b20':
            this.foo.loadItems(this.getFilter());
        }
      } 
    },
    getRoute(path){
      return {path: path, query : {filter: this.getFilter()}};
    },        
    getFilter(){
      switch(this.radioGroup){
        case 0:
          return 'all';
        case 1:
          return 'avg';
        case 2:
          return 'min';
        case 3:
          return 'max';
      }
    },        
  }
};
</script>