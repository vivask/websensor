import { defineStore } from 'pinia';

export const useSetStore = defineStore('set', {
  state: () => ({
    today: new Date(),
  }),
  getters: {
    get_default_sys_date () {
      return this.today.getFullYear()+'-'+(this.today.getMonth()+1)+'-'+this.today.getDate()
    },
    get_default_sys_time () {
      return this.today.getHours()+':'+this.today.getMinutes()+':'+this.today.getSeconds()
    },
    get_default_begin_date () {
      return this.today.getFullYear()+'-'+(this.today.getMonth()+1)+'-'+this.today.getDate()
    },
    get_default_begin_time () {
      return this.today.getHours()+':'+this.today.getMinutes()+':'+(this.today.getSeconds()+2)
    },
    get_default_end_date () {
      return this.today.getFullYear()+'-'+(this.today.getMonth()+1)+'-'+(this.today.getDate()+1)
    },
    get_default_end_time () {
      return this.today.getHours()+':'+this.today.getMinutes()+':'+this.today.getSeconds()
    }
  }
})
