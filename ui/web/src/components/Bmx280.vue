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
      load_data: function(){
        const filter = this.$store.getters.get_menu_filter;
        const option = this.$store.getters.get_menu_bmx280;
        var uri = "/api/v1/bmx280/read/" + option + "/" + filter;
        if(filter == 'all'){
          //"count items on page:page num"
          uri = "/api/v1/bmx280/read/" + filter + "/100:1";
        }
        console.log('[BMX280] Uri: ',uri);
        this.$store.commit('clear_items_array');
        this.$ajax
          .get(uri)
          .then(response => {
            this.$store.commit('update_items_array', response.data.items);
            if(filter != 'all'){
              this.bmx280Items = this.$store.getters.get_items_array;  
              this.$modal.hide('wait-spinner');
            }else{
              const pages = response.data.pages;
              this.get_next_page(2, pages)
              .then(response => {
                this.bmx280Items = this.$store.getters.get_items_array;
                this.$modal.hide('wait-spinner');
              })
              .catch(error => {
                console.log(error);
                this.$modal.hide('wait-spinner');
              });
            }
          })
          .catch(error => {
            console.log(error);
            this.$modal.hide('wait-spinner');
          });
      },
      get_next_page(page, pages){
        if(page <= pages){
          const filter = this.$store.getters.get_menu_filter;
          const uri = "/api/v1/bmx280/read/" + filter + "/100:" + page;
          console.log('[BMX280] ', page, '/', pages, ' Uri: ', uri);
          return this.$ajax
            .get(uri)
            .then(response => {
              this.$store.commit('update_items_array', response.data.items);
              page++;
              if(page <= pages){
                return this.get_next_page(page, pages);
              }
            })
            .catch(error => {
              return error;
            });
        }else{
          return Promise.reject(new Error(page + ' is larger then ' + pages));
        }
      }
    },
}  
</script>