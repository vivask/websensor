<template>
  <div class="shadow">
    <div class="modalwin">
      <q-inner-loading :showing="visible">
        <q-spinner-gears size="50px" color="primary" />
      </q-inner-loading>
    </div>
  </div>
</template>

<script>
import { ref } from 'vue'

export default {
  setup () {
    const visible = ref(true)

    return {
      visible,
    }
  }
}
</script>

