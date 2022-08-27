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
  
          <v-list>
            <v-list-item
              v-for="(option, index) in options"
              :key="index"
              @click="() => {}"
            >
              <v-list-item-action>
                <v-checkbox
                  :input-value="active"
                  color="deep-purple accent-4"
                ></v-checkbox>
              </v-list-item-action>              
              <v-list-item-title>{{ option.title }}</v-list-item-title>
            </v-list-item>
          </v-list>
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
            @click="menuActionClick(item.route)"
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
export default {
  name: "App",
  data() {
    return {
      menu: null,
      items: [
        { title: 'Settings', icon: 'mdi-cog', route: '/' },
      ],
      options: [
        { title: 'Average'},
        { title: 'Minimum'},
        { title: 'Maximum'},
      ],
      drawer: true,
      miniVariant: false,
      visible: false,
    };
  },
  mounted() {
    this.$ajax
      .get("/api/v1/settings/info")
      .then(response => {
        if(response.data.ds18b20_available){
          this.items.push({ title: 'Ds18b20', icon: 'mdi-thermometer', route: '/ds18b20' });
        }
        if(response.data.bmx280_available){
          this.items.push({ title: 'Bmx280', icon: 'mdi-water-percent', route: '/bmx280' });
        }
      })
      .catch(error => {
        console.log(error);
      });
  },
  methods: {
    menuActionClick(route){
      this.visible = (route != '/');
      this.$router.push(route);
    },
    showChart(){
      if(this.$route.name == 'ds18b20') {
        alert(this.$route.name);
        this.$router.push('/ds18b20/chart');
      }
      if(this.$route.name == 'bmx280') {
        this.$router.push('/bmx280/chart');
      }
    }
  }
};
</script>