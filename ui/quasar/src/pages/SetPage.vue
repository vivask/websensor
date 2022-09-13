<template>
  <div class="q-pa-md setting-container center box-shadow">

    <q-form
      @submit="onSubmit"
      @reset="onReset"
      class="q-gutter-md"
    >
    <div class="row">
      <div class="mt-25">System time: </div>
      <div class="q-pa-md right" style="max-width: 300px">
        <q-input filled v-model="sys_time">
          <template v-slot:prepend>
            <q-icon name="event" class="cursor-pointer">
              <q-popup-proxy cover transition-show="scale" transition-hide="scale">
                <q-date v-model="sys_time" mask="YYYY-MM-DD HH:mm:ss">
                  <div class="row items-center justify-end">
                    <q-btn v-close-popup label="Close" color="primary" flat />
                  </div>
                </q-date>
              </q-popup-proxy>
            </q-icon>
          </template>
          <template v-slot:append>
            <q-icon name="access_time" class="cursor-pointer">
              <q-popup-proxy cover transition-show="scale" transition-hide="scale">
                <q-time v-model="sys_time" mask="YYYY-MM-DD HH:mm:ss" format24h with-seconds>
                  <div class="row items-center justify-end">
                    <q-btn v-close-popup label="Close" color="primary" flat />
                  </div>
                </q-time>
              </q-popup-proxy>
            </q-icon>
          </template>
        </q-input>
      </div>
    </div>

    <div class="row">
      <div class="mt-25">Begin time: </div>
      <div class="q-pa-md right" style="max-width: 300px">
        <q-input filled v-model="begin_time">
          <template v-slot:prepend>
            <q-icon name="event" class="cursor-pointer">
              <q-popup-proxy cover transition-show="scale" transition-hide="scale">
                <q-date v-model="begin_time" mask="YYYY-MM-DD HH:mm:ss">
                  <div class="row items-center justify-end">
                    <q-btn v-close-popup label="Close" color="primary" flat />
                  </div>
                </q-date>
              </q-popup-proxy>
            </q-icon>
          </template>
          <template v-slot:append>
            <q-icon name="access_time" class="cursor-pointer">
              <q-popup-proxy cover transition-show="scale" transition-hide="scale">
                <q-time v-model="begin_time" mask="YYYY-MM-DD HH:mm:ss" format24h with-seconds>
                  <div class="row items-center justify-end">
                    <q-btn v-close-popup label="Close" color="primary" flat />
                  </div>
                </q-time>
              </q-popup-proxy>
            </q-icon>
          </template>
        </q-input>
      </div>
    </div>

    <div class="row">
      <div class="mt-25">End time: </div>
      <div class="q-pa-md right" style="max-width: 300px">
        <q-input filled v-model="end_time">
          <template v-slot:prepend>
            <q-icon name="event" class="cursor-pointer">
              <q-popup-proxy cover transition-show="scale" transition-hide="scale">
                <q-date v-model="end_time" mask="YYYY-MM-DD HH:mm:ss">
                  <div class="row items-center justify-end">
                    <q-btn v-close-popup label="Close" color="primary" flat />
                  </div>
                </q-date>
              </q-popup-proxy>
            </q-icon>
          </template>
          <template v-slot:append>
            <q-icon name="access_time" class="cursor-pointer">
              <q-popup-proxy cover transition-show="scale" transition-hide="scale">
                <q-time v-model="end_time" mask="YYYY-MM-DD HH:mm:ss" format24h with-seconds>
                  <div class="row items-center justify-end">
                    <q-btn v-close-popup label="Close" color="primary" flat />
                  </div>
                </q-time>
              </q-popup-proxy>
            </q-icon>
          </template>
        </q-input>
      </div>
    </div>

      <div>
        <q-btn label="Submit" type="submit" color="primary" />
        <q-btn label="Reset" type="reset" color="primary" flat class="q-ml-sm" />
      </div>
    </q-form>

  </div>
</template>

<script>
import { ref } from 'vue'
import axios from 'axios'
import { useLayoutStore } from 'src/stores/layout'
import { date } from 'quasar'

export default {
  setup () {
    const today = new Date()
    const sys_time = ref(date.formatDate(today, 'YYYY-MM-DD HH:mm:ss'))
    const begin = new Date(today.getTime() + 5*1000)
    const begin_time = ref(date.formatDate(begin, 'YYYY-MM-DD HH:mm:ss'))
    const end = new Date(today.getTime() + 24*60*60*1000)
    const end_time  = ref(date.formatDate(end, 'YYYY-MM-DD HH:mm:ss'))

    return {
      sys_time,
      begin_time,
      end_time,

      onReset () {
        sys_time.value = null
        begin_time.value = null
        end_time.value = null
      }
    }
  },
  mounted () {
    axios.get("/api/v1/settings/info")
        .then(response => {
          if(response.data.sys_time.length > 0){
            this.sys_time = response.data.sys_time
            this.begin_time = response.data.begin_time
            this.end_time = response.data.end_time
          }
        })
        .catch(error => {
          console.log(error);
        });
  },
  methods: {
    onSubmit () {
      const layout = useLayoutStore()
      layout.gear_spinner_show()
      axios.post("/api/v1/settings/hwclock", {
            sys_time: this.sys_time,
            begin_time: this.begin_time,
            end_time: this.end_time,
            })
            .then(response => {
              setTimeout(() => {
                console.log(response.data.message)
                layout.gear_spinner_hide()
                const path = layout.get_first_available_page
                //console.log(path)
                //this.$router.push({ path: path })
              },
              2000)
            })
            .catch(error => {
             layout.gear_spinner_hide()
              console.log(error)
            });
    },
  }
}
</script>
