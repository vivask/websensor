<template>
  <div class="q-pa-md container inner">
      <q-table
        title="DS18B20"
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
import { useDs18b20Store } from 'src/stores/ds18b20'
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
  ]
  export default {
    setup () {
      const store = useDs18b20Store()
      const rows = computed(() => store.get_items_array);
      return {
        store,
        columns,
        rows,
      }
    },
    mounted (){
      const store = useLayoutStore()
      store.set_selected_menu('DS18B20')
      store.set_selected_submenu(this.$route.query.opt)
      store.set_filter(this.$route.query.filter)
    }
  }
</script>

