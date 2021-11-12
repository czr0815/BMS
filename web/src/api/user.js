import request from '@/utils/request'
import { getToken } from '@/utils/auth'

const token = getToken()

export function login(data) {
  return request({
    //url: '/vue-admin-template/user/login',
    url: '/loginByPW',
    method: 'post',
    data
  })
}

export function getInfo() {
  return request({
    url: '/vue-admin-template/user/info',
    method: 'get',
    params: {
      token: token
    }
  })
}

export function logout() {
  return request({
    url: '/vue-admin-template/user/logout',
    method: 'post'
  })
}

export function updateUser(userForm){
  return request({
    url: '/api/v1/updateUser',
    method: 'post',
    params: {
      token: token
    },
    data: userForm
  })
}

export function sendPhoneCode(telephone){
  return request({
    url: '/sendVerifyCode',
    method: 'get',
    params: {
      telephone: telephone
    }
  })
}
