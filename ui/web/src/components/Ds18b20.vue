<template>
      <v-layout text-xs-center wrap>
        <v-flex xs12 sm6 offset-sm3>
          <v-card>
            <v-card-text>
              <v-data-table
                :headers="ds18b20Headers"
                :items="ds18b20Items"
                :items-per-page="ds18b20ItemsPerPage"
                :footer-props="ds18b20Footer"
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
</template>

<script>
export default {
    data() {
        return {
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
            ds18b20ItemsPerPage: 10,
            ds18b20Footer: {'items-per-page-options': [10, 50, 100, -1]},
        }
    },
    mounted() {
        this.load_data(this.$menu_filter);
    },
    methods: {
        load_data: function(filter) {
            var uri = "/api/v1/ds18b20/read/" + filter;
            if(filter == 'all'){
                //"count items on page:page num"
                uri = uri + "/100:1";
            }
            console.log('[DS18B20] Uri: ',uri);
            this.$ajax
                .get(uri)
                .then(response => {
                if(filter != 'all'){
                    this.ds18b20Items = response.data.items;
                }else{
                    this.ds18b20Items = response.data.items;
                    const pages = response.data.pages;
                    for(var i=2; i<=pages; i++){
                    uri = "/api/v1/ds18b20/read/" + filter + "/100:" + i;
                    this.$ajax
                        .get(uri)
                        .then(response => {
                        this.ds18b20Items = this.ds18b20Items.concat(response.data.items);
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
    }      
}  
</script>