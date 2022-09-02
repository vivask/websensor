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
            <v-layout text-xs-center wrap>
              <v-flex xs12 sm6 offset-sm3>
                <v-card>
                  <v-card-text>
                    <v-container fluid grid-list-lg>
                      <v-layout row wrap>
                        <v-flex xs7>
                          <v-subheader class="mt-1">System date</v-subheader>
                        </v-flex>
                        <v-flex xs2>
                          <v-text-field v-model="sys_date" class="mt-0" type="date"></v-text-field>
                        </v-flex>
                        <v-flex xs2>
                          <v-text-field v-model="sys_time" class="mt-0" type="time" step="2"></v-text-field>
                        </v-flex>
                        <v-flex xs1>
                          <v-btn fab dark small v-bind:color="sys_set===false ? 'red accent-4' : 'green accent-4'" class="mt-3" @click="save_hwclock">
                            <v-icon dark>mdi-check</v-icon>
                          </v-btn>                 
                        </v-flex>
                        <v-flex xs7>
                          <v-subheader class="mt-1">Begin loging</v-subheader>
                        </v-flex>
                        <v-flex xs2>
                          <v-text-field v-model="begin_date" class="mt-0" type="date"></v-text-field>
                        </v-flex>
                        <v-flex xs2>
                          <v-text-field v-model="begin_time" class="mt-0" type="time" step="2"></v-text-field>
                        </v-flex>
                        <v-flex xs1>
                          <v-btn fab dark small v-bind:color="begin_set===false ? 'red accent-4' : 'green accent-4'" class="mt-3" @click="save_begin">
                            <v-icon dark>mdi-check</v-icon>
                          </v-btn>                 
                        </v-flex>
                        <v-flex xs7>
                          <v-subheader class="mt-1">End loging</v-subheader>
                        </v-flex>
                        <v-flex xs2>
                          <v-text-field v-model="end_date" class="mt-0" type="date"></v-text-field>
                        </v-flex>
                        <v-flex xs2>
                          <v-text-field v-model="end_time" class="mt-0" type="time" step="2"></v-text-field>
                        </v-flex>
                        <v-flex xs1>
                          <v-btn fab dark small v-bind:color="end_set===false ? 'red accent-4' : 'green accent-4'" class="mt-3" @click="save_end">
                            <v-icon dark>mdi-check</v-icon>
                          </v-btn>                 
                        </v-flex>
                      </v-layout>
                    </v-container>
                  </v-card-text>
                </v-card>
              </v-flex>
            </v-layout>
          </v-container>
        </template>

        <template>
          <v-container v-if="ds18b20Visible">
            <v-layout text-xs-center wrap>
              <v-flex xs12 sm6 offset-sm3>
                <v-card>
                  <v-card-text>
                    <v-data-table
                      :headers="ds18b20Headers"
                      :items="ds18b20Items"
                      :items-per-page="-1"
                      class="elevation-1"
                    >
                      <template v-slot:item.temperature="{ item }">
                        <span>{{ parseFloat(item.temperature).toFixed(1) }}</span>
                      </template>
                    </v-data-table>
                  </v-card-text>
                </v-card>
              </v-flex>
            </v-layout>
          </v-container>
        </template>

        <template>
          <v-container v-if="bmx280Visible">
            <v-layout text-xs-center wrap>
              <v-flex xs12 sm6 offset-sm3>
                <v-card>
                  <v-card-text>
                    <v-data-table
                      :headers="bmx280Headers"
                      :items="bmx280Items"
                      :items-per-page="-1"
                      class="elevation-1"
                    >
                      <template v-slot:item.temperature="{ item }">
                        <span>{{ parseFloat(item.temperature).toFixed(1) }}</span>
                      </template>
                      <template v-slot:item.humidity="{ item }">
                        <span>{{ parseFloat(item.humidity).toFixed(1) }}</span>
                      </template>
                      <template v-slot:item.pressure="{ item }">
                        <span>{{ parseFloat(item.pressure*133.322).toFixed(1) }}</span>
                      </template>
                    </v-data-table>
                  </v-card-text>
                </v-card>
              </v-flex>
            </v-layout>
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

