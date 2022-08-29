<template>
  <v-container>
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

<script>
export default {
  data() {
    return { 
      sys_date: '2022-08-24', 
      sys_time: '10:00:00', 
      begin_date: '2022-08-24', 
      begin_time: '10:00:01', 
      end_date: '2022-08-24', 
      end_time: '10:20:00',
      sys_set: false,
      begin_set: false,
      end_set: false 
    };
  },
  mounted() {
    this.$ajax
      .get("/api/v1/settings/info")
      .then(data => {
        /*this.sys_date = data.data.sys_date;
        this.sys_time = data.data.sys_time;
        this.begin_date = data.data.begin_date;
        this.begin_time = data.data.begin_time;
        this.end_date = data.data.end_date;
        this.end_time = data.data.end_time;
        this.sys_set = !(this.sys_date==="" && this.sys_time==="");
        this.begin_set =  !(this.begin_date==="" && this.begin_time==="");
        this.end_set =  !(this.end_date==="" && this.end_time==="");*/
      })
      .catch(error => {
        console.log(error);
      });
  },    
  methods: {
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
    }
  }
};
</script>
