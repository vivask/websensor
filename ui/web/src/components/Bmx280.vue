<template>
      <v-layout text-xs-center wrap>
        <v-flex xs12 sm6 offset-sm3>
          <v-card>
            <v-card-text>
              <v-data-table
                :headers="bmx280Headers"
                :items="bmx280Items"
                :items-per-page="10"                      
                :footer-props="bmx280Footer"
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
</template>

<script>
export default {
    data() {
        return {
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
            bmx280Footer: {'items-per-page-options': [10, 50, 100, -1]},
    
        }
    },
    mounted() {
        this.load_data();
    },
    methods: {
        load_data: function() {
          const filter = this.$store.getters.get_menu_filter;
          const option = this.$store.getters.get_menu_bmx280;
          const uri = "/api/v1/bmx280/read/" + option + "/" + filter;
          if(filter == 'all'){
              //"count items on page:page num"
              uri = uri + "/100:1";
          }
          console.log('[BMX280] Uri: ',uri);
          this.$ajax
              .get(uri)
              .then(response => {
                if(filter != 'all'){
                    this.bmx280Headers = response.data.items;
                }else{
                    this.bmx280Headers = response.data.items;
                    const pages = response.data.pages;
                    for(var i=2; i<=pages; i++){
                      uri = "/api/v1/bmx280/read/" + option + "/" + filter + "/100:" + i;
                      this.$ajax
                          .get(uri)
                          .then(response => {
                            this.bmx280Headers = this.bmx280Headers.concat(response.data.items);
                          })
                          .catch(error => {
                            console.log(error);        
                          });
                        }
                }
              })
              .catch(error => {
                console.log(error);
              });      
        },
    },  
}  
</script>