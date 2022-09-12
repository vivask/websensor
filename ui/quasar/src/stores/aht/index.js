import { defineStore } from 'pinia';
import { useLayoutStore } from 'src/stores/layout';
import axios from 'axios'
import { ref } from 'vue';

export const useAhtStore = defineStore('aht', {

  state: () => ({
    store: useLayoutStore(),
    items_array: [],
  }),

  getters: {
    get_items_array () {
      return this.items_array
    },
  },

  actions: {
    update_items_array() {
      const filter = this.store.get_filter;
      var uri = "/api/v1/aht/read/" + this.store.selected_submenu + "/" + filter;
      if(filter == 'all'){
        //"count items on page:page num"
        uri = "/api/v1/aht/read/" + filter + "/100:1";
        this.store.wait_spinner_show();
      }
      console.log('[AHT] Uri: ',uri);
      //this.items_array = [];
      axios.get(uri)
        .then(response => {
          this.items_array = response.data.items;
          const pages = response.data.pages;
          if(filter != 'all' || pages == 1){
            this.store.wait_spinner_hide();
          }else{
            this.get_next_page_aht(2, pages)
            .then(response => {
              this.store.wait_spinner_hide();
            })
            .catch(error => {
              console.log(error);
              this.store.wait_spinner_hide();
            });
          }
        })
        .catch(error => {
          console.log(error);
          this.store.wait_spinner_hide();
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
