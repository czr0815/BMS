import Vue from 'vue'
import Router from 'vue-router'

Vue.use(Router)

/* Layout */
import Layout from '@/layout'

/**
 * Note: sub-menu only appear when route children.length >= 1
 * Detail see: https://panjiachen.github.io/vue-element-admin-site/guide/essentials/router-and-nav.html
 *
 * hidden: true                   if set true, item will not show in the sidebar(default is false)
 * alwaysShow: true               if set true, will always show the root menu
 *                                if not set alwaysShow, when item has more than one children route,
 *                                it will becomes nested mode, otherwise not show the root menu
 * redirect: noRedirect           if set noRedirect will no redirect in the breadcrumb
 * name:'router-name'             the name is used by <keep-alive> (must set!!!)
 * meta : {
    roles: ['admin','editor']    control the page roles (you can set multiple roles)
    title: 'title'               the name show in sidebar and breadcrumb (recommend set)
    icon: 'svg-name'/'el-icon-x' the icon show in the sidebar
    breadcrumb: false            if set false, the item will hidden in breadcrumb(default is true)
    activeMenu: '/example/list'  if set path, the sidebar will highlight the path you set
  }
 */

/**
 * constantRoutes
 * a base page that does not have permission requirements
 * all roles can be accessed
 */
export const constantRoutes = [
  {
    path: '/login',
    component: () => import('@/views/login/index'),
    hidden: true
  },

  {
    path: '/404',
    component: () => import('@/views/404'),
    hidden: true
  },

  {
    path: "/303",
    text: true,
    title: "平台总览"
  },
  {
    path: '/',
    component: Layout,
    redirect: '/dashboard',
    children: [{
      path: 'dashboard',
      name: 'Dashboard',
      component: () => import('@/views/dashboard/index'),
      meta: { title: '我的首页', icon: 'el-icon-set-up' }
    }]
  },
  {
    path: '/order',
    component: Layout,
    children: [{
      path: '/order',
      name: 'Order',
      component: () => import('@/views/order/index'),
      meta: { title: '订单信息', icon: 'el-icon-copy-document' }
    }]
  },
  {
    path: '/warning',
    component: Layout,
    children: [{
      path: '/warning',
      name: 'Warning',
      component: () => import('@/views/warning/index'),
      meta: { title: '报警信息', icon: 'el-icon-chat-line-round' }
    }]
  },

  {
    path: "/304",
    text: true,
    title: "系统管理"
  },
  {
    path: '/station',
    component: Layout,
    children: [{
      path: '/station',
      name: 'Station',
      component: () => import('@/views/station/index'),
      meta: { title: '储能站管理', icon: 'el-icon-school' }
    }]
  },
  {
    path: '/bcu',
    component: Layout,
    children: [{
      path: '/bcu',
      name: 'BCU',
      component: () => import('@/views/bcu/index'),
      meta: { title: 'BCU管理', icon: 'el-icon-notebook-2' }
    }]
  },
  {
    path: '/bmu',
    component: Layout,
    children: [{
      path: '/bmu',
      name: 'BMU',
      component: () => import('@/views/bmu/index'),
      meta: { title: 'BMU管理', icon: 'el-icon-cpu' }
    }]
  },
  {
    path: '/user',
    component: Layout,
    children: [{
      path: '/user',
      name: 'User',
      component: () => import('@/views/user/index'),
      meta: { title: '用户管理', icon: 'el-icon-user' }
    }]
  },

  {
    path: "/305",
    text: true,
    title: "数据可视化"
  },
  {
    path: '/table',
    component: Layout,
    children: [{
      path: '/table',
      name: 'table',
      component: () => import('@/views/table/index'),
      meta: { title: '数据报表', icon: 'el-icon-data-analysis' }
    }]
  },
  {
    path: '/screen',
    component: Layout,
    children: [{
      path: '/smart',
      name: 'bigScreen',
      component: () => import('@/views/bigscreen/index'),
      meta: { title: '智慧大屏', icon: 'el-icon-news' }
    }]
  },

  {
    path: "/306",
    text: true,
    title: "  "
  },

  {
    path: '/logout',
    component: Layout,
    children: [{
      path: 'dashboard',
      name: 'Dashboard',
      component: () => import('@/views/dashboard/index'),
      meta: { title: '退出登录', icon: 'el-icon-guide' }
    }]
  },

  // 404 page must be placed at the end !!!
  { path: '*', redirect: '/404', hidden: true }
]

const createRouter = () => new Router({
  // mode: 'history', // require service support
  scrollBehavior: () => ({ y: 0 }),
  routes: constantRoutes
})

const router = createRouter()

// Detail see: https://github.com/vuejs/vue-router/issues/1234#issuecomment-357941465
export function resetRouter() {
  const newRouter = createRouter()
  router.matcher = newRouter.matcher // reset router
}

export default router
