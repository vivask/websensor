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
      ],
      items: [],
    };
  },
  mounted() {
    this.loadItems(this.$route.query.filter);
  },
  methods: {
    loadItems(filter) {
      const uri = "/api/v1/ds18b20/read/" + filter;
      console.log(uri);
      this.$ajax
        .get(uri)
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
