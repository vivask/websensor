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
            bmx280Footer: {'items-per-page-options': [10, 50, 100]},
    
        }
    },
    mounted() {
        this.load_data('avg', 'temperature');
    },
    methods: {
        lload_data: function(filter, option){
            const uri = "/api/v1/bmx280/read/" + option + "/" + filter;
            if(filter == 'all'){
                //"count items on page:page num"
                uri = uri + "/10:1"
            }
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
    },  
}  
</script>