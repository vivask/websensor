<template>
  <q-item
    clickable
    :active="clicked"
    tag="a"
    target="_blank"
    :href="link"
    @click="click(index)"
  >
    <q-item-section
      v-if="icon"
      avatar
    >
      <q-icon :name="icon" :color="iconColor" />
    </q-item-section>

    <q-item-section>
      <q-item-label>{{ title }}</q-item-label>
      <q-item-label caption>{{ caption }}</q-item-label>
    </q-item-section>
  </q-item>
  <q-separator :key="'sep' + title" v-if="separator" />
</template>

<script>
import { defineComponent } from 'vue'

export default defineComponent({
  name: 'EssentialLink',
  props: {
    title: {
      type: String,
      required: true
    },

    caption: {
      type: String,
      default: ''
    },

    link: {
      type: String,
      default: '#'
    },

    icon: {
      type: String,
      default: ''
    },

    iconColor: {
      type: String,
      default: ''
    },

    clicked: {
      type: Boolean,
      default: false
    },

    separator: {
      type: Boolean,
      default: false
    }
  }
})
</script>
