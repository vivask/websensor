<template>
  <q-page padding>
    <div class="q-pa-md">
      <q-table
        title="AHT25"
        dense
        :rows="rows"
        :columns="columns"
        row-key="name"
        :rows-per-page-options="[10, 50, 100, 0]"
      />
    </div>
  </q-page>
</template>

<script>

import { computed } from 'vue'
import { useAhtStore } from 'stores/aht';

  const columns = [
    {
      name: 'date_time',
      required: true,
      label: 'Date',
      align: 'left',
      field: 'date_time',
      sortable: true
    },
    { name: 'temperature', align: 'center', label: 'Temperature (°C)', field: 'temperature', sortable: true },
    { name: 'humidity', label: 'Humidity (%)', field: 'humidity', sortable: true },
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
      this.store.update_items_array();
    }
  }
</script>
