<template>
  <div class="q-pa-md container inner">
      <q-table
        title="BME280"
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
import { useBmx280Store } from 'src/stores/bmx280'
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
    {
      name: 'pressure',
      label: 'Pressure (mmHg)',
      field: 'pressure',
      format: val => parseFloat(val).toFixed(1),
      sortable: true
    },
  ]
  export default {
    setup () {
      const store = useBmx280Store()
      const rows = computed(() => store.get_items_array);
      return {
        store,
        columns,
        rows,
      }
    },
    mounted (){
      const store = useLayoutStore()
      store.set_selected_menu('BME280')
      store.set_selected_submenu(this.$route.query.opt)
      store.set_filter(this.$route.query.filter)
    }
  }
</script>

