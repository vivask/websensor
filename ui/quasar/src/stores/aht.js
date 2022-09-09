import { ref } from 'vue'
import { defineStore } from 'pinia';
import { useLayoutStore } from 'stores/layout';
import axios from 'axios'

export const useAhtStore = defineStore('aht', {

  state: () => ({
    store: useLayoutStore(),
    items_array: [
      {'date_time': '2022-09-08 10:00:00', 'temperature': 20.0, 'humidity': 43.0},
      {'date_time': '2022-09-08 10:00:05', 'temperature': 20.5, 'humidity': 43.5}
    ],
  }),

  getters: {
    get_items_array() {
      return this.items_array
    }
  },

  actions: {
    update_items_array() {
      const filter = this.store.get_filter;
      const option = this.store.get_aht_option;
      var uri = "/api/v1/aht/read/" + option + "/" + filter;
      if(filter == 'all'){
        //"count items on page:page num"
        uri = "/api/v1/aht/read/" + filter + "/100:1";
        //this.$modal.show('wait-spinner');
      }
      console.log('[AHT] Uri: ',uri);
      //this.items_array = [];
      axios.get(uri)
        .then(response => {
          this.items_array = response.data.items;
          const pages = response.data.pages;
          if(filter != 'all' || pages == 1){
            //this.$modal.hide('wait-spinner');
          }else{
            this.get_next_page_aht(2, pages)
            .then(response => {
              //this.$modal.hide('wait-spinner');
            })
            .catch(error => {
              console.log(error);
              //this.$modal.hide('wait-spinner');
            });
          }
        })
        .catch(error => {
          console.log(error);
          //this.$modal.hide('wait-spinner');
        });
    },
    get_next_page_aht(page, pages){
      if(page <= pages){
        const filter = this.store.get_filter;
        const uri = "/api/v1/aht/read/" + filter + "/100:" + page;
        console.log('[AHT] ', page, '/', pages, ' Uri: ', uri);
        return axios.get(uri)
          .then(response => {
            this.items_array = this.items_array.concat(response.data.items);
            page++;
            if(page <= pages){
              return this.get_next_page_aht(page, pages);
            }
          })
          .catch(error => {
            return error;
          });
      }else{
        return Promise.reject(new Error(page + ' is larger then ' + pages));
      }
    }
  }
})
