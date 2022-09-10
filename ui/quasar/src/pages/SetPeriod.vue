<template>
  <div class="q-pa-md setting-container center box-shadow">

    <q-form
      @submit="onSubmit"
      @reset="onReset"
      class="q-gutter-md"
    >
    <div class="row">
      <div class="mt-20">System time: </div>
      <div class="right date-field">
      <q-input
        filled
        v-model="sys_date"
        type="date"
        lazy-rules
        :rules="[ val => val && val.length > 0 || 'Please type something']"
      />
      </div>
      <div class="right time-field">
      <q-input
        filled
        type="time"
        v-model="sys_time"
        lazy-rules
        step="2"
        :rules="[ val => val && val.length > 0 || 'Please type something']"
      />
      </div>
    </div>
    <div class="row">
      <div class="mt-20">Begin loging: </div>
      <div class="right date-field">
      <q-input
        filled
        v-model="begin_date"
        type="date"
        lazy-rules
        :rules="[ val => val && val.length > 0 || 'Please type something']"
      />
      </div>
      <div class="right time-field">
      <q-input
        filled
        type="time"
        v-model="begin_time"
        lazy-rules
        step="2"
        :rules="[ val => val && val.length > 0 || 'Please type something']"
      />
      </div>
    </div>
    <div class="row">
      <div class="mt-20">End loging: </div>
      <div class="right date-field">
      <q-input
        filled
        v-model="end_date"
        type="date"
        lazy-rules
        :rules="[ val => val && val.length > 0 || 'Please type something']"
      />
      </div>
      <div class="right time-field">
      <q-input
        filled
        type="time"
        v-model="end_time"
        lazy-rules
        step="2"
        :rules="[ val => val && val.length > 0 || 'Please type something']"
      />
      </div>
    </div>

      <div>
        <q-btn label="Submit" type="submit" color="primary"/>
        <q-btn label="Reset" type="reset" color="primary" flat class="q-ml-sm" />
      </div>
    </q-form>

  </div>
</template>

<script>
import { ref } from 'vue'
import axios from 'axios'


export default {
  setup () {

    const sys_date = ref(null)
    const sys_time = ref(null)
    const begin_date = ref(null)
    const begin_time = ref(null)
    const end_date = ref(null)
    const end_time = ref(null)

    return {
      sys_date,
      sys_time,
      begin_date,
      begin_time,
      end_date,
      end_time,

      onSubmit () {
        axios.post("/api/v1/settings/hwclock", {
            sys_date: sys_date.value,
            sys_time: sys_time.value,
            begin_date: begin_date.value,
            begin_time: begin_time.value,
            end_date: end_date.value,
            end_time: end_time.value
            })
            .then(data => {
              console.log(data);
            })
            .catch(error => {
              console.log(error);
            });
      },

      onReset () {
        sys_date.value = null
        sys_time.value = null
        begin_date.value = null
        begin_time.value = null
        end_date.value = null
        end_time.value = null
      }
    }
  },
  mounted(){
    axios.get("/api/v1/settings/info")
        .then(response => {
          this.sys_date = response.data.sys_date
          this.sys_time = response.data.sys_time
          this.begin_date = response.data.begin_date
          this.begin_time = response.data.begin_time
          this.end_date = response.data.end_date
          this.end_time = response.data.end_time
        })
        .catch(error => {
          console.log(error);
        });
  }
}
</script>
