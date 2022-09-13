const routes = [
  {
    path: '/',
    component: () => import('layouts/MainLayout.vue'),
    children: [
      { path: '', component: () => import('pages/SetPage.vue') },
      { path: 'aht', component: () => import('pages/AhtPage.vue') },
      { path: 'bmx280', component: () => import('pages/Bmx280Page.vue') },
      { path: 'ds18b20', component: () => import('pages/Ds18b20Page.vue') }
    ]
  },

  // Always leave this as last one,
  // but you can also remove it
  {
    path: '/:catchAll(.*)*',
    component: () => import('pages/ErrorNotFound.vue')
  }
]

export default routes
