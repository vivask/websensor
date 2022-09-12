<template>
  <div class="q-pa-md container center">
      <q-table
        title="AHT25"
        dense
        :rows="rows"
        :columns="columns"
        row-key="name"
        :rows-per-page-options="[10, 50, 100, 0]"
      />
  </div>
</template>

<script>

import { computed } from 'vue'
import { useAhtStore } from 'src/stores/aht'
import { useLayoutStore } from 'src/stores/layout'

  const columns = [
    {
      name: 'date_time',
      required: true,
      label: 'Date',
      align: 'left',
      field: 'date_time',
      sortable: true
    },
    {
      name: 'temperature',
      align: 'center',
      label: 'Temperature (°C)',
      field: 'temperature',
      format: val => parseFloat(val).toFixed(1),
      sortable: true
    },
    {
      name: 'humidity',
      label: 'Humidity (%)',
      field: 'humidity',
      format: val => parseFloat(val).toFixed(1),
      sortable: true
    },
  ]
  export default {
    setup () {
      const store = useAhtStore()
      const rows = computed(() => store.get_items_array);
      return {
        store,
        columns,
        rows,
      }
    },
    mounted (){
      const store = useLayoutStore()
      //console.log("Filter: ", this.$route.query.filter)
      //console.log("Option: ", this.$route.query.opt)
      store.set_selected_submenu(this.$route.query.opt)
      store.set_filter(this.$route.query.filter)
    }
  }
</script>