export default {
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
      sys_date: '2022-08-24', 
      sys_time: '10:00:00', 
      begin_date: '2022-08-24', 
      begin_time: '10:00:01', 
      end_date: '2022-08-24', 
      end_time: '10:20:00',
      sys_set: false,
      begin_set: false,
      end_set: false,
      rightMenuVisible: false,
      settingsVisible: true,
      ds18b20Visible: false,
      bmx280Visible: false,
      begin_idx: 0,
      end_idx: 50,
      ds18b20Headers: [
        {
          text: 'Date',
          align: 'start',
          sortable: false,
          value: 'date_time',
        },
        { text: 'Temperature (°C)', value: 'temperature' , formatter: this.floatFormat},
      ],
      ds18b20Items: [],
      bmx280Headers: [
        {
          text: 'Date',
          align: 'start',
          sortable: false,
          value: 'date_time',
        },
        { text: 'Temperature (°C)', value: 'temperature' , formatter: this.floatFormat},
        { text: 'Humidity (%)', value: 'humidity' , formatter: this.floatFormat},
        { text: 'Pressure (mmHg)', value: 'pressure' , formatter: this.pressureFormat},
      ],
      bmx280Items: [],
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
        this.refreshSettingsButtons(response);
      })
      .catch(error => {
        console.log(error);
      });
  },
  methods: {
    router(routeName){
      console.log(routeName);
      if(routeName === 'settings'){
        this.ds18b20Visible = false;
        this.bmx280Visible = false;
        this.rightMenuVisible = false;
        this.loadSettings();
        this.settingsVisible = true;
        return;
      }
      if(routeName === 'ds18b20'){
        this.settingsVisible = false;
        this.bmx280Visible = false;
        this.rightMenuVisible = true;
        this.loadDs18b20();
        this.ds18b20Visible = true;
        return;
      }
      if(routeName === 'bmx280'){
        this.settingsVisible = false;
        this.ds18b20Visible = false;
        this.rightMenuVisible = true;
        this.loadBmx280();
        this.bmx280Visible = true;
        return;
      }
    },
    refreshSettingsButtons(response){
        this.sys_date = response.data.sys_date;
        this.sys_time = response.data.sys_time;
        this.begin_date = response.data.begin_date;
        this.begin_time = response.data.begin_time;
        this.end_date = response.data.end_date;
        this.end_time = response.data.end_time;
        this.sys_set = !(this.sys_date==="" && this.sys_time==="");
        this.begin_set =  !(this.begin_date==="" && this.begin_time==="");
        this.end_set =  !(this.end_date==="" && this.end_time==="");
    },
    loadSettings: function(){
      this.$ajax
      .get("/api/v1/settings/info")
      .then(response => {
        this.refreshSettingsButtons(response);
      })
      .catch(error => {
        console.log(error);
      });
    },
    loadDs18b20: function() {
      const uri = "/api/v1/ds18b20/read/" + this.getFilter();
      console.log(uri);
      this.$ajax
        .get(uri)
        .then(response => {
          console.log(response);
          this.ds18b20Items = response.data.items;          
        })
        .catch(error => {
          console.log(error);
        });      
    },
    loadBmx280: function(){
      const uri = "/api/v1/bmx280/read/" + this.getBmx280Option() + "/" + this.getFilter();
      console.log(uri);
      this.$ajax
        .get(uri)
        .then(response => {
          this.bmx280Items = response.data.items;
        })
        .catch(error => {
          console.log(error);
        });
    },
    menuActionClick(index){
      this.menuIndex = index;
      const routeName = this.items[index].name;
      this.router(routeName);
    },
    menuRadioClick(){
      const routeName = this.items[this.menuIndex].name;
      this.router(routeName);
    },
    menuBmx280RadioClick(){
      const routeName = this.items[this.menuIndex].name;
      this.router(routeName);
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
    getBmx280Option(){
      switch(this.radioGroupBmx280){
        case 0:
          return 'temperature';
        case 1:
          return 'humidity';
        case 2:
          return 'pressure';
      }
    },     
    save_hwclock: function() {
      this.$ajax
        .post("/api/v1/settings/hwclock", {
          date: this.sys_date,
          time: this.sys_time
        })
        .then(data => {
          this.sys_set = true;
          console.log(data);
        })
        .catch(error => {
          console.log(error);
        });
      loadSettings();
    },
    save_begin: function() {
      this.$ajax
        .post("/api/v1/settings/begin", {
          date: this.begin_date,
          time: this.begin_time,
        })
        .then(data => {
          this.begin_set = true;
          console.log(data);
        })
        .catch(error => {
          console.log(error);
        });
      loadSettings();
    },
    save_end: function() {
      this.$ajax
        .post("/api/v1/settings/end", {
          date: this.end_date,
          time: this.end_time
        })
        .then(data => {
          this.end_set = true;
          console.log(data);
        })
        .catch(error => {
          console.log(error);
        });
      loadSettings();
    }
  },
  floatFormat(value){
    console.log(value);
    const ret = fixeFloat(value, 1);
    console.log(ret);
    return ret;
  },
  pressureFormat(value){
    return fixeFloat(value/133, 1);
  }
};
</script>