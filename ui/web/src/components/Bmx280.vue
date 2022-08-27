<template>
  <v-container>
    <v-layout text-xs-center wrap>
      <v-flex xs12 sm6 offset-sm3>
        <v-card>
          <v-card-text>
            <v-data-table
              :headers="headers"
              :items="items"
              :items-per-page="-1"
              class="elevation-1"
            ></v-data-table>
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
      begin_idx: 0,
      end_idx: 50,
      headers: [
        {
          text: 'Date',
          align: 'start',
          sortable: false,
          value: 'date_time',
        },
        { text: 'Temperature (°C)', value: 'temperature' },
        { text: 'Pressure (kPa)', value: 'pressure' },
        { text: 'Humidity (%)', value: 'humidity' },
      ],
      items: [],
    };
  },
  mounted() {
    this.loadItems();
  },
  methods: {
    loadItems() {
      this.$ajax
        .post("/api/v1/bmx280/read", {
          begin_idx: this.begin_idx,
          end_idx: this.end_idx
        })
        .then(response => {
          console.log(response);
          this.items = response.data.items;
        })
        .catch(error => {
          console.log(error);
        });
    } 
  }
};
</script>
